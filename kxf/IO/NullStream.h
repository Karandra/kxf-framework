#pragma once
#include "Common.h"
#include "IStream.h"

namespace kxf
{
	class NullStream final: public IStream
	{
		public:
			static IStream& Get()
			{
				static NullStream instanse;
				return instanse;
			}

		public:
			// IStream
			void Close() override
			{
			}

			StreamError GetLastError() const override
			{
				return StreamError::Fail();
			}
			void SetLastError(StreamError lastError) override
			{
			}

			bool IsSeekable() const override
			{
				return false;
			}
			DataSize GetSize() const override
			{
				return {};
			}
	};
}

namespace kxf
{
	class NullInputStream final: public IInputStream
	{
		public:
			static IInputStream& Get()
			{
				static NullInputStream instanse;
				return instanse;
			}

		public:
			// IStream
			void Close() override
			{
			}

			StreamError GetLastError() const override
			{
				return StreamError::Fail();
			}
			void SetLastError(StreamError lastError) override
			{
			}

			bool IsSeekable() const override
			{
				return false;
			}
			DataSize GetSize() const override
			{
				return {};
			}

		public:
			// IInputStream
			bool CanRead() const override
			{
				return false;
			}

			DataSize LastRead() const override
			{
				return {};
			}
			void SetLastRead(DataSize lastRead) override
			{
			}

			std::optional<uint8_t> Peek() override
			{
				return {};
			}
			IInputStream& Read(void* buffer, size_t size) override
			{
				return *this;
			}
			IInputStream& Read(IOutputStream& other) override
			{
				return *this;
			}
			bool ReadAll(void* buffer, size_t size) override
			{
				return false;
			}

			DataSize TellI() const override
			{
				return {};
			}
			DataSize SeekI(DataSize offset, IOStreamSeek seek) override
			{
				return {};
			}
	};
}

namespace kxf
{
	class NullOutputStream final: public IOutputStream
	{
		public:
			static IOutputStream& Get()
			{
				static NullOutputStream instanse;
				return instanse;
			}

		public:
			// IStream
			void Close() override
			{
			}

			StreamError GetLastError() const override
			{
				return StreamError::Fail();
			}
			void SetLastError(StreamError lastError) override
			{
			}

			bool IsSeekable() const override
			{
				return false;
			}
			DataSize GetSize() const override
			{
				return {};
			}

		public:
			DataSize LastWrite() const override
			{
				return {};
			}
			void SetLastWrite(DataSize lastWrite) override
			{
			}

			IOutputStream& Write(const void* buffer, size_t size) override
			{
				return *this;
			}
			IOutputStream& Write(IInputStream& other) override
			{
				return *this;
			}
			bool WriteAll(const void* buffer, size_t size) override
			{
				return false;
			}

			DataSize TellO() const override
			{
				return {};
			}
			DataSize SeekO(DataSize offset, IOStreamSeek seek) override
			{
				return {};
			}

			bool Flush() override
			{
				return false;
			}
			bool SetAllocationSize(DataSize allocationSize) override
			{
				return false;
			}
	};
}
