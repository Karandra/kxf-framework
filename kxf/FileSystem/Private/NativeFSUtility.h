#pragma once
#include "../Common.h"
#include "../FileItem.h"
#include "../FSPath.h"
#include "../NativeFileSystem.h"
#include "kxf/Utility/Common.h"

#include <Windows.h>
#include "kxf/Win32/UndefMacros.h"
struct _WIN32_FIND_DATAW;
struct _BY_HANDLE_FILE_INFORMATION;

namespace kxf::FileSystem::Private
{
	uint32_t GetFileAttributes(const FSPath& path);
	UniversallyUniqueID GetFileUniqueID(HANDLE fileHandle, const _BY_HANDLE_FILE_INFORMATION& fileInfo);
	FSPath GetFileFullPath(HANDLE fileHandle);

	constexpr inline FlagSet<FileAttribute> MapFileAttributes(uint32_t nativeAttributes) noexcept
	{
		if (nativeAttributes == INVALID_FILE_ATTRIBUTES)
		{
			return FileAttribute::Invalid;
		}
		else if (nativeAttributes == FILE_ATTRIBUTE_NORMAL)
		{
			return FileAttribute::Normal;
		}
		else
		{
			FlagSet<FileAttribute> attributes;
			attributes.Add(FileAttribute::Hidden, nativeAttributes & FILE_ATTRIBUTE_HIDDEN);
			attributes.Add(FileAttribute::Archive, nativeAttributes & FILE_ATTRIBUTE_ARCHIVE);
			attributes.Add(FileAttribute::Directory, nativeAttributes & FILE_ATTRIBUTE_DIRECTORY);
			attributes.Add(FileAttribute::ReadOnly, nativeAttributes & FILE_ATTRIBUTE_READONLY);
			attributes.Add(FileAttribute::System, nativeAttributes & FILE_ATTRIBUTE_SYSTEM);
			attributes.Add(FileAttribute::Temporary, nativeAttributes & FILE_ATTRIBUTE_TEMPORARY);
			attributes.Add(FileAttribute::Compressed, nativeAttributes & FILE_ATTRIBUTE_COMPRESSED);
			attributes.Add(FileAttribute::Encrypted, nativeAttributes & FILE_ATTRIBUTE_ENCRYPTED);
			attributes.Add(FileAttribute::ReparsePoint, nativeAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
			attributes.Add(FileAttribute::SparseFile, nativeAttributes & FILE_ATTRIBUTE_SPARSE_FILE);
			attributes.Add(FileAttribute::Offline, nativeAttributes & FILE_ATTRIBUTE_OFFLINE);
			attributes.Add(FileAttribute::ContentIndexed, !(nativeAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED));
			attributes.Add(FileAttribute::RecallOnOpen, nativeAttributes & FILE_ATTRIBUTE_RECALL_ON_OPEN);
			attributes.Add(FileAttribute::RecallOnDataAccess, nativeAttributes & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS);

			return attributes;
		}
	}
	constexpr inline FlagSet<int32_t> MapFileAttributes(FlagSet<FileAttribute> attributes) noexcept
	{
		if (attributes == FileAttribute::Invalid)
		{
			return INVALID_FILE_ATTRIBUTES;
		}
		else if (attributes == FileAttribute::Normal)
		{
			return FILE_ATTRIBUTE_NORMAL;
		}
		else
		{
			FlagSet<int32_t> nativeAttributes;
			nativeAttributes.Add(FILE_ATTRIBUTE_HIDDEN, attributes & FileAttribute::Hidden);
			nativeAttributes.Add(FILE_ATTRIBUTE_ARCHIVE, attributes & FileAttribute::Archive);
			nativeAttributes.Add(FILE_ATTRIBUTE_DIRECTORY, attributes & FileAttribute::Directory);
			nativeAttributes.Add(FILE_ATTRIBUTE_READONLY, attributes & FileAttribute::ReadOnly);
			nativeAttributes.Add(FILE_ATTRIBUTE_SYSTEM, attributes & FileAttribute::System);
			nativeAttributes.Add(FILE_ATTRIBUTE_TEMPORARY, attributes & FileAttribute::Temporary);
			nativeAttributes.Add(FILE_ATTRIBUTE_COMPRESSED, attributes & FileAttribute::Compressed);
			nativeAttributes.Add(FILE_ATTRIBUTE_ENCRYPTED, attributes & FileAttribute::Encrypted);
			nativeAttributes.Add(FILE_ATTRIBUTE_REPARSE_POINT, attributes & FileAttribute::ReparsePoint);
			nativeAttributes.Add(FILE_ATTRIBUTE_SPARSE_FILE, attributes & FileAttribute::SparseFile);
			nativeAttributes.Add(FILE_ATTRIBUTE_OFFLINE, attributes & FileAttribute::Offline);
			nativeAttributes.Add(FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, !(attributes & FileAttribute::ContentIndexed));
			nativeAttributes.Add(FILE_ATTRIBUTE_RECALL_ON_OPEN, attributes & FileAttribute::RecallOnOpen);
			nativeAttributes.Add(FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS, attributes & FileAttribute::RecallOnDataAccess);

			return nativeAttributes;
		}
	}
	constexpr inline FlagSet<ReparsePointTag> MapReparsePointTags(uint32_t nativeTags) noexcept
	{
		// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-fscc/c8e77b37-3909-4fe6-a4ea-2b9d423b1ee4

		FlagSet<ReparsePointTag> tags;
		tags.Add(ReparsePointTag::MountPoint, nativeTags & IO_REPARSE_TAG_MOUNT_POINT);
		tags.Add(ReparsePointTag::SymLink, nativeTags & IO_REPARSE_TAG_SYMLINK);

		return tags;
	}

	constexpr inline FlagSet<uint32_t> MapFileAccessMode(FlagSet<IOStreamAccess> mode) noexcept
	{
		if (mode == IOStreamAccess::None)
		{
			return 0;
		}
		else
		{
			FlagSet<uint32_t> nativeMode;
			nativeMode.Add(GENERIC_READ, mode & IOStreamAccess::Read);
			nativeMode.Add(GENERIC_WRITE, mode & IOStreamAccess::Write);
			nativeMode.Add(FILE_READ_ATTRIBUTES, mode & IOStreamAccess::ReadAttributes);
			nativeMode.Add(FILE_WRITE_ATTRIBUTES, mode & IOStreamAccess::WriteAttributes);

			return nativeMode;
		}
		return std::numeric_limits<DWORD>::max();
	}
	constexpr inline FlagSet<uint32_t> MapFileShareMode(FlagSet<IOStreamShare> mode) noexcept
	{
		if (mode == IOStreamShare::None)
		{
			return 0;
		}
		else
		{
			FlagSet<uint32_t> nativeMode;
			nativeMode.Add(FILE_SHARE_READ, mode & IOStreamShare::Read);
			nativeMode.Add(FILE_SHARE_WRITE, mode & IOStreamShare::Write);
			nativeMode.Add(FILE_SHARE_DELETE, mode & IOStreamShare::Delete);

			return nativeMode;
		}
		return std::numeric_limits<DWORD>::max();
	}
	constexpr inline uint32_t MapFileDisposition(IOStreamDisposition mode) noexcept
	{
		switch (mode)
		{
			case IOStreamDisposition::OpenExisting:
			{
				return OPEN_EXISTING;
			}
			case IOStreamDisposition::OpenAlways:
			{
				return OPEN_ALWAYS;
			}
			case IOStreamDisposition::CreateNew:
			{
				return CREATE_NEW;
			}
			case IOStreamDisposition::CreateAlways:
			{
				return CREATE_ALWAYS;
			}
		};
		return 0;
	}
	constexpr inline FlagSet<uint32_t> MapFileFlags(FlagSet<IOStreamFlag> flags) noexcept
	{
		FlagSet<uint32_t> nativeMode;
		nativeMode.Add(FILE_ATTRIBUTE_NORMAL, flags & IOStreamFlag::Normal);
		nativeMode.Add(FILE_FLAG_BACKUP_SEMANTICS, flags & IOStreamFlag::AllowDirectories);

		return nativeMode;
	}

	inline DateTime ConvertDateTime(const FILETIME& fileTime) noexcept
	{
		return DateTime().SetFileTime(fileTime, TimeZone::UTC);
	}
	inline DateTime ConvertDateTime(const SYSTEMTIME& systemTime) noexcept
	{
		return DateTime().SetSystemTime(systemTime, TimeZone::UTC);
	}
	inline DateTime ConvertDateTime(const LARGE_INTEGER& fileTimeLI) noexcept
	{
		FILETIME fileTime = {};
		fileTime.dwLowDateTime = fileTimeLI.LowPart;
		fileTime.dwHighDateTime = fileTimeLI.HighPart;
		return ConvertDateTime(fileTime);
	}

	inline std::optional<SYSTEMTIME> ConvertDateTimeToSystemTime(DateTime dateTime) noexcept
	{
		if (dateTime)
		{
			return dateTime.GetSystemTime(TimeZone::UTC);
		}
		return {};
	}
	inline std::optional<FILETIME> ConvertDateTimeToFileTime(DateTime dateTime) noexcept
	{
		if (dateTime)
		{
			return dateTime.GetFileTime(TimeZone::UTC);
		}
		return {};
	}
	
	bool IsValidFindItem(const _WIN32_FIND_DATAW& findInfo) noexcept;
	void* CallFindFirstFile(const String& query, _WIN32_FIND_DATAW& findInfo, bool isCaseSensitive = false);

	FileItem ConvertFileInfo(const _WIN32_FIND_DATAW& findInfo, const FSPath& location, UniversallyUniqueID id = {}, FlagSet<FSActionFlag> flags = {});
	FileItem ConvertFileInfo(void* fileHandle, UniversallyUniqueID id = {}, FlagSet<FSActionFlag> flags = {});

	CallbackResult<bool> CopyOrMoveDirectoryTree(NativeFileSystem& fileSystem,
												 const FSPath& source,
												 const FSPath& destination,
												 CallbackFunction<FSPath, FSPath, DataSize, DataSize> func,
												 FlagSet<FSActionFlag> flags,
												 bool move);
}
