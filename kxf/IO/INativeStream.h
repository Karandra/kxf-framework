#pragma once
#include "Common.h"
#include "kxf/Core/DateTime.h"
#include "kxf/Core/ErrorCode.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	enum class FileAttribute: uint32_t;
}

namespace kxf
{
	class KXF_API INativeStream: public RTTI::Interface<INativeStream>
	{
		kxf_RTTI_DeclareIID(INativeStream, {0x917d1d69, 0x16a3, 0x494d, {0x91, 0x83, 0x35, 0x6c, 0x4d, 0x39, 0x32, 0x7d}});

		public:
			INativeStream() noexcept = default;
			virtual ~INativeStream() = default;

		public:
			virtual void* GetHandle() const = 0;
			virtual bool AttachHandle(void* handle) = 0;
			virtual bool ReopenHandle(FlagSet<IOStreamAccess> access,
									  FlagSet<IOStreamShare> share,
									  FlagSet<IOStreamFlag> flags = IOStreamFlag::None
			) = 0;
			virtual void* DetachHandle() = 0;

			virtual ErrorCode GetLastNativeError() const = 0;

			virtual FlagSet<FileAttribute> GetAttributes() const = 0;
			virtual bool SetAttributes(FlagSet<FileAttribute> attributes) = 0;

			virtual bool GetTimestamp(DateTime& creationTime, DateTime& modificationTime, DateTime& lastAccessTime) const = 0;
			virtual bool ChangeTimestamp(DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) = 0;

		public:
			DateTime GetCreationTime() const
			{
				DateTime creationTime;
				DateTime modificationTime;
				DateTime lastAccessTime;

				if (GetTimestamp(creationTime, modificationTime, lastAccessTime))
				{
					return creationTime;
				}
				return {};
			}
			bool SetCreationTime(DateTime creationTime)
			{
				return ChangeTimestamp(creationTime, {}, {});
			}

			DateTime GetModificationTime() const
			{
				DateTime creationTime;
				DateTime modificationTime;
				DateTime lastAccessTime;

				if (GetTimestamp(creationTime, modificationTime, lastAccessTime))
				{
					return modificationTime;
				}
				return {};
			}
			bool SetModificationTime(DateTime modificationTime)
			{
				return ChangeTimestamp({}, modificationTime, {});
			}

			DateTime GetLastAccessTime() const
			{
				DateTime creationTime;
				DateTime modificationTime;
				DateTime lastAccessTime;

				if (GetTimestamp(creationTime, modificationTime, lastAccessTime))
				{
					return lastAccessTime;
				}
				return {};
			}
			bool SetLastAccessTime(DateTime lastAccessTime)
			{
				return ChangeTimestamp({}, {}, lastAccessTime);
			}
	};
}
