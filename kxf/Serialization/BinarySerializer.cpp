#include "kxf-pch.h"
#include "BinarySerializer.h"
#include "kxf/Core/String.h"
#include "kxf/IO/IStream.h"

namespace
{
	uint64_t WriteBuffer(kxf::IOutputStream& stream, const void* buffer, size_t length)
	{
		uint64_t written = stream.Write(buffer, length).LastWrite().ToBytes();
		if (written != length)
		{
			throw kxf::BinarySerializerException("Could not write the required amount of bytes");
		}
		return written;
	}
	uint64_t ReadBuffer(kxf::IInputStream& stream, void* buffer, size_t length)
	{
		uint64_t read = stream.Read(buffer, length).LastRead().ToBytes();
		if (read != length)
		{
			throw kxf::BinarySerializerException("Could not read the required amount of bytes");
		}
		return read;
	}
}

namespace kxf
{
	BinarySerializerException::BinarySerializerException(const char* message)
		:runtime_error(message)
	{
	}
	BinarySerializerException::BinarySerializerException(const wchar_t* message)
		:BinarySerializerException(String(message))
	{
	}
	BinarySerializerException::BinarySerializerException(const std::string& message)
		:runtime_error(message)
	{
	}
	BinarySerializerException::BinarySerializerException(const std::wstring& message)
		:BinarySerializerException(String(message))
	{
	}
	BinarySerializerException::BinarySerializerException(const String& message)
		:runtime_error(message.ToUTF8())
	{
	}
}

namespace kxf::Private
{
	uint64_t BufferBinarySerializer::DoWriteBuffer(IOutputStream& stream, const void* buffer, size_t length) const
	{
		return WriteBuffer(stream, buffer, length);
	}
	uint64_t BufferBinarySerializer::DoReadBuffer(IInputStream& stream, void* buffer, size_t length) const
	{
		return ReadBuffer(stream, buffer, length);
	}

	uint64_t IntBinarySerializer::DoSerializeInteger(IOutputStream& stream, const void* buffer, size_t length, bool isSigned) const
	{
		return WriteBuffer(stream, buffer, length);
	}
	uint64_t IntBinarySerializer::DoDeserializeInteger(IInputStream& stream, void* buffer, size_t length, bool isSigned) const
	{
		return ReadBuffer(stream, buffer, length);
	}

	uint64_t FloatBinarySerializer::DoSerializeFloat(IOutputStream& stream, const void* buffer, size_t length) const
	{
		return WriteBuffer(stream, buffer, length);
	}
	uint64_t FloatBinarySerializer::DoDeserializeFloat(IInputStream& stream, void* buffer, size_t length) const
	{
		return ReadBuffer(stream, buffer, length);
	}

	uint64_t StringBinarySerializer::DoSerializeString(IOutputStream& stream, const void* buffer, size_t length) const
	{
		return Serialization::WriteObject(stream, static_cast<uint64_t>(length)) + WriteBuffer(stream, buffer, length);
	}
	uint64_t StringBinarySerializer::DoDeserializeString(IInputStream& stream, void* buffer, size_t& length) const
	{
		if (buffer)
		{
			// We have the buffer, read 'length' bytes into it
			return ReadBuffer(stream, buffer, length);
		}
		else
		{
			// No buffer provided, read string size
			uint64_t length64 = 0;
			uint64_t read = Serialization::ReadObject(stream, length64);
			length = static_cast<size_t>(length64);

			return read;
		}
	}
}
