#include "KxfPCH.h"
#include "NativeFSUtility.h"
#include "kxf/IO/NativeFileStream.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/Utility/Memory.h"

namespace kxf::FileSystem::Private
{
	uint32_t GetFileAttributes(const FSPath& path)
	{
		String pathName = path.GetFullPathTryNS(FSPathNamespace::Win32File);
		return ::GetFileAttributesW(pathName.wc_str());
	}
	UniversallyUniqueID GetFileUniqueID(HANDLE fileHandle, const _BY_HANDLE_FILE_INFORMATION& fileInfo)
	{
		// "If you want to use GUIDs to identify your files, then nobody's stopping you": https://devblogs.microsoft.com/oldnewthing/20110228-00/?p=11363
		// Raymond Chen shows us use of NTFS ObjectIDs but I'm not sure it's the best idea to always use them
		// https://stackoverflow.com/questions/62440438/getfileinformationbyhandleex-fileidinfo-vs-deviceiocontrol-fsctl-create-or-get-o

		UniversallyUniqueID result = LocallyUniqueID(*Utility::CompositeInteger(fileInfo.nFileIndexLow, fileInfo.nFileIndexHigh));
		if (System::IsWindowsVersionOrGreater(NamedSystemRelease::Windows8))
		{
			FILE_ID_INFO fileIDInfo = {};
			if (::GetFileInformationByHandleEx(fileHandle, FILE_INFO_BY_HANDLE_CLASS::FileIdInfo, &fileIDInfo, sizeof(fileIDInfo)))
			{
				result = UniversallyUniqueID::CreateFromInt128(fileIDInfo.FileId.Identifier);
			}
		}
		return result;
	}

	bool IsValidFindItem(const _WIN32_FIND_DATAW& findInfo) noexcept
	{
		std::wstring_view name = findInfo.cFileName;
		return !(findInfo.dwFileAttributes == INVALID_FILE_ATTRIBUTES || name.empty() || name == L".." || name == L".");
	}
	void* CallFindFirstFile(const String& query, _WIN32_FIND_DATAW& findInfo, bool isCaseSensitive)
	{
		const DWORD searchFlags = FIND_FIRST_EX_LARGE_FETCH|(isCaseSensitive ? FIND_FIRST_EX_CASE_SENSITIVE : 0);
		return ::FindFirstFileExW(query.wc_str(), FindExInfoBasic, &findInfo, FINDEX_SEARCH_OPS::FindExSearchNameMatch, nullptr, searchFlags);
	}

	FileItem ConvertFileInfo(const _WIN32_FIND_DATAW& findInfo, const FSPath& location, UniversallyUniqueID id, FlagSet<FSActionFlag> flags)
	{
		FileItem fileItem;
		const bool isDirectory = findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

		// Construct path
		FSPath path;
		if (location.IsAbsolute())
		{
			path.ReserveLength(location.GetLength() + std::char_traits<XChar>::length(findInfo.cFileName) + 1);

			path = location;
			path /= findInfo.cFileName;
			path.EnsureNamespaceSet(location.GetNamespace());
		}
		else
		{
			// Invalid operation, we need full path to parent directory
			return {};
		}

		// Attributes and reparse point
		fileItem.SetAttributes(MapFileAttributes(findInfo.dwFileAttributes));
		if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		{
			fileItem.SetReparsePointTags(MapReparsePointTags(findInfo.dwReserved0));
		}

		// File size
		if (!isDirectory)
		{
			ULARGE_INTEGER size = {};
			size.HighPart = findInfo.nFileSizeHigh;
			size.LowPart = findInfo.nFileSizeLow;

			fileItem.SetSize(DataSize::FromBytes(size.QuadPart));
		}

		// Compressed file size
		if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
		{
			ULARGE_INTEGER compressedSize = {};

			String pathName = path.GetFullPathTryNS(FSPathNamespace::Win32File);
			compressedSize.LowPart = ::GetCompressedFileSizeW(pathName.wc_str(), &compressedSize.HighPart);
			fileItem.SetCompressedSize(DataSize::FromBytes(compressedSize.QuadPart));
		}

		// Date and time
		fileItem.SetCreationTime(ConvertDateTime(findInfo.ftCreationTime));
		fileItem.SetModificationTime(ConvertDateTime(findInfo.ftLastWriteTime));
		fileItem.SetLastAccessTime(ConvertDateTime(findInfo.ftLastAccessTime));

		// Unique ID
		if (id)
		{
			fileItem.SetUniqueID(std::move(id));
		}
		else if (flags.Contains(FSActionFlag::QueryUniqueID))
		{
			// TODO: Switch to a different directory enumeration method to avoid opening the file here to get its ID
			BY_HANDLE_FILE_INFORMATION fileInfo = {};
			NativeFileStream stream(path, IOStreamAccess::ReadAttributes, IOStreamDisposition::OpenExisting, IOStreamShare::Everything, IOStreamFlag::AllowDirectories);
			if (stream && ::GetFileInformationByHandle(stream.GetHandle(), &fileInfo))
			{
				fileItem.SetUniqueID(GetFileUniqueID(stream.GetHandle(), fileInfo));
			}
		}

		// Assign path
		fileItem.SetFullPath(std::move(path));

		return fileItem;
	}
	FileItem ConvertFileInfo(void* fileHandle, UniversallyUniqueID id, FlagSet<FSActionFlag> flags)
	{
		NativeFileStream stream;
		if (stream.AttachHandle(fileHandle))
		{
			Utility::ScopeGuard atExit= [&]()
			{
				stream.DetachHandle();
			};

			// File item and path
			FileItem fileItem(stream.GetFilePath());

			BY_HANDLE_FILE_INFORMATION fileInfo = {};
			if (::GetFileInformationByHandle(fileHandle, &fileInfo))
			{
				// Attributes and reparse point
				const bool isDirectory = fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
				fileItem.SetAttributes(MapFileAttributes(fileInfo.dwFileAttributes));
				if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
				{
					FILE_ATTRIBUTE_TAG_INFO attributeTagInfo = {};
					if (::GetFileInformationByHandleEx(fileHandle, FILE_INFO_BY_HANDLE_CLASS::FileAttributeTagInfo, &attributeTagInfo, sizeof(attributeTagInfo)))
					{
						fileItem.SetReparsePointTags(MapReparsePointTags(attributeTagInfo.ReparseTag));
					}
				}

				// File size
				if (!isDirectory)
				{
					fileItem.SetSize(DataSize::FromBytes(*Utility::CompositeInteger(fileInfo.nFileSizeLow, fileInfo.nFileSizeHigh)));
				}

				// Date and time
				fileItem.SetCreationTime(ConvertDateTime(fileInfo.ftCreationTime));
				fileItem.SetModificationTime(ConvertDateTime(fileInfo.ftLastWriteTime));
				fileItem.SetLastAccessTime(ConvertDateTime(fileInfo.ftLastAccessTime));

				// Compressed file size
				if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
				{
					FILE_COMPRESSION_INFO compressionInfo = {};
					if (::GetFileInformationByHandleEx(fileHandle, FILE_INFO_BY_HANDLE_CLASS::FileCompressionInfo, &compressionInfo, sizeof(compressionInfo)))
					{
						fileItem.SetCompressedSize(DataSize::FromBytes(compressionInfo.CompressedFileSize.QuadPart));
					}
				}

				// Unique ID
				if (id)
				{
					fileItem.SetUniqueID(std::move(id));
				}
				else if (flags.Contains(FSActionFlag::QueryUniqueID))
				{
					fileItem.SetUniqueID(GetFileUniqueID(fileHandle, fileInfo));
				}
			}
			return fileItem;
		}
		return {};
	}

	bool CopyOrMoveDirectoryTree(NativeFileSystem& fileSystem,
								 const FSPath& source,
								 const FSPath& destination,
								 std::function<CallbackCommand(FSPath, FSPath, DataSize, DataSize)> func,
								 FlagSet<FSActionFlag> flags,
								 bool move)
	{
		for (const FileItem& item: fileSystem.EnumItems(source, {}, flags|FSActionFlag::Recursive))
		{
			FSPath target = destination / item.GetFullPath().GetAfter(source);
			if (item.IsDirectory())
			{
				if (!func || std::invoke(func, source, target, 0, 0) != CallbackCommand::Terminate)
				{
					fileSystem.CreateDirectory(target);
					if (move)
					{
						fileSystem.RemoveItem(source);
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				bool result = false;
				if (func)
				{
					auto ForwardCallback = [&](DataSize copied, DataSize total)
					{
						return std::invoke(func, source, std::move(target), copied, total);
					};
					result = move ? fileSystem.MoveItem(source, target, std::move(ForwardCallback), flags) : fileSystem.CopyItem(source, target, std::move(ForwardCallback), flags);
				}
				else
				{
					result = move ? fileSystem.MoveItem(source, target, {}, flags) : fileSystem.CopyItem(source, target, {}, flags);
				}

				if (!result)
				{
					return false;
				}
			}
		}
		return true;
	}
}
