#include "kxf-pch.h"
#include "LZ4Stream.h"
#include <lz4.h>

namespace
{
	constexpr const int g_MaxDictionarySize = 64;
	constexpr const size_t g_BlockSize = 1024 * 64;

	LZ4_stream_t* AsCompressorStream(void* stream)
	{
		return reinterpret_cast<LZ4_stream_t*>(stream);
	}
	LZ4_streamDecode_t* AsDecompressorStream(void* stream)
	{
		return reinterpret_cast<LZ4_streamDecode_t*>(stream);
	}
}

namespace kxf::Compression::LZ4
{
	String GetLibraryName()
	{
		return "LZ4";
	}
	Version GetLibraryVersion()
	{
		return LZ4_versionString();
	}

	size_t Compress(const void* sourceBuffer, size_t sourceSize, void* destinationBuffer, size_t destinationSize)
	{
		int size = LZ4_compress_default(reinterpret_cast<const char*>(sourceBuffer), reinterpret_cast<char*>(destinationBuffer), sourceSize, destinationSize);
		return size > 0 ? size : 0;
	}
	std::vector<uint8_t> Compress(const void* sourceBuffer, size_t sourceSize)
	{
		std::vector<uint8_t> destinationBuffer;
		destinationBuffer.resize(CompressBound(sourceSize));

		size_t resultSize = Compress(sourceBuffer, sourceSize, destinationBuffer.data(), destinationBuffer.size());
		destinationBuffer.resize(resultSize);

		return destinationBuffer;
	}

	size_t Decompress(const void* sourceBuffer, size_t sourceSize, void* destinationBuffer, size_t destinationSize)
	{
		int size = LZ4_decompress_safe(reinterpret_cast<const char*>(sourceBuffer), reinterpret_cast<char*>(destinationBuffer), sourceSize, destinationSize);
		return size > 0 ? size : 0;
	}
	std::vector<uint8_t> Decompress(const void* sourceBuffer, size_t sourceSize)
	{
		std::vector<uint8_t> destinationBuffer;
		destinationBuffer.resize(sourceSize);

		size_t resultSize = Decompress(sourceBuffer, sourceSize, destinationBuffer.data(), destinationBuffer.size());
		destinationBuffer.resize(resultSize);

		return destinationBuffer;
	}
}

namespace kxf
{
	bool LZ4BaseStream::SetDictionary(const void* data, size_t size)
	{
		if (m_LZ4Stream)
		{
			return LZ4_loadDict(AsCompressorStream(m_LZ4Stream), reinterpret_cast<const char*>(data), size) <= g_MaxDictionarySize;
		}
		return false;
	}
}

namespace kxf
{
	void LZ4InputStream::Init()
	{
		// Set stream pointer
		SetStream(m_StreamObject);

		// Init stream with or without dictionary
		if (!m_Dictionary.empty())
		{
			LZ4_setStreamDecode(reinterpret_cast<LZ4_streamDecode_t*>(m_StreamObject), reinterpret_cast<const char*>(m_Dictionary.data()), m_Dictionary.size());
		}
		else
		{
			LZ4_setStreamDecode(reinterpret_cast<LZ4_streamDecode_t*>(m_StreamObject), nullptr, 0);
		}
	}
	IInputStream& LZ4InputStream::Read(void* buffer, size_t size)
	{
		m_LastRead = {};
		m_LastError = {};

		size_t totalProcessed = 0;
		for (size_t compressedCounter = 0; compressedCounter <= size; compressedCounter += ms_BlockSize)
		{
			uint8_t tempBuffer[Compression::LZ4::CompressBound(ms_BlockSize)] = {0};
			m_Stream->Read(tempBuffer, std::min(size, ms_BlockSize));

			void* ringBuffer = GetBuffer(m_RingBufferIndex);
			m_RingBufferIndex = NextBufferIndex(m_RingBufferIndex);

			int processedSize = LZ4_decompress_safe_continue(GetStream<LZ4_streamDecode_t>(),
															 reinterpret_cast<const char*>(tempBuffer),
															 reinterpret_cast<char*>(ringBuffer),
															 m_Stream->LastRead().ToBytes(),
															 ms_BlockSize);

			if (processedSize > 0)
			{
				std::memcpy(reinterpret_cast<uint8_t*>(buffer) + totalProcessed, tempBuffer, processedSize);
				totalProcessed += (size_t)processedSize;
				m_LastRead = totalProcessed;

				continue;
			}

			m_LastError = StreamErrorCode::EndOfStream;
			break;
		}
		return *this;
	}
}
