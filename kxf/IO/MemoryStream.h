#pragma once
#include "Common.h"
#include "IStream.h"
#include "IMemoryStream.h"
#include "MemoryStreamBuffer.h"

namespace kxf
{
	class MemoryInputStream;
	class MemoryOutputStream;
}

namespace kxf
{
	class KXF_API MemoryInputStream: public RTTI::Implementation<MemoryInputStream, IInputStream, IMemoryStream>
	{
		private:
			MemoryStreamBuffer m_StreamBuffer;
			DataSize m_LastRead;
			StreamError m_LastError = StreamErrorCode::Success;

		private:
			void ResetState(StreamErrorCode errorCode = StreamErrorCode::Success)
			{
				m_LastRead = {};
				m_LastError = errorCode;
				m_StreamBuffer.Rewind();
			}

		public:
			MemoryInputStream() noexcept
			{
				m_StreamBuffer.SetStorageFixed();
			}
			MemoryInputStream(const MemoryInputStream&) = delete;
			MemoryInputStream(MemoryInputStream&& other) noexcept
			{
				*this = std::move(other);
			}
			MemoryInputStream(const void* buffer, size_t size) noexcept
			{
				m_StreamBuffer.AttachStorage(buffer, size);
				m_StreamBuffer.SetStorageFixed();
			}
			MemoryInputStream(const void* buffer, const void* end) noexcept
			{
				m_StreamBuffer.AttachStorage(buffer, end);
				m_StreamBuffer.SetStorageFixed();
			}
			MemoryInputStream(const MemoryOutputStream& stream) noexcept;
			MemoryInputStream(MemoryOutputStream&& stream) noexcept;
			MemoryInputStream(MemoryStreamBuffer streamBuffer) noexcept
				:m_StreamBuffer(std::move(streamBuffer))
			{
				m_StreamBuffer.Rewind();
			}
			MemoryInputStream(IInputStream& stream, DataSize size = {})
			{
				m_StreamBuffer.CreateStorage(stream, size ? size.ToBytes() : 0);
				m_StreamBuffer.SetStorageFixed();
			}

		public:
			// IStream
			void Close() noexcept override
			{
				m_StreamBuffer = {};
				m_LastRead = {};
				m_LastError = StreamErrorCode::Success;
			}

			StreamError GetLastError() const noexcept override
			{
				return m_LastError;
			}
			void SetLastError(StreamError lastError) noexcept override
			{
				m_LastError = std::move(lastError);
			}

			bool IsSeekable() const override
			{
				return true;
			}
			DataSize GetSize() const noexcept override
			{
				return m_StreamBuffer.GetBufferSize();
			}

			// IInputStream
			bool CanRead() const noexcept override
			{
				return !m_StreamBuffer.IsNull() && !m_StreamBuffer.IsEndOfStream();
			}

			DataSize LastRead() const noexcept override
			{
				return m_LastRead;
			}
			void SetLastRead(DataSize lastRead) noexcept override
			{
				m_LastRead = lastRead;
			}

			std::optional<uint8_t> Peek() noexcept override
			{
				uint8_t value = 0;
				intptr_t length = sizeof(value);
				if (m_StreamBuffer.Read(&value, length) == length)
				{
					m_StreamBuffer.Seek(-length, IOStreamSeek::FromCurrent);
					return value;
				}
				return {};
			}
			IInputStream& Read(void* buffer, size_t size) noexcept override
			{
				m_LastRead = m_StreamBuffer.Read(buffer, size);
				return *this;
			}
			using IInputStream::Read;

			DataSize TellI() const noexcept override
			{
				return m_StreamBuffer.Tell();
			}
			DataSize SeekI(DataSize offset, IOStreamSeek seek) noexcept override
			{
				return m_StreamBuffer.Seek(static_cast<intptr_t>(offset.ToBytes()), seek);
			}

			// IMemoryStream
			MemoryStreamBuffer DetachStreamBuffer() noexcept override
			{
				ResetState(StreamErrorCode::EndOfStream);
				return std::move(m_StreamBuffer);
			}
			void AttachStreamBuffer(MemoryStreamBuffer streamBuffer) noexcept override
			{
				m_StreamBuffer = std::move(streamBuffer);
				ResetState();
			}

			MemoryStreamBuffer& GetStreamBuffer() noexcept override
			{
				return m_StreamBuffer;
			}
			const MemoryStreamBuffer& GetStreamBuffer() const noexcept override
			{
				return m_StreamBuffer;
			}
			size_t CopyToBuffer(void* buffer, size_t size) const override
			{
				const size_t effectiveSize = std::min(size, m_StreamBuffer.GetBufferSize());
				std::memcpy(buffer, m_StreamBuffer.GetBufferStart(), effectiveSize);

				return effectiveSize;
			}

		public:
			MemoryInputStream& operator=(const MemoryInputStream&) = delete;
			MemoryInputStream& operator=(MemoryInputStream&& other) noexcept
			{
				m_StreamBuffer = std::move(other.m_StreamBuffer);
				m_LastRead = std::move(other.m_LastRead);
				m_LastError = std::move(other.m_LastError);

				return *this;
			}
	};
}

namespace kxf
{
	class KXF_API MemoryOutputStream: public RTTI::Implementation<MemoryInputStream, IOutputStream, IMemoryStream, IReadableOutputStream>
	{
		private:
			MemoryStreamBuffer m_StreamBuffer;
			DataSize m_LastWrite;
			StreamError m_LastError = StreamErrorCode::Success;

		private:
			void ResetState(StreamErrorCode errorCode = StreamErrorCode::Success)
			{
				m_LastWrite = {};
				m_LastError = errorCode;
				m_StreamBuffer.Rewind();
			}

		public:
			MemoryOutputStream() noexcept
			{
				m_StreamBuffer.CreateStorage();
			}
			MemoryOutputStream(void* buffer, size_t size)
			{
				m_StreamBuffer.AttachStorage(buffer, size);
			}
			MemoryOutputStream(void* buffer, const void* end)
			{
				m_StreamBuffer.AttachStorage(buffer, end);
			}
			MemoryOutputStream(MemoryStreamBuffer streamBuffer) noexcept
				:m_StreamBuffer(std::move(streamBuffer))
			{
				m_StreamBuffer.Rewind();
			}
			MemoryOutputStream(MemoryInputStream&& stream) noexcept
				:m_StreamBuffer(std::move(stream.GetStreamBuffer()))
			{
			}
			MemoryOutputStream(const MemoryOutputStream&) = delete;
			MemoryOutputStream(MemoryOutputStream&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			// IStream
			void Close() noexcept override
			{
				m_StreamBuffer = {};
				m_LastWrite = {};
				m_LastError = StreamErrorCode::Success;
			}

			StreamError GetLastError() const noexcept override
			{
				return m_LastError;
			}
			void SetLastError(StreamError lastError) noexcept override
			{
				m_LastError = std::move(lastError);
			}

			bool IsSeekable() const override
			{
				return true;
			}
			DataSize GetSize() const noexcept override
			{
				return m_StreamBuffer.GetBufferSize();
			}

			// IOutputStream
			DataSize LastWrite() const noexcept override
			{
				return m_LastWrite;
			}
			void SetLastWrite(DataSize lastWrite) noexcept override
			{
				m_LastWrite = lastWrite;
			}

			IOutputStream& Write(const void* buffer, size_t size) override
			{
				m_LastWrite = m_StreamBuffer.Write(buffer, size);
				return *this;
			}
			using IOutputStream::Write;

			DataSize TellO() const noexcept override
			{
				return m_StreamBuffer.Tell();
			}
			DataSize SeekO(DataSize offset, IOStreamSeek seek) noexcept override
			{
				return m_StreamBuffer.Seek(static_cast<intptr_t>(offset.ToBytes()), seek);
			}

			bool Flush() noexcept override
			{
				return false;
			}
			bool SetAllocationSize(DataSize offset) override
			{
				return m_StreamBuffer.ReserveStorage(offset.ToBytes());
			}

			// IMemoryStream
			MemoryStreamBuffer DetachStreamBuffer() noexcept override
			{
				ResetState(StreamErrorCode::EndOfStream);
				return std::move(m_StreamBuffer);
			}
			void AttachStreamBuffer(MemoryStreamBuffer streamBuffer) noexcept override
			{
				m_StreamBuffer = std::move(streamBuffer);
				ResetState();
			}

			MemoryStreamBuffer& GetStreamBuffer() noexcept override
			{
				return m_StreamBuffer;
			}
			const MemoryStreamBuffer& GetStreamBuffer() const noexcept override
			{
				return m_StreamBuffer;
			}
			size_t CopyToBuffer(void* buffer, size_t size) const override
			{
				const size_t effectiveSize = std::min(size, m_StreamBuffer.GetBufferSize());
				std::memcpy(buffer, m_StreamBuffer.GetBufferStart(), effectiveSize);

				return effectiveSize;
			}

			// IReadableOutputStream
			std::unique_ptr<IInputStream> CreateInputStream() const override;

		public:
			MemoryOutputStream& operator=(const MemoryOutputStream&) = delete;
			MemoryOutputStream& operator=(MemoryOutputStream&& other) noexcept
			{
				m_StreamBuffer = std::move(other.m_StreamBuffer);
				m_LastWrite = std::move(other.m_LastWrite);
				m_LastError = std::move(other.m_LastError);

				return *this;
			}
	};
}
