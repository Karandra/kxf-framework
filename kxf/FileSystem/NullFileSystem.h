#pragma once
#include "Common.h"
#include "IFileSystem.h"
#include "FileItem.h"
#include "kxf/Core/Enumerator.h"

namespace kxf::FileSystem
{
	class NullFileSystem final: public RTTI::Implementation<NullFileSystem, IFileSystem, IFileSystemWithID>
	{
		public:
			NullFileSystem() noexcept = default;

		public:
			// IFileSystem
			bool IsNull() const override
			{
				return true;
			}

			bool IsValidPathName(const FSPath& path) const override
			{
				return false;
			}
			String GetForbiddenPathNameCharacters(const String& except = {}) const override
			{
				return {};
			}

			bool IsLookupScoped() const override
			{
				return false;
			}
			FSPath GetLookupDirectory() const override
			{
				return {};
			}
			FSPath ResolvePath(const FSPath& relativePath) const override
			{
				return {};
			}

			FileItem GetItem(const FSPath& path) const override
			{
				return {};
			}
			Enumerator<FileItem> EnumItems(const FSPath& directory, const FSPath& query = {}, FlagSet<FSActionFlag> flags = {}) const override
			{
				return {};
			}
			bool IsDirectoryEmpty(const FSPath& directory) const override
			{
				return false;
			}

			bool CreateDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes) override
			{
				return false;
			}
			bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) override
			{
				return false;
			}

			bool CopyItem(const FSPath& source, const FSPath& destination, CallbackFunction<DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool MoveItem(const FSPath& source, const FSPath& destination, CallbackFunction<DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool RemoveItem(const FSPath& path) override
			{
				return false;;
			}
			bool RemoveDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}

			std::shared_ptr<IStream> CreateStream(const FSPath& path,
												  FlagSet<IOStreamAccess> access,
												  IOStreamDisposition disposition,
												  FlagSet<IOStreamShare> share = IOStreamShare::Read,
												  FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
												  FlagSet<FSActionFlag> actionFlags = FSActionFlag::None
			) override
			{
				return nullptr;
			}

		public:
			// IFileSystemWithID
			UniversallyUniqueID GetLookupScope() const override
			{
				return {};
			}

			FileItem GetItem(const UniversallyUniqueID& id) const override
			{
				return {};
			}
			Enumerator<FileItem> EnumItems(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags = {}) const override
			{
				return {};
			}
			bool IsDirectoryEmpty(const UniversallyUniqueID& id) const override
			{
				return false;
			}

			bool ChangeAttributes(const UniversallyUniqueID& id, FlagSet<FileAttribute> attributes) override
			{
				return false;
			}
			bool ChangeTimestamp(const UniversallyUniqueID& id, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime)  override
			{
				return false;
			}

			bool CopyItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, CallbackFunction<DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool MoveItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, CallbackFunction<DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool RemoveItem(const UniversallyUniqueID& id) override
			{
				return false;
			}
			bool RemoveDirectory(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}

			std::shared_ptr<IStream> CreateStream(const UniversallyUniqueID& id,
												  FlagSet<IOStreamAccess> access,
												  IOStreamDisposition disposition,
												  FlagSet<IOStreamShare> share = IOStreamShare::Read,
												  FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
												  FlagSet<FSActionFlag> actionFlags = FSActionFlag::None
			) override
			{
				return nullptr;
			}
	};
}
