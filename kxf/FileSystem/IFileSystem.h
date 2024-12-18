#pragma once
#include "Common.h"
#include "FSPath.h"
#include "StorageVolume.h"
#include "kxf/Core/DataSize.h"
#include "kxf/Core/CallbackFunction.h"
#include "kxf/Core/LocallyUniqueID.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/Utility/Common.h"

namespace kxf
{
	class FileItem;

	class IStream;
	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	class KXF_API IFileSystem: public RTTI::Interface<IFileSystem>
	{
		kxf_RTTI_DeclareIID(IFileSystem, {0x950f74ec, 0x6352, 0x47a1, {0xac, 0x8f, 0x43, 0x6a, 0x6, 0x1e, 0x9f, 0x65}});

		public:
			virtual ~IFileSystem() = default;

		public:
			virtual bool IsNull() const = 0;

			virtual bool IsValidPathName(const FSPath& path) const = 0;
			virtual String GetForbiddenPathNameCharacters(const String& except = {}) const = 0;

			virtual bool IsLookupScoped() const = 0;
			virtual FSPath ResolvePath(const FSPath& relativePath) const = 0;
			virtual FSPath GetLookupDirectory() const = 0;

			virtual FileItem GetItem(const FSPath& path) const = 0;
			virtual Enumerator<FileItem> EnumItems(const FSPath& directory, const FSPath& query = {}, FlagSet<FSActionFlag> flags = {}) const = 0;
			virtual bool IsDirectoryEmpty(const FSPath& directory) const;
			
			virtual bool CreateDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes) = 0;
			virtual bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) = 0;

			virtual bool CopyItem(const FSPath& source, const FSPath& destination, CallbackFunction<DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool MoveItem(const FSPath& source, const FSPath& destination, CallbackFunction<DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool RemoveItem(const FSPath& path) = 0;
			virtual bool RemoveDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) = 0;

			virtual std::shared_ptr<IStream> CreateStream(const FSPath& path,
														  FlagSet<IOStreamAccess> access,
														  IOStreamDisposition disposition,
														  FlagSet<IOStreamShare> share = IOStreamShare::Read,
														  FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
														  FlagSet<FSActionFlag> actionFlags = FSActionFlag::None
			) = 0;
			std::shared_ptr<IInputStream> OpenToRead(const FSPath& path,
													 IOStreamDisposition disposition = IOStreamDisposition::OpenExisting,
													 FlagSet<IOStreamShare> share = IOStreamShare::Read,
													 FlagSet<FSActionFlag> flags = FSActionFlag::None
			) const;
			std::shared_ptr<IOutputStream> OpenToWrite(const FSPath& path,
													   IOStreamDisposition disposition = IOStreamDisposition::CreateAlways,
													   FlagSet<IOStreamShare> share = IOStreamShare::Read,
													   FlagSet<FSActionFlag> flags = FSActionFlag::None
			);
	};

	class KXF_API IFileSystemWithID: public RTTI::Interface<IFileSystemWithID>
	{
		kxf_RTTI_DeclareIID(IFileSystemWithID, {0x8a4f7e63, 0x6092, 0x4859, {0xa1, 0x74, 0x25, 0x8, 0x7a, 0x4a, 0x90, 0xcb}});

		public:
			virtual ~IFileSystemWithID() = default;

		public:
			virtual bool IsNull() const = 0;

			virtual bool IsLookupScoped() const = 0;
			virtual UniversallyUniqueID GetLookupScope() const = 0;

			virtual FileItem GetItem(const UniversallyUniqueID& id) const = 0;
			virtual Enumerator<FileItem> EnumItems(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags = {}) const = 0;
			virtual bool IsDirectoryEmpty(const UniversallyUniqueID& id) const;

			virtual bool ChangeAttributes(const UniversallyUniqueID& id, FlagSet<FileAttribute> attributes) = 0;
			virtual bool ChangeTimestamp(const UniversallyUniqueID& id, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) = 0;

			virtual bool CopyItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, CallbackFunction<DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool MoveItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, CallbackFunction<DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool RemoveItem(const UniversallyUniqueID& id) = 0;
			virtual bool RemoveDirectory(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags = {}) = 0;

			virtual std::shared_ptr<IStream> CreateStream(const UniversallyUniqueID& id,
														  FlagSet<IOStreamAccess> access,
														  IOStreamDisposition disposition,
														  FlagSet<IOStreamShare> share = IOStreamShare::Read,
														  FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
														  FlagSet<FSActionFlag> actionFlags = FSActionFlag::None
			) = 0;
			std::shared_ptr<IInputStream> OpenToRead(const UniversallyUniqueID& id,
													 IOStreamDisposition disposition = IOStreamDisposition::OpenExisting,
													 FlagSet<IOStreamShare> share = IOStreamShare::Read,
													 FlagSet<FSActionFlag> flags = FSActionFlag::None
			) const;
			std::shared_ptr<IOutputStream> OpenToWrite(const UniversallyUniqueID& id,
													   IOStreamDisposition disposition = IOStreamDisposition::CreateAlways,
													   FlagSet<IOStreamShare> share = IOStreamShare::Read,
													   FlagSet<FSActionFlag> flags = FSActionFlag::None
			);
	};
}

namespace kxf::FileSystem
{
	KXF_API IFileSystem& GetNullFileSystem() noexcept;
	KXF_API IFileSystemWithID& GetNullFileSystemWithID() noexcept;
}
