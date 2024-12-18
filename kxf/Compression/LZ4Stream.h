#pragma once
#include "Common.h"
#include "kxf/Core/Version.h"
#include "kxf/Core/String.h"
#include "kxf/IO/StreamDelegate.h"

namespace kxf::Compression::LZ4
{
	KXF_API String GetLibraryName();
	KXF_API Version GetLibraryVersion();

	inline constexpr size_t CompressBound(size_t sourceSize) noexcept
	{
		// Copied directly from 'LZ4_COMPRESSBOUND' macro
		constexpr const size_t LZ4_MAX_INPUT_SIZE = 0x7E000000u; // 2 113 929 216 bytes
		return sourceSize > LZ4_MAX_INPUT_SIZE ? 0 : sourceSize + (sourceSize / 255) + 16;
	}

	KXF_API size_t Compress(const void* sourceBuffer, size_t sourceSize, void* destinationBuffer, size_t destinationSize);
	KXF_API std::vector<uint8_t> Compress(const void* sourceBuffer, size_t sourceSize);

	KXF_API size_t Decompress(const void* sourceBuffer, size_t sourceSize, void* destinationBuffer, size_t destinationSize);
	KXF_API std::vector<uint8_t> Decompress(const void* sourceBuffer, size_t sourceSize);
}

namespace kxf
{
	class KXF_API LZ4BaseStream
	{
		protected:
			constexpr static const size_t ms_BlockSize = 1024 * 8;

		private:
			uint8_t m_Buffer[2][ms_BlockSize] = {{0}, {0}};
			void* m_LZ4Stream = nullptr;

		protected:
			bool HasStream() const
			{
				return m_LZ4Stream;
			}
			void* GetStream() const
			{
				return m_LZ4Stream;
			}
			template<class T> T* GetStream() const
			{
				return reinterpret_cast<T*>(m_LZ4Stream);
			}
			void SetStream(void* stream)
			{
				m_LZ4Stream = stream;
			}

			void* GetBuffer(size_t index)
			{
				return m_Buffer[index];
			}
			const void* GetBuffer(size_t index) const
			{
				return m_Buffer[index];
			}
			size_t GetBufferSize() const
			{
				return ms_BlockSize;
			}
			
			size_t NextBufferIndex(size_t index)
			{
				return (index + 1) % 2;
			}

		public:
			LZ4BaseStream() = default;
			~LZ4BaseStream() = default;

		public:
			bool SetDictionary(const void* data, size_t size);
			bool SetDictionary(const wxMemoryBuffer& buffer)
			{
				return SetDictionary(buffer.GetData(), buffer.GetDataLen());
			}
	};
}

namespace kxf
{
	class KXF_API LZ4InputStream: public LZ4BaseStream, public InputStreamDelegate
	{
		public:
			using DictionaryBuffer = std::vector<uint8_t>;

		private:
			uint8_t m_StreamObject[64] = {};
			DictionaryBuffer m_Dictionary;
			size_t m_RingBufferIndex = 0;

			DataSize m_LastRead;
			std::optional<StreamError> m_LastError;

		private:
			void Init();

		public:
			LZ4InputStream(IInputStream& stream, const DictionaryBuffer& dictionary = {})
				:InputStreamDelegate(stream), m_Dictionary(dictionary)
			{
				Init();
			}
			LZ4InputStream(std::unique_ptr<IInputStream> stream, const DictionaryBuffer& dictionary = {})
				:InputStreamDelegate(std::move(stream)), m_Dictionary(dictionary)
			{
				Init();
			}

		public:
			// IStream
			StreamError GetLastError() const override
			{
				return m_LastError ? *m_LastError : m_Stream->GetLastError();
			}

			// IInputStream
			DataSize LastRead() const override
			{
				return m_LastRead ? m_LastRead : m_Stream->LastRead();
			}
			IInputStream& Read(void* buffer, size_t size) override;
	};
}

namespace kxf
{
	class KXF_API LZ4OutputStream: public LZ4BaseStream, public OutputStreamDelegate
	{
		private:
			int m_Acceleration = 0;

		public:
			LZ4OutputStream(IOutputStream& stream, int acceleration = 0)
				:OutputStreamDelegate(stream)
			{
				SetAcceleration(acceleration);
			}
			LZ4OutputStream(std::unique_ptr<IOutputStream> stream, int acceleration = 0)
				:OutputStreamDelegate(std::move(stream))
			{
				SetAcceleration(acceleration);
			}

		public:
			int GetAcceleration() const
			{
				return m_Acceleration;
			}
			void SetAcceleration(int value)
			{
				m_Acceleration = std::clamp(value, 0, 9);
			}
	};
}
