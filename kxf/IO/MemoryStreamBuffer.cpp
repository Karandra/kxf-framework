#include "kxf-pch.h"
#include "MemoryStreamBuffer.h"
#include "IStream.h"

namespace
{
	constexpr size_t g_BufferSize = kxf::DataSize::FromKB(8).ToBytes();
}

namespace kxf
{
	void MemoryStreamBuffer::CreateStorage(IInputStream& stream, size_t bytesMax)
	{
		m_Flags.Add(MemoryStreamBufferFlag::OwnStorage);

		size_t initialSize = g_BufferSize;
		if (auto size = stream.GetSize())
		{
			initialSize = size.ToBytes();
			m_BufferStorage.reserve(initialSize);
		}

		size_t totalRead = 0;
		CreateStorage(initialSize);

		while (true)
		{
			uint8_t buffer[g_BufferSize];

			size_t currentChunkSize = std::size(buffer);
			if (bytesMax != 0 && bytesMax - totalRead < currentChunkSize)
			{
				currentChunkSize = bytesMax - totalRead;
			}
			DataSize read = stream.Read(buffer, currentChunkSize).LastRead();

			totalRead += read.ToBytes();
			if (!read || read == 0 || stream.GetLastError() == StreamErrorCode::EndOfStream)
			{
				break;
			}
			else if (Write(buffer, read.ToBytes()) != read.ToBytes())
			{
				break;
			}
		}
		TruncateStorage();
		m_BufferCurrent = m_BufferStart;
	}

	bool MemoryStreamBuffer::ResizeStorage(size_t length)
	{
		if (m_Flags.Contains(MemoryStreamBufferFlag::OwnStorage) && !m_Flags.Contains(MemoryStreamBufferFlag::FixedStorage))
		{
			m_BufferStorage.resize(length);
			UpdateFromStorage();

			if (m_BufferCurrent > m_BufferEnd)
			{
				m_BufferCurrent = m_BufferEnd;
			}
			return true;
		}
		return false;
	}
	bool MemoryStreamBuffer::ReserveStorage(size_t length)
	{
		if (m_Flags.Contains(MemoryStreamBufferFlag::OwnStorage) && !m_Flags.Contains(MemoryStreamBufferFlag::FixedStorage))
		{
			m_BufferStorage.reserve(length);
			UpdateFromStorage();

			return true;
		}
		return false;
	}
	bool MemoryStreamBuffer::TruncateStorage() noexcept
	{
		if (m_Flags.Contains(MemoryStreamBufferFlag::OwnStorage))
		{
			m_BufferStorage.resize(Tell());
			UpdateFromStorage();
			m_BufferCurrent = m_BufferEnd;

			return true;
		}
		return false;
	}

	size_t MemoryStreamBuffer::Seek(intptr_t offset, IOStreamSeek seek)
	{
		switch (seek)
		{
			case IOStreamSeek::FromCurrent:
			{
				m_BufferCurrent += offset;
				break;
			}
			case IOStreamSeek::FromStart:
			{
				m_BufferCurrent = m_BufferStart + offset;
				break;
			}
			case IOStreamSeek::FromEnd:
			{
				m_BufferCurrent = m_BufferEnd - offset;
				break;
			}
		};

		if (m_BufferCurrent > m_BufferEnd && !ResizeStorage(m_BufferCurrent - m_BufferStart))
		{
			// End of stream
			m_BufferCurrent = m_BufferEnd;
		}
		return Tell();
	}

	size_t MemoryStreamBuffer::Read(void* buffer, size_t size) noexcept
	{
		if (m_BufferCurrent && buffer && size != 0)
		{
			size_t bytesToRead = std::min(size, GetBytesLeft());
			std::memcpy(buffer, m_BufferCurrent, bytesToRead);
			m_BufferCurrent += bytesToRead;

			return bytesToRead;
		}
		return 0;
	}
	size_t MemoryStreamBuffer::Read(MemoryStreamBuffer& other)
	{
		size_t readTotal = 0;
		uint8_t buffer[g_BufferSize];

		while (true)
		{
			size_t read = Read(buffer, std::size(buffer));
			if (read == 0)
			{
				break;
			}
			else if (other.Write(buffer, read) != read)
			{
				break;
			}
			readTotal += read;

			if (IsEndOfStream())
			{
				break;
			}
		}
		return readTotal;
	}

	size_t MemoryStreamBuffer::Write(const void* buffer, size_t size) noexcept
	{
		if (!buffer || size == 0)
		{
			return 0;
		}

		auto DoWrite = [this](const void* buffer, size_t size, size_t bytesMax = std::numeric_limits<size_t>::max())
		{
			size_t bytesToWrite = std::min(size, bytesMax);
			std::memcpy(m_BufferCurrent, buffer, bytesToWrite);
			m_BufferCurrent += bytesToWrite;

			return bytesToWrite;
		};

		const size_t bytesLeft = GetBytesLeft();
		if (m_Flags.Contains(MemoryStreamBufferFlag::OwnStorage))
		{
			// See if we need to grow the storage
			if (!m_BufferCurrent)
			{
				// We're using dynamic storage but it's not yet allocated. Do this now.
				m_BufferStorage.resize(size);
				UpdateFromStorage();

				return DoWrite(buffer, size);
			}
			else if (bytesLeft < size)
			{
				if (m_Flags.Contains(MemoryStreamBufferFlag::FixedStorage))
				{
					// Can't reallocate, write what we can
					return DoWrite(buffer, size, bytesLeft);
				}
				else
				{
					// Reallocate and write
					m_BufferStorage.resize(m_BufferStorage.size() + size - bytesLeft);
					UpdateFromStorage();

					return DoWrite(buffer, size);
				}
			}
			else
			{
				// Write without reallocation, we have the required space available
				return DoWrite(buffer, size);
			}
		}
		else if (m_BufferCurrent)
		{
			// We don't own the storage, write as much as we can
			return DoWrite(buffer, size, bytesLeft);
		}
		return 0;
	}
	size_t MemoryStreamBuffer::Write(MemoryStreamBuffer& other)
	{
		return other.Read(*this);
	}

	MemoryStreamBuffer& MemoryStreamBuffer::operator=(const MemoryStreamBuffer& other)
	{
		if (other)
		{
			if (other.UsesOwnStorage())
			{
				// Other buffer own the storage, copy it.
				m_BufferStorage = other.m_BufferStorage;
			}
			else
			{
				// Other buffer doesn't own its storage, make an owning copy
				m_BufferStorage.assign(other.m_BufferStart, other.m_BufferEnd);
			}
			UpdateFromStorage();
		}
		else
		{
			Destroy();
		}

		m_Flags = std::move(other.m_Flags);
		return *this;
	}
	MemoryStreamBuffer& MemoryStreamBuffer::operator=(MemoryStreamBuffer&& other) noexcept
	{
		m_BufferStorage = std::move(other.m_BufferStorage);
		m_Flags = std::move(other.m_Flags);

		UpdateFromStorage();
		other.Destroy();

		return *this;
	}
}
