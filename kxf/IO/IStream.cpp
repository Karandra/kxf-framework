#include "kxf-pch.h"
#include "IStream.h"

namespace
{
	constexpr size_t g_BufferSize = kxf::DataSize::FromKB(64).ToBytes();
}

namespace kxf
{
	IInputStream& IInputStream::Read(IOutputStream& other)
	{
		DataSize readTotal = 0;
		uint8_t buffer[g_BufferSize];

		while (true)
		{
			DataSize read = Read(buffer, std::size(buffer)).LastRead();
			if (!read || read == 0)
			{
				break;
			}
			else if (other.Write(buffer, read.ToBytes()).LastWrite() != read)
			{
				break;
			}

			readTotal += read;
			if (GetLastError() == StreamErrorCode::EndOfStream)
			{
				break;
			}
		}
		SetLastRead(readTotal);

		return *this;
	}
	bool IInputStream::ReadAll(void* buffer, size_t size)
	{
		DataSize readTotal = 0;
		size_t bufferOffset = 0;

		while (true)
		{
			const DataSize lastRead = Read(static_cast<uint8_t*>(buffer) + bufferOffset, size - bufferOffset).LastRead();

			// There is no point in continuing looping if we can't read anything at all
			if (!lastRead || lastRead == 0)
			{
				break;
			}
			readTotal += lastRead;

			// Return successfully if we read exactly the requested number of bytes (normally the ">" case
			// should never occur and so we could use "==" test, but be safe and avoid overflowing size even
			// in case of bugs in 'LastRead').
			if (lastRead >= size)
			{
				size = 0;
				break;
			}

			// Advance the buffer before trying to read the rest of data
			bufferOffset += lastRead.ToBytes();

			// If an error occurred on the stream - break
			if (GetLastError().IsFail())
			{
				break;
			}
		}
		SetLastRead(readTotal);

		return size == bufferOffset;
	}
}

namespace kxf
{
	IOutputStream& IOutputStream::Write(IInputStream& other)
	{
		other.Read(*this);
		return *this;
	}
	bool IOutputStream::WriteAll(const void* buffer, size_t size)
	{
		// This function mirrors implementation of 'IInputStream::ReadAll', look there for more details.

		DataSize writtenTotal = 0;
		size_t bufferOffset = 0;

		while (true)
		{
			const DataSize lastWrite = Write(buffer, size).LastWrite();
			if (!lastWrite || lastWrite == 0)
			{
				break;
			}
			writtenTotal += lastWrite;

			if (lastWrite >= size)
			{
				size = 0;
				break;
			}
			bufferOffset += lastWrite.ToBytes();

			// If an error occurred on the stream - break
			if (GetLastError().IsFail())
			{
				break;
			}
		}
		SetLastWrite(writtenTotal);

		return size == bufferOffset;
	}
}
