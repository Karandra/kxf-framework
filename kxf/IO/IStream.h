#pragma once
#include "Common.h"
#include "StreamError.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	class KXF_API IStream: public RTTI::Interface<IStream>
	{
		kxf_RTTI_DeclareIID(IStream, {0x46dac595, 0x145e, 0x400e, {0x8d, 0x7e, 0x10, 0xe0, 0xed, 0xa5, 0x95, 0x27}});

		public:
			IStream() noexcept = default;
			virtual ~IStream() = default;

		public:
			virtual void Close() = 0;

			virtual StreamError GetLastError() const = 0;
			virtual void SetLastError(StreamError lastError) = 0;

			virtual bool IsSeekable() const = 0;
			virtual DataSize GetSize() const = 0;

		public:
			explicit operator bool() const
			{
				return GetLastError().IsSuccess();
			}
			bool operator!() const
			{
				return GetLastError().IsFail();
			}
	};
}

namespace kxf
{
	class KXF_API IInputStream: public RTTI::ExtendInterface<IInputStream, IStream>
	{
		kxf_RTTI_DeclareIID(IInputStream, {0x208dfd90, 0xe250, 0x4916, {0xa6, 0x90, 0x51, 0x12, 0xf9, 0x20, 0x9c, 0x99}});

		public:
			IInputStream() noexcept = default;

		public:
			virtual bool CanRead() const = 0;

			virtual DataSize LastRead() const = 0;
			virtual void SetLastRead(DataSize lastRead) = 0;

			virtual std::optional<uint8_t> Peek() = 0;
			virtual IInputStream& Read(void* buffer, size_t size) = 0;
			virtual IInputStream& Read(IOutputStream& other);
			virtual bool ReadAll(void* buffer, size_t size);

			virtual DataSize TellI() const = 0;
			virtual DataSize SeekI(DataSize offset, IOStreamSeek seek) = 0;
			DataSize RewindI()
			{
				return SeekI(0, IOStreamSeek::FromStart);
			}
	};
}

namespace kxf
{
	class KXF_API IOutputStream: public RTTI::ExtendInterface<IOutputStream, IStream>
	{
		kxf_RTTI_DeclareIID(IOutputStream, {0xf972dabc, 0x284f, 0x44a5, {0xa8, 0x83, 0x7f, 0xd2, 0x3, 0x13, 0x56, 0x1}});

		public:
			IOutputStream() noexcept = default;

		public:
			virtual DataSize LastWrite() const = 0;
			virtual void SetLastWrite(DataSize lastWrite) = 0;

			virtual IOutputStream& Write(const void* buffer, size_t size) = 0;
			virtual IOutputStream& Write(IInputStream& other);
			virtual bool WriteAll(const void* buffer, size_t size);

			virtual DataSize TellO() const = 0;
			virtual DataSize SeekO(DataSize offset, IOStreamSeek seek) = 0;
			DataSize RewindO()
			{
				return SeekO(0, IOStreamSeek::FromStart);
			}

			virtual bool Flush() = 0;
			virtual bool SetAllocationSize(DataSize allocationSize) = 0;
	};
}

namespace kxf
{
	class KXF_API IWriteableInputStream: public RTTI::Interface<IWriteableInputStream>
	{
		kxf_RTTI_DeclareIID(IWriteableInputStream, {0xaad1e815, 0xa527, 0x4efc, {0xbf, 0x7a, 0xa0, 0xa2, 0x3c, 0xc, 0xd2, 0xd7}});

		public:
			virtual std::unique_ptr<IOutputStream> CreateOutputStream() const = 0;
	};

	class KXF_API IReadableOutputStream: public RTTI::Interface<IReadableOutputStream>
	{
		kxf_RTTI_DeclareIID(IReadableOutputStream, {0x905227ef, 0xa85, 0x4b86, {0xb6, 0x34, 0xca, 0x83, 0x11, 0xc3, 0xbd, 0x34}});

		public:
			virtual std::unique_ptr<IInputStream> CreateInputStream() const = 0;
	};
}
