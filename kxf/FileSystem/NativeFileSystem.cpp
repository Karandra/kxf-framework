#include "kxf-pch.h"
#include "NativeFileSystem.h"
#include "FSActionEvent.h"
#include "Private/NativeFSUtility.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/Core/AlignedObjectStorage.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/System/HandlePtr.h"
#include "kxf/IO/NativeFileStream.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/String.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/Utility/RecursiveCollectionEnumerator.h"
#include <wx/filename.h>

namespace
{
	using namespace kxf;

	bool OpenFileByID(const StorageVolume& volume,
					  const UniversallyUniqueID& fileID,
					  NativeFileStream& fileStream,
					  FlagSet<IOStreamAccess> access = IOStreamAccess::ReadAttributes,
					  IOStreamDisposition disposition = IOStreamDisposition::OpenExisting,
					  FlagSet<IOStreamShare> share = IOStreamShare::Everything,
					  FlagSet<IOStreamFlag> flags = IOStreamFlag::AllowDirectories)
	{
		if (fileID && volume)
		{
			NativeFileStream volumeStream(volume.GetDevicePath(), IOStreamAccess::ReadAttributes, IOStreamDisposition::OpenExisting, IOStreamShare::Everything, IOStreamFlag::AllowDirectories);
			if (volumeStream)
			{
				// Some search on Google says that 'FILE_ID_DESCRIPTOR' isn't always 24. it *is* 24 for me on both x64 and x86
				// and it seems to work fine with the size of 24. Still I'm going to make it bigger, just in case.
				AlignedObjectStorage<FILE_ID_DESCRIPTOR, 64, alignof(FILE_ID_DESCRIPTOR)> fileIDDescriptor;
				fileIDDescriptor->dwSize = fileIDDescriptor.size();

				if (System::IsWindowsVersionOrGreater(NamedSystemRelease::Windows8))
				{
					fileIDDescriptor->Type = FILE_ID_TYPE::ExtendedFileIdType;

					auto uuid = fileID.ToInt128();
					std::memcpy(fileIDDescriptor->ExtendedFileId.Identifier, uuid.data(), uuid.size());
				}
				else
				{
					if (LocallyUniqueID luid = fileID.ToLocallyUniqueID())
					{
						fileIDDescriptor->Type = FILE_ID_TYPE::FileIdType;
						fileIDDescriptor->FileId.QuadPart = luid.ToInt();
					}
					else
					{
						return false;
					}
				}

				HANDLE handle = ::OpenFileById(volumeStream.GetHandle(),
											   &fileIDDescriptor,
											   *FileSystem::Private::MapFileAccessMode(access),
											   *FileSystem::Private::MapFileShareMode(share),
											   nullptr,
											   *FileSystem::Private::MapFileFlags(flags));
				if (handle && handle != INVALID_HANDLE_VALUE)
				{
					return fileStream.AttachHandle(handle);
				}
			}
		}
		return false;
	}

	bool DoRemoveFile(const String& path) noexcept
	{
		return ::SetFileAttributesW(path.wc_str(), FILE_ATTRIBUTE_NORMAL) && ::DeleteFileW(path.wc_str());
	}
	bool DoRemoveDirectory(const String& path) noexcept
	{
		return ::SetFileAttributesW(path.wc_str(), FILE_ATTRIBUTE_NORMAL) && ::RemoveDirectoryW(path.wc_str());
	}
}
namespace kxf::FileSystem::Private
{
	class PathResolver final
	{
		private:
			const NativeFileSystem& m_FileSystem;

		public:
			PathResolver(const NativeFileSystem& fs) noexcept
				:m_FileSystem(fs)
			{
			}

		public:
			FSPath Resolve(const FSPath& path) const
			{
				if (!m_FileSystem.IsNull())
				{
					bool isAbsolute = path.IsAbsolute();
					if (m_FileSystem.m_LookupDirectory && !isAbsolute)
					{
						return m_FileSystem.m_LookupDirectory / path;
					}
					else if (isAbsolute || m_FileSystem.UnqualifiedPathsAllowed())
					{
						return path;
					}
				}
				return {};
			}

			template<class TFunc, class R = std::invoke_result_t<TFunc, const FSPath&>>
			requires(std::is_invocable_r_v<R, TFunc, const FSPath&>)
			auto DoWithResolvedPath1(const FSPath& path, TFunc&& func, R&& noneResult = {}) const
			{
				if (!m_FileSystem.IsNull())
				{
					const bool isAbsolute = path.IsAbsolute();

					if (m_FileSystem.m_LookupDirectory && !isAbsolute)
					{
						return std::invoke(func, m_FileSystem.m_LookupDirectory / path);
					}
					else if (isAbsolute || m_FileSystem.UnqualifiedPathsAllowed())
					{
						return std::invoke(func, path);
					}
				}
				return std::forward<R>(noneResult);
			}

			template<class TFunc, class R = std::invoke_result_t<TFunc, const FSPath&, const FSPath&>>
			requires(std::is_invocable_r_v<R, TFunc, const FSPath&, const FSPath&>)
			auto DoWithResolvedPath2(const FSPath& path1, const FSPath& path2, TFunc&& func, R&& noneResult = {}) const
			{
				if (!m_FileSystem.IsNull())
				{
					const bool isAbsolute1 = path1.IsAbsolute();
					const bool isAbsolute2 = path2.IsAbsolute();

					if (m_FileSystem.m_LookupDirectory)
					{
						return std::invoke(func, (isAbsolute1 ? path1 : m_FileSystem.m_LookupDirectory / path1), (isAbsolute2 ? path2 : m_FileSystem.m_LookupDirectory / path2));
					}
					else if ((isAbsolute1 && isAbsolute2) || m_FileSystem.UnqualifiedPathsAllowed())
					{
						return std::invoke(func, path1, path2);
					}
				}
				return std::forward<R>(noneResult);
			}
	};

	class NativeDirectoryEnumerator final: public RecursiveCollectionEnumerator<FileItem, FSPath>
	{
		private:
			FSPath m_Query;
			FlagSet<FSActionFlag> m_Flags;
			bound_handle_ptr<HANDLE, ::FindClose, INVALID_HANDLE_VALUE> m_SearchHandle;

		private:
			String ConstructFullQuery(const FSPath& directory) const
			{
				if (m_Query)
				{
					return (directory / m_Query).GetFullPathTryNS(FSPathNamespace::Win32File);
				}
				else
				{
					return (directory / kxfS("*")).GetFullPathTryNS(FSPathNamespace::Win32File);
				}
			}
			std::optional<FileItem> DoItem(IEnumerator& enumerator, WIN32_FIND_DATAW& findInfo, const FSPath& directory, std::vector<FSPath>& childDirectories)
			{
				// Skip invalid items and current and parent directory links
				if (!FileSystem::Private::IsValidFindItem(findInfo))
				{
					enumerator.SkipCurrent();
					return {};
				}

				// Add directory to the stack if we need to scan child directories
				const bool isDirectory = findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
				if (isDirectory && m_Flags.Contains(FSActionFlag::Recursive))
				{
					FSPath& childDirectory = childDirectories.emplace_back(directory / findInfo.cFileName);
					childDirectory.EnsureNamespaceSet(directory.GetNamespace());
				}

				// Filter files and/or directories
				if ((m_Flags.Contains(FSActionFlag::LimitToFiles) && isDirectory) || (m_Flags.Contains(FSActionFlag::LimitToDirectories) && !isDirectory))
				{
					enumerator.SkipCurrent();
					return {};
				}

				// Fetch the file info
				FileItem fileItem = FileSystem::Private::ConvertFileInfo(findInfo, directory, {}, m_Flags);

				// Make final path relative if needed
				if (m_Flags.Contains(FSActionFlag::RelativePath))
				{
					fileItem.SetFullPath(fileItem.GetFullPath().GetAfter(GetRootPath()));
				}
				return fileItem;
			};

		protected:
			std::optional<FileItem> SearchDirectory(IEnumerator& enumerator, const FSPath& directory, std::vector<FSPath>& childDirectories, bool& isSubTreeDone) override
			{
				if (directory)
				{
					WIN32_FIND_DATAW findInfo = {};
					if (!m_SearchHandle)
					{
						if (m_SearchHandle = FileSystem::Private::CallFindFirstFile(ConstructFullQuery(directory), findInfo, m_Flags & FSActionFlag::CaseSensitive))
						{
							return DoItem(enumerator, findInfo, directory, childDirectories);
						}
					}
					else if (::FindNextFileW(*m_SearchHandle, &findInfo))
					{
						return DoItem(enumerator, findInfo, directory, childDirectories);
					}
					else
					{
						m_SearchHandle = nullptr;
						isSubTreeDone = true;

						// Skip this step if we need to scan subdirectories because otherwise we'd terminate the process
						if (m_Flags.Contains(FSActionFlag::Recursive))
						{
							enumerator.SkipCurrent();
						}
					}
				}
				return {};
			};

		public:
			NativeDirectoryEnumerator() = default;
			NativeDirectoryEnumerator(FSPath rootPath, FSPath query, FlagSet<FSActionFlag> flags)
				:RecursiveCollectionEnumerator(std::move(rootPath)), m_Query(std::move(query)), m_Flags(flags)
			{
			}
	};
}

namespace kxf
{
	FSPath NativeFileSystem::GetCurrentModuleRootDirectory()
	{
		return DynamicLibrary::GetCurrentModule().GetFilePath().GetParent();
	}
	FSPath NativeFileSystem::GetExecutingModuleRootDirectory()
	{
		return DynamicLibrary::GetExecutingModule().GetFilePath().GetParent();
	}
	FSPath NativeFileSystem::GetExecutingModuleWorkingDirectory()
	{
		uint32_t length = ::GetCurrentDirectoryW(0, nullptr);
		if (length != 0)
		{
			String result;
			::GetCurrentDirectoryW(length, Utility::StringBuffer(result, length - 1));

			return FSPath(std::move(result)).EnsureNamespaceSet(FSPathNamespace::Win32File);
		}
		return {};
	}
	bool NativeFileSystem::SetExecutingModuleWorkingDirectory(const FSPath& directory)
	{
		if (directory)
		{
			auto DoChange = [](const FSPath& directory)
			{
				String directoryString = directory.GetFullPathTryNS(FSPathNamespace::Win32File);
				return ::SetCurrentDirectoryW(directoryString.wc_str());
			};

			if (auto app = ICoreApplication::GetInstance())
			{
				FSActionEvent event;
				event.SetSource(GetExecutingModuleWorkingDirectory());
				event.SetDestination(directory);
				event.Allow();

				if (app->ProcessEvent(event, ICoreApplication::EvtExecutingModuleWorkingDirectoryChanged) && !event.IsSkipped())
				{
					return event.IsAllowed() && DoChange(event.GetDestination());
				}
			}
			return DoChange(directory);
		}
		return false;
	}

	// IFileSystem
	bool NativeFileSystem::IsValidPathName(const FSPath& path) const
	{
		// All forbidden characters don't have case variants so we can use case-sensitive comparison
		return path && !path.ContainsAnyOfCharacters(wxFileName::GetForbiddenChars(), true);
	}
	String NativeFileSystem::GetForbiddenPathNameCharacters(const String& except) const
	{
		String forbiddenChars = wxFileName::GetForbiddenChars();
		for (XChar c: except)
		{
			forbiddenChars.Replace(c, NullString);
		}
		return forbiddenChars;
	}

	FSPath NativeFileSystem::ResolvePath(const FSPath& relativePath) const
	{
		return FileSystem::Private::PathResolver(*this).Resolve(relativePath);
	}

	FileItem NativeFileSystem::GetItem(const FSPath& path) const
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath1(path, [](const FSPath& path) -> FileItem
		{
			NativeFileStream fileStream(path, IOStreamAccess::ReadAttributes, IOStreamDisposition::OpenExisting, IOStreamShare::Everything, IOStreamFlag::AllowDirectories);
			if (fileStream)
			{
				return FileSystem::Private::ConvertFileInfo(fileStream.GetHandle());
			}
			return {};
		});
	}
	Enumerator<FileItem> NativeFileSystem::EnumItems(const FSPath& directory, const FSPath& query, FlagSet<FSActionFlag> flags) const
	{
		// Invalid flags combination
		if (flags.Contains(FSActionFlag::LimitToFiles|FSActionFlag::LimitToDirectories))
		{
			return {};
		}

		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath1(directory, [&](FSPath path)
		{
			return FileSystem::Private::NativeDirectoryEnumerator(std::move(path), query, flags);
		});
	}

	bool NativeFileSystem::CreateDirectory(const FSPath& path, FlagSet<FSActionFlag> flags)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath1(path, [&](const FSPath& path) -> bool
		{
			if (flags.Contains(FSActionFlag::Recursive))
			{
				if (FileSystem::Private::GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)
				{
					FSPath finalPath;
					finalPath.ReserveLength(path.GetLength());
					finalPath.SetNamespace(path.GetNamespace());

					bool result = false;
					for (String currentDirectoryName: path.EnumComponents())
					{
						finalPath /= std::move(currentDirectoryName);

						const String currentPath = finalPath.GetFullPathTryNS(FSPathNamespace::Win32File);
						const bool isCreated = ::CreateDirectoryW(currentPath.wc_str(), nullptr);
						const bool alreadyExist = !isCreated && (*Win32Error::GetLastError() == ERROR_ALREADY_EXISTS || finalPath.GetComponentCount() == 1);

						result = isCreated || alreadyExist;
						if (!isCreated && !alreadyExist)
						{
							return false;
						}
					}
					return result;
				}
			}
			else
			{
				const String pathName = path.GetFullPathTryNS(FSPathNamespace::Win32File);
				return ::CreateDirectoryW(pathName.wc_str(), nullptr);
			}
			return false;
		});
	}
	bool NativeFileSystem::ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath1(path, [&](const FSPath& path) -> bool
		{
			if (attributes != FileAttribute::Invalid && path.IsAbsolute())
			{
				String pathString = path.GetFullPathTryNS(FSPathNamespace::Win32File);
				return ::SetFileAttributesW(pathString.wc_str(), *FileSystem::Private::MapFileAttributes(attributes));
			}
			return false;
		});
	}
	bool NativeFileSystem::ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath1(path, [&](const FSPath& path) -> bool
		{
			if ((creationTime.IsValid() || modificationTime.IsValid() || lastAccessTime.IsValid()))
			{
				const IOStreamFlag streamFlags = GetItem(path).IsDirectory() ? IOStreamFlag::AllowDirectories : IOStreamFlag::Normal;
				NativeFileStream stream(path, IOStreamAccess::WriteAttributes, IOStreamDisposition::OpenExisting, IOStreamShare::Everything, streamFlags);
				if (stream)
				{
					return stream.ChangeTimestamp(creationTime, modificationTime, lastAccessTime);
				}
			}
			return false;
		});
	}

	bool NativeFileSystem::CopyItem(const FSPath& source, const FSPath& destination, CallbackFunction<DataSize, DataSize> func, FlagSet<FSActionFlag> flags)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath2(source, destination, [&](const FSPath& source, const FSPath& destination) -> bool
		{
			FlagSet<DWORD> copyFlags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION|COPY_FILE_COPY_SYMLINK;
			copyFlags.Add(COPY_FILE_FAIL_IF_EXISTS, !(flags & FSActionFlag::ReplaceIfExist));
			copyFlags.Add(COPY_FILE_NO_BUFFERING, flags & FSActionFlag::NoBuffering);

			const String sourcePath = source.GetFullPathTryNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathTryNS(FSPathNamespace::Win32File);

			BOOL canceled = FALSE;
			if (func)
			{
				return ::CopyFileExW(sourcePath.wc_str(), destinationPath.wc_str(),
									 [](LARGE_INTEGER TotalFileSize,
										LARGE_INTEGER TotalBytesTransferred,
										LARGE_INTEGER StreamSize,
										LARGE_INTEGER StreamBytesTransferred,
										DWORD dwStreamNumber,
										DWORD dwCallbackReason,
										HANDLE hSourceFile,
										HANDLE hDestinationFile,
										LPVOID lpData) -> DWORD
				{
					auto& callback = *static_cast<decltype(func)*>(lpData);

					if (callback.Invoke(DataSize::FromBytes(TotalBytesTransferred.QuadPart), DataSize::FromBytes(TotalFileSize.QuadPart)).ShouldTerminate())
					{
						return PROGRESS_CANCEL;
					}
					return PROGRESS_CONTINUE;
				}, &func, &canceled, *copyFlags);
			}
			else
			{
				return ::CopyFileExW(sourcePath.wc_str(), destinationPath.wc_str(), nullptr, nullptr, &canceled, *copyFlags);
			}
		});
	}
	bool NativeFileSystem::MoveItem(const FSPath& source, const FSPath& destination, CallbackFunction<DataSize, DataSize> func, FlagSet<FSActionFlag> flags)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath2(source, destination, [&](const FSPath& source, const FSPath& destination) -> bool
		{
			FlagSet<DWORD> moveFlags = MOVEFILE_COPY_ALLOWED;
			moveFlags.Add(MOVEFILE_REPLACE_EXISTING, flags & FSActionFlag::ReplaceIfExist);
			moveFlags.Add(MOVEFILE_WRITE_THROUGH, flags & FSActionFlag::NoBuffering);

			const String sourcePath = source.GetFullPathTryNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathTryNS(FSPathNamespace::Win32File);

			if (func)
			{
				return ::MoveFileWithProgressW(sourcePath.wc_str(), destinationPath.wc_str(), [](LARGE_INTEGER TotalFileSize,
																								 LARGE_INTEGER TotalBytesTransferred,
																								 LARGE_INTEGER StreamSize,
																								 LARGE_INTEGER StreamBytesTransferred,
																								 DWORD dwStreamNumber,
																								 DWORD dwCallbackReason,
																								 HANDLE hSourceFile,
																								 HANDLE hDestinationFile,
																								 LPVOID lpData) -> DWORD
				{
					auto& callback = *static_cast<decltype(func)*>(lpData);

					if (callback.Invoke(DataSize::FromBytes(TotalBytesTransferred.QuadPart), DataSize::FromBytes(TotalFileSize.QuadPart)).ShouldTerminate())
					{
						return PROGRESS_CANCEL;
					}
					return PROGRESS_CONTINUE;
				}, &func, *moveFlags);
			}
			else
			{
				return ::MoveFileExW(sourcePath.wc_str(), destinationPath.wc_str(), *moveFlags);
			}
		});
	}
	bool NativeFileSystem::RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSActionFlag> flags)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath2(source, destination, [&](const FSPath& source, const FSPath& destination) -> bool
		{
			const String sourcePath = source.GetFullPathTryNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathTryNS(FSPathNamespace::Win32File);

			DWORD moveFlags = flags & FSActionFlag::ReplaceIfExist ? MOVEFILE_REPLACE_EXISTING : 0;
			return ::MoveFileExW(sourcePath.wc_str(), destinationPath.wc_str(), moveFlags);
		});
	}
	bool NativeFileSystem::RemoveItem(const FSPath& path)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath1(path, [&](const FSPath& path) -> bool
		{
			const String sourcePath = path.GetFullPathTryNS(FSPathNamespace::Win32File);

			const uint32_t attributes = ::GetFileAttributesW(sourcePath.wc_str());
			if (attributes != INVALID_FILE_ATTRIBUTES)
			{
				if (::SetFileAttributesW(sourcePath.wc_str(), FILE_ATTRIBUTE_NORMAL))
				{
					if (attributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						return ::RemoveDirectoryW(sourcePath.wc_str());
					}
					else
					{
						return ::DeleteFileW(sourcePath.wc_str());
					}
				}
			}
			return false;
		});
	}
	bool NativeFileSystem::RemoveDirectory(const FSPath& path, FlagSet<FSActionFlag> flags)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath1(path, [&](const FSPath& path) -> bool
		{
			const String sourcePath = path.GetFullPathTryNS(FSPathNamespace::Win32File);

			const FlagSet<uint32_t> attributes = ::GetFileAttributesW(sourcePath.wc_str());
			if (!attributes.Equals(INVALID_FILE_ATTRIBUTES) && attributes.Contains(FILE_ATTRIBUTE_DIRECTORY))
			{
				if (flags.Contains(FSActionFlag::Recursive))
				{
					std::vector<String> directories;
					for (const FileItem& item: EnumItems(path, {}, FSActionFlag::Recursive))
					{
						String filePath = item.GetFullPath().GetFullPathTryNS(FSPathNamespace::Win32File);
						if (item.IsDirectory())
						{
							if (!DoRemoveDirectory(filePath))
							{
								if (*Win32Error::GetLastError() == ERROR_DIR_NOT_EMPTY)
								{
									// Defer deletion for later
									directories.emplace_back(std::move(filePath));
								}
								return false;
							}
						}
						else if (!DoRemoveFile(filePath))
						{
							return false;
						}
					}
					directories.emplace_back(path);

					size_t failCount = 0;
					size_t failCountThreshold = directories.size() * 2;
					bool shouldExit = false;
					while (!shouldExit && !directories.empty())
					{
						for (size_t i = directories.size() - 1; i != 0; i--)
						{
							if (DoRemoveDirectory(directories[i]))
							{
								directories.erase(directories.begin() + i);
							}
							else
							{
								failCount++;
								if (*Win32Error::GetLastError() != ERROR_DIR_NOT_EMPTY)
								{
									shouldExit = true;
									break;
								}
							}
						}

						// Not be the best solution but anyway
						if (failCount >= failCountThreshold)
						{
							return false;
						}
					}
					return !shouldExit && directories.empty();
				}
				else
				{
					return DoRemoveDirectory(sourcePath);
				}
			}
			return false;
		});
	}

	std::shared_ptr<IStream> NativeFileSystem::CreateStream(const FSPath& path,
															FlagSet<IOStreamAccess> access,
															IOStreamDisposition disposition,
															FlagSet<IOStreamShare> share,
															FlagSet<IOStreamFlag> streamFlags,
															FlagSet<FSActionFlag> flags)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath1(path, [&](const FSPath& path) -> std::shared_ptr<IStream>
		{
			NativeFileStream fileStream(path, access, disposition, share, streamFlags);
			if (!fileStream && flags.Contains(FSActionFlag::CreateDirectoryTree) && fileStream.GetLastNativeError().IsSameAs<Win32Error>(ERROR_PATH_NOT_FOUND))
			{
				if (streamFlags.Contains(IOStreamFlag::AllowDirectories))
				{
					CreateDirectory(path, flags.ExtractIfMatches(FSActionFlag::Recursive));
				}
				else
				{
					CreateDirectory(path.GetParent(), flags.ExtractIfMatches(FSActionFlag::Recursive));
				}

				fileStream.Open(path, access, disposition, share, streamFlags);
			}

			if (fileStream)
			{
				return std::make_shared<NativeFileStream>(std::move(fileStream))->QueryInterface<IStream>();
			}
			return nullptr;
		});
	}

	// IFileIDSystem
	FileItem NativeFileSystem::GetItem(const UniversallyUniqueID& id) const
	{
		if (IsNull())
		{
			return {};
		}

		NativeFileStream stream;
		if (OpenFileByID(m_LookupVolume, id, stream))
		{
			return FileSystem::Private::ConvertFileInfo(stream.GetHandle(), id);
		}
		return {};
	}
	Enumerator<FileItem> NativeFileSystem::EnumItems(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags) const
	{
		return {};
	}

	std::shared_ptr<IStream> NativeFileSystem::CreateStream(const UniversallyUniqueID& id,
															FlagSet<IOStreamAccess> access,
															IOStreamDisposition disposition,
															FlagSet<IOStreamShare> share,
															FlagSet<IOStreamFlag> streamFlags,
															FlagSet<FSActionFlag> flags)
	{
		if (!id || IsNull())
		{
			return nullptr;
		}

		NativeFileStream fileStream;
		if (OpenFileByID(m_LookupVolume, id, fileStream, access, disposition, share, streamFlags))
		{
			return std::make_shared<NativeFileStream>(std::move(fileStream))->QueryInterface<IStream>();
		}
		return nullptr;
	}

	// NativeFileSystem
	bool NativeFileSystem::IsInUse(const FSPath& path) const
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath1(path, [&](const FSPath& path) -> bool
		{
			return NativeFileStream().Open(path, IOStreamAccess::Read, IOStreamDisposition::OpenExisting, IOStreamShare::None);
		});
	}
	CallbackResult<size_t> NativeFileSystem::EnumStreams(const FSPath& path, CallbackFunction<String, DataSize> func) const
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath1(path, [&](const FSPath& path)
		{
			bool invoke = true;
			size_t counter = 0;
			String pathString = path.GetFullPathTryNS(FSPathNamespace::Win32File);

			WIN32_FIND_STREAM_DATA streamInfo = {};
			HANDLE handle = ::FindFirstStreamW(pathString.wc_str(), STREAM_INFO_LEVELS::FindStreamInfoStandard, &streamInfo, 0);
			if (handle && handle != INVALID_HANDLE_VALUE)
			{
				Utility::ScopeGuard atExit([&]()
				{
					::FindClose(handle);
				});

				do
				{
					// Fetch the file info and invoke the callback
					counter++;
					if (invoke && func.Invoke(streamInfo.cStreamName, DataSize::FromBytes(streamInfo.StreamSize.QuadPart)).ShouldTerminate())
					{
						invoke = false;
					}
				}
				while (::FindNextStreamW(handle, &streamInfo));
			}
			return func.Finalize(counter);
		});
	}

	CallbackResult<bool> NativeFileSystem::CopyDirectoryTree(const FSPath& source, const FSPath& destination, CallbackFunction<FSPath, FSPath, DataSize, DataSize> func, FlagSet<FSActionFlag> flags)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath2(source, destination, [&](const FSPath& source, const FSPath& destination)
		{
			return FileSystem::Private::CopyOrMoveDirectoryTree(*this, source, destination, std::move(func), flags, false);
		});
	}
	CallbackResult<bool> NativeFileSystem::MoveDirectoryTree(const FSPath& source, const FSPath& destination, CallbackFunction<FSPath, FSPath, DataSize, DataSize> func, FlagSet<FSActionFlag> flags)
	{
		FileSystem::Private::PathResolver pathResolver(*this);
		return pathResolver.DoWithResolvedPath2(source, destination, [&](const FSPath& source, const FSPath& destination)
		{
			return FileSystem::Private::CopyOrMoveDirectoryTree(*this, source, destination, std::move(func), flags, true);
		});
	}
}
