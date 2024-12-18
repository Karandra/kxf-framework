#pragma once
#include "Common.h"
#include "IFileSystem.h"
#include "FileItem.h"
#include "StorageVolume.h"
#include "kxf/Core/CallbackFunction.h"

namespace kxf::FileSystem::Private
{
	class PathResolver;
	class NativeDirectoryEnumerator;
}

namespace kxf
{
	class KXF_API NativeFileSystem: public RTTI::Implementation<NativeFileSystem, IFileSystem, IFileSystemWithID>
	{
		private:
			friend class kxf::FileSystem::Private::PathResolver;
			friend class kxf::FileSystem::Private::NativeDirectoryEnumerator;

		public:
			static FSPath GetCurrentModuleRootDirectory();
			static FSPath GetExecutingModuleRootDirectory();
			static FSPath GetExecutingModuleWorkingDirectory();
			static bool SetExecutingModuleWorkingDirectory(const FSPath& directory);

		protected:
			StorageVolume m_LookupVolume;
			FSPath m_LookupDirectory;
			bool m_AllowUnqualifiedPaths = false;

		private:
			void DoAssingLookupVolume(StorageVolume volume) noexcept
			{
				m_LookupVolume = std::move(volume);
				m_LookupDirectory.SetVolume(m_LookupVolume);
			}
			void DoAssingLookupVolumeUUID(const UniversallyUniqueID& scope) noexcept
			{
				m_LookupVolume = scope;
				m_LookupDirectory.SetVolume(m_LookupVolume);
			}
			void DoAssingLookupDirectory(FSPath directory) noexcept
			{
				m_LookupVolume = directory.GetStorageVolume();
				m_LookupDirectory = std::move(directory);
			}

		public:
			NativeFileSystem() = default;
			NativeFileSystem(StorageVolume volume)
			{
				DoAssingLookupVolume(std::move(volume));
			}
			NativeFileSystem(FSPath directory)
			{
				DoAssingLookupDirectory(std::move(directory));
			}
			NativeFileSystem(const UniversallyUniqueID& scope)
			{
				DoAssingLookupVolumeUUID(scope);
			}

		public:
			// IFileSystem
			bool IsNull() const override
			{
				return false;
			}

			bool IsValidPathName(const FSPath& path) const override;
			String GetForbiddenPathNameCharacters(const String& except = {}) const override;

			bool IsLookupScoped() const override
			{
				return m_LookupDirectory || m_LookupVolume;
			}
			FSPath ResolvePath(const FSPath& relativePath) const override;
			FSPath GetLookupDirectory() const override
			{
				return m_LookupDirectory;
			}

			FileItem GetItem(const FSPath& path) const override;
			Enumerator<FileItem> EnumItems(const FSPath& directory, const FSPath& query = {}, FlagSet<FSActionFlag> flags = {}) const override;

			bool CreateDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) override;
			bool ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes) override;
			bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) override;

			bool CopyItem(const FSPath& source, const FSPath& destination, CallbackFunction<DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {}) override;
			bool MoveItem(const FSPath& source, const FSPath& destination, CallbackFunction<DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {}) override;
			bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSActionFlag> flags = {}) override;
			bool RemoveItem(const FSPath& path) override;
			bool RemoveDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) override;

			std::shared_ptr<IStream> CreateStream(const FSPath& path,
												  FlagSet<IOStreamAccess> access,
												  IOStreamDisposition disposition,
												  FlagSet<IOStreamShare> share = IOStreamShare::Read,
												  FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
												  FlagSet<FSActionFlag> actionFlags = FSActionFlag::None
			) override;
			using IFileSystem::OpenToRead;
			using IFileSystem::OpenToWrite;

		public:
			// IFileSystemWithID
			UniversallyUniqueID GetLookupScope() const override
			{
				return m_LookupVolume.GetUniqueID();
			}

			FileItem GetItem(const UniversallyUniqueID& id) const override;
			Enumerator<FileItem> EnumItems(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags = {}) const override;

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
			) override;
			using IFileSystemWithID::OpenToRead;
			using IFileSystemWithID::OpenToWrite;

		public:
			// NativeFileSystem
			StorageVolume GetLookupVolume() const noexcept
			{
				return m_LookupVolume;
			}
			void SetLookupVolume(StorageVolume volume) noexcept
			{
				DoAssingLookupVolume(std::move(volume));
			}
			void SetLookupVolume(const UniversallyUniqueID& scope) noexcept
			{
				DoAssingLookupVolumeUUID(scope);
			}
			void SetLookupDirectory(FSPath directory) noexcept
			{
				DoAssingLookupDirectory(std::move(directory));
			}

			bool UnqualifiedPathsAllowed() const noexcept
			{
				return m_AllowUnqualifiedPaths;
			}
			void AllowUnqualifiedPaths(bool allow = true) noexcept
			{
				m_AllowUnqualifiedPaths = allow;
			}

			bool IsInUse(const FSPath& path) const;
			CallbackResult<size_t> EnumStreams(const FSPath& path, CallbackFunction<String, DataSize> func) const;

			CallbackResult<bool> CopyDirectoryTree(const FSPath& source, const FSPath& destination, CallbackFunction<FSPath, FSPath, DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {});
			CallbackResult<bool> MoveDirectoryTree(const FSPath& source, const FSPath& destination, CallbackFunction<FSPath, FSPath, DataSize, DataSize> func = {}, FlagSet<FSActionFlag> flags = {});
	};
}
