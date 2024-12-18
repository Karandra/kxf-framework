#pragma once
#include "Common.h"

namespace kxf
{
	class IInputStream;

	enum class MemoryStreamBufferFlag: uint32_t
	{
		None = 0,

		OwnStorage = 1 << 0,
		FixedStorage = 1 << 1
	};
	kxf_FlagSet_Declare(MemoryStreamBufferFlag);
}

namespace kxf
{
	class KXF_API MemoryStreamBuffer final
	{
		private:
			uint8_t* m_BufferStart = nullptr;
			uint8_t* m_BufferEnd = nullptr;
			uint8_t* m_BufferCurrent = nullptr;
			std::vector<uint8_t> m_BufferStorage;
			FlagSet<MemoryStreamBufferFlag> m_Flags;

		private:
			void Destroy() noexcept
			{
				m_BufferStart = nullptr;
				m_BufferCurrent = nullptr;
				m_BufferEnd = nullptr;
				m_BufferStorage.clear();
			}
			void UpdateFromStorage() noexcept
			{
				if (m_Flags.Contains(MemoryStreamBufferFlag::OwnStorage))
				{
					// Save the difference so we can restore the current position
					const size_t diff = m_BufferCurrent - m_BufferStart;

					m_BufferStart = m_BufferStorage.data();
					m_BufferEnd = m_BufferStorage.data() + m_BufferStorage.size();
					m_BufferCurrent = m_BufferStart + diff;
				}
				else
				{
					Destroy();
				}
			}
			void DoSetStorage(const void* start, const void* end, bool copy)
			{
				Destroy();

				if (start != end)
				{
					if (copy)
					{
						m_BufferStorage.assign(reinterpret_cast<const uint8_t*>(start), reinterpret_cast<const uint8_t*>(end));
						UpdateFromStorage();
					}
					else
					{
						m_BufferStart = reinterpret_cast<uint8_t*>(const_cast<void*>(start));
						m_BufferEnd = reinterpret_cast<uint8_t*>(const_cast<void*>(end));
					}
					m_BufferCurrent = m_BufferStart;
				}
			}

		public:
			MemoryStreamBuffer() noexcept = default;
			MemoryStreamBuffer(const MemoryStreamBuffer& other)
			{
				*this = other;
			}
			MemoryStreamBuffer(MemoryStreamBuffer&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			bool IsNull() const noexcept
			{
				return m_BufferStart == nullptr || m_BufferEnd == nullptr || m_BufferCurrent == nullptr;
			}
			bool IsEndOfStream() const
			{
				return m_BufferCurrent >= m_BufferEnd;
			}
			bool UsesOwnStorage() const
			{
				return m_Flags.Contains(MemoryStreamBufferFlag::OwnStorage);
			}

			bool IsStorageFixed() const noexcept
			{
				return m_Flags.Contains(MemoryStreamBufferFlag::FixedStorage);
			}
			void SetStorageFixed(bool fixed = true) noexcept
			{
				m_Flags.Mod(MemoryStreamBufferFlag::FixedStorage, fixed);
			}

			void CreateStorage() noexcept
			{
				m_Flags.Add(MemoryStreamBufferFlag::OwnStorage);
			}
			void CreateStorage(size_t length)
			{
				m_Flags.Add(MemoryStreamBufferFlag::OwnStorage);

				m_BufferStorage.resize(length);
				UpdateFromStorage();
			}
			void CreateStorage(IInputStream& stream, size_t bytesMax = 0);
			void CreateStorage(const void* start, const void* end)
			{
				m_Flags.Add(MemoryStreamBufferFlag::OwnStorage);
				DoSetStorage(start, end, true);
			}
			void CreateStorage(const void* start, size_t length)
			{
				m_Flags.Add(MemoryStreamBufferFlag::OwnStorage);
				DoSetStorage(start, reinterpret_cast<const uint8_t*>(start) + length, true);
			}
			void AttachStorage(const void* start, const void* end) noexcept
			{
				m_Flags.Remove(MemoryStreamBufferFlag::OwnStorage);
				DoSetStorage(start, end, false);
			}
			void AttachStorage(const void* start, size_t length) noexcept
			{
				m_Flags.Remove(MemoryStreamBufferFlag::OwnStorage);
				DoSetStorage(start, reinterpret_cast<const uint8_t*>(start) + length, false);
			}

			bool ResizeStorage(size_t length);
			bool ReserveStorage(size_t length);
			bool TruncateStorage() noexcept;

			size_t Tell() const noexcept
			{
				return m_BufferCurrent - m_BufferStart;
			}
			size_t Seek(intptr_t offset, IOStreamSeek seek);
			size_t Rewind() noexcept
			{
				return Seek(0, IOStreamSeek::FromStart);
			}
			size_t GetBytesLeft() const noexcept
			{
				return m_BufferEnd - m_BufferCurrent;
			}
			size_t GetBufferSize() const noexcept
			{
				return m_BufferEnd - m_BufferStart;
			}

			size_t Read(void* buffer, size_t size) noexcept;
			size_t Read(MemoryStreamBuffer& other);

			size_t Write(const void* buffer, size_t size) noexcept;
			size_t Write(MemoryStreamBuffer& other);

			const void* GetBufferStart() const noexcept
			{
				return m_BufferStart;
			}
			void* GetBufferStart() noexcept
			{
				return m_BufferStart;
			}

			const void* GetBufferCurrent() const noexcept
			{
				return m_BufferCurrent;
			}
			void* GetBufferCurrent() noexcept
			{
				return m_BufferCurrent;
			}

			const void* GetBufferEnd() const noexcept
			{
				return m_BufferEnd;
			}
			void* GetBufferEnd() noexcept
			{
				return m_BufferEnd;
			}

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}

			MemoryStreamBuffer& operator=(const MemoryStreamBuffer& other);
			MemoryStreamBuffer& operator=(MemoryStreamBuffer&& other) noexcept;
	};
}
