#include "KxfPCH.h"
#include "NativeFileStream.h"
#include "kxf/System/Win32Error.h"
#include "kxf/FileSystem/Private/NativeFSUtility.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/Utility/String.h"

namespace
{
	using namespace kxf;

	int64_t SeekByHandle(HANDLE handle, DataSize offset, IOStreamSeek seekMode) noexcept
	{
		DWORD seekModeWin = std::numeric_limits<DWORD>::max();
		switch (seekMode)
		{
			case IOStreamSeek::FromStart:
			{
				seekModeWin = FILE_BEGIN;
				break;
			}
			case IOStreamSeek::FromCurrent:
			{
				seekModeWin = FILE_CURRENT;
				break;
			}
			case IOStreamSeek::FromEnd:
			{
				seekModeWin = FILE_END;
				break;
			}
			default:
			{
				return -1;
			}
		};

		LARGE_INTEGER moveTo = {};
		moveTo.QuadPart = offset.ToBytes();

		LARGE_INTEGER newOffset = {};
		if (::SetFilePointerEx(handle, moveTo, &newOffset, seekModeWin))
		{
			return newOffset.QuadPart;
		}
		return -1;
	}
	int64_t GetSizeByHandle(HANDLE handle) noexcept
	{
		LARGE_INTEGER size = {};
		if (::GetFileSizeEx(handle, &size))
		{
			return size.QuadPart;
		}
		return wxInvalidOffset;
	}
	int64_t GetOffsetByHandle(HANDLE handle) noexcept
	{
		LARGE_INTEGER offset = {};
		::SetFilePointerEx(handle, offset, &offset, FILE_CURRENT);
		return offset.QuadPart;
	}
	FSPath GetPathByHandle(HANDLE handle)
	{
		constexpr DWORD flags = VOLUME_NAME_DOS|FILE_NAME_NORMALIZED;
		const DWORD length = ::GetFinalPathNameByHandleW(handle, nullptr, 0, flags);
		if (length != 0)
		{
			String result;
			if (::GetFinalPathNameByHandleW(handle, Utility::StringBuffer(result, length), length, flags) != 0)
			{
				return FSPath(std::move(result));
			}
		}
		return {};
	}
}

namespace kxf
{
	bool NativeFileStream::DoClose()
	{
		if (DoIsOpened())
		{
			Flush();

			const HANDLE handle = m_Handle;
			m_Handle = nullptr;
			*this = {};

			if (::CloseHandle(handle))
			{
				m_LastError = ERROR_HANDLES_CLOSED;
			}
			else
			{
				m_LastError = Win32Error::GetLastError();
			}
			return true;
		}
		return false;
	}
	bool NativeFileStream::DoIsOpened() const
	{
		return m_Handle && m_Handle != INVALID_HANDLE_VALUE;
	}
	bool NativeFileStream::DoIsEndOfStream() const
	{
		return *m_LastError == ERROR_HANDLE_EOF || GetOffsetByHandle(m_Handle) == GetSizeByHandle(m_Handle);
	}

	StreamError NativeFileStream::GetLastError() const
	{
		if (m_LastError.IsSuccess())
		{
			return StreamError::Success();
		}
		else
		{
			switch (m_LastError.GetValue())
			{
				case ERROR_READ_FAULT:
				{
					return StreamErrorCode::ReadError;
				}
				case ERROR_WRITE_FAULT:
				{
					return StreamErrorCode::WriteError;
				}
				case ERROR_WRITE_PROTECT:
				{
					return StreamErrorCode::ReadOnly;
				}
				case ERROR_HANDLE_EOF:
				{
					return StreamErrorCode::EndOfStream;
				}
			};
			return StreamError::Fail();
		}
	}
	void NativeFileStream::SetLastError(StreamError lastError)
	{
		switch (lastError.GetCode())
		{
			case StreamErrorCode::Success:
			{
				m_LastError = Win32Error::Success();
				break;
			}
			case StreamErrorCode::ReadError:
			{
				m_LastError = ERROR_READ_FAULT;
				break;
			}
			case StreamErrorCode::WriteError:
			{
				m_LastError = ERROR_WRITE_FAULT;
				break;
			}
			case StreamErrorCode::ReadOnly:
			{
				m_LastError = ERROR_WRITE_PROTECT;
				break;
			}
			case StreamErrorCode::EndOfStream:
			{
				m_LastError = ERROR_HANDLE_EOF;
				break;
			}
			default:
			{
				m_LastError = Win32Error::Fail();
				break;
			}
		};
	}

	// IStream
	bool NativeFileStream::IsSeekable() const
	{
		return ::GetFileType(m_Handle) == FILE_TYPE_DISK;
	}
	DataSize NativeFileStream::GetSize() const
	{
		return GetSizeByHandle(m_Handle);
	}

	// IInputStream
	std::optional<uint8_t> NativeFileStream::Peek()
	{
		Utility::ScopeGuard atExit = [&, oldOffset = GetOffsetByHandle(m_Handle)]()
		{
			SeekByHandle(m_Handle, oldOffset, IOStreamSeek::FromStart);
		};

		DWORD lastRead = 0;
		uint8_t value = 0;
		if (::ReadFile(m_Handle, &value, sizeof(value), &lastRead, nullptr))
		{
			return value;
		}
		return {};
	}
	IInputStream& NativeFileStream::Read(void* buffer, size_t size)
	{
		if (buffer)
		{
			DWORD lastRead = 0;
			if (::ReadFile(m_Handle, buffer, size, &lastRead, nullptr))
			{
				if (lastRead == 0 && size != 0 && GetOffsetByHandle(m_Handle) == GetSizeByHandle(m_Handle))
				{
					m_LastError = ERROR_HANDLE_EOF;
				}
				else
				{
					m_LastError = Win32Error::Success();
				}
			}
			else
			{
				m_LastError = Win32Error::GetLastError();
			}

			m_LastRead = lastRead;
			m_StreamOffset = GetOffsetByHandle(m_Handle);
		}
		else
		{
			m_LastRead = {};
			m_LastError = ERROR_INVALID_PARAMETER;
		}
		return *this;
	}

	DataSize NativeFileStream::TellI() const
	{
		return GetOffsetByHandle(m_Handle);
	}
	DataSize NativeFileStream::SeekI(DataSize offset, IOStreamSeek seek)
	{
		m_StreamOffset = SeekByHandle(m_Handle, offset, seek);
		return m_StreamOffset;
	}

	// IOutputStream
	IOutputStream& NativeFileStream::Write(const void* buffer, size_t size)
	{
		if (buffer)
		{
			DWORD lastWrite = 0;
			if (::WriteFile(m_Handle, buffer, size, &lastWrite, nullptr))
			{
				m_LastError = Win32Error::Success();
			}
			else
			{
				m_LastError = Win32Error::GetLastError();
			}

			m_LastWrite = lastWrite;
			m_StreamOffset = GetOffsetByHandle(m_Handle);
		}
		else
		{
			m_LastWrite = {};
			m_LastError = ERROR_INVALID_PARAMETER;
		}
		return *this;
	}
	DataSize NativeFileStream::TellO() const
	{
		return GetOffsetByHandle(m_Handle);
	}
	DataSize NativeFileStream::SeekO(DataSize offset, IOStreamSeek seek)
	{
		m_StreamOffset = SeekByHandle(m_Handle, offset, seek);
		return m_StreamOffset;
	}

	bool NativeFileStream::Flush()
	{
		return ::FlushFileBuffers(m_Handle);
	}
	bool NativeFileStream::SetAllocationSize(DataSize allocationSize)
	{
		if (allocationSize)
		{
			Utility::ScopeGuard atExit = [&, oldOffset = GetOffsetByHandle(m_Handle)]()
			{
				SeekByHandle(m_Handle, oldOffset, IOStreamSeek::FromStart);
			};

			// Seek to an offset and set end of file there
			SeekByHandle(m_Handle, allocationSize, IOStreamSeek::FromStart);
		}
		return ::SetEndOfFile(m_Handle);
	}

	// IReadableOutputStream
	std::unique_ptr<IInputStream> NativeFileStream::CreateInputStream() const
	{
		NativeFileStream stream(GetFilePath(), IOStreamAccess::Read, IOStreamDisposition::OpenExisting, IOStreamShare::Everything, m_Flags);
		if (stream)
		{
			return std::make_unique<NativeFileStream>(std::move(stream));
		}
		return nullptr;
	}

	// INativeStream
	bool NativeFileStream::AttachHandle(void* handle)
	{
		DoClose();

		m_AccessMode = {};
		m_Disposition = IOStreamDisposition::OpenExisting;
		m_ShareMode = {};
		m_Flags = {};
		m_Handle = handle;
		
		if (DoIsOpened())
		{
			m_LastError = Win32Error::Success();
			return true;
		}
		else
		{
			m_LastError = Win32Error::Fail();
			return false;
		}
	}
	bool NativeFileStream::ReopenHandle(FlagSet<IOStreamAccess> access, FlagSet<IOStreamShare> share, FlagSet<IOStreamFlag> flags)
	{
		if (DoIsOpened())
		{
			m_AccessMode = access;
			m_Disposition = IOStreamDisposition::OpenExisting;
			m_ShareMode = share;
			m_Flags = flags;
			m_Handle = ::ReOpenFile(m_Handle,
									*FileSystem::Private::MapFileAccessMode(m_AccessMode),
									*FileSystem::Private::MapFileShareMode(m_ShareMode),
									*FileSystem::Private::MapFileFlags(flags));

			if (DoIsOpened())
			{
				m_LastError = Win32Error::Success();
				return true;
			}
			else
			{
				m_LastError = Win32Error::GetLastError();
			}
		}
		return false;
	}
	void* NativeFileStream::DetachHandle()
	{
		void* handle = m_Handle;
		m_Handle = nullptr;
		return handle;
	}

	FlagSet<FileAttribute> NativeFileStream::GetAttributes() const
	{
		if (DoIsOpened())
		{
			BY_HANDLE_FILE_INFORMATION info = {};
			if (::GetFileInformationByHandle(m_Handle, &info))
			{
				return FileSystem::Private::MapFileAttributes(info.dwFileAttributes);
			}
		}
		return {};
	}
	bool NativeFileStream::SetAttributes(FlagSet<FileAttribute> attributes)
	{
		if (DoIsOpened())
		{
			FILE_BASIC_INFO basicInfo = {};
			if (::GetFileInformationByHandleEx(m_Handle, FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo, &basicInfo, sizeof(basicInfo)))
			{
				basicInfo.FileAttributes = *FileSystem::Private::MapFileAttributes(attributes);
				return ::SetFileInformationByHandle(m_Handle, FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo, &basicInfo, sizeof(basicInfo));
			}
		}
		return false;
	}

	bool NativeFileStream::GetTimestamp(DateTime& creationTime, DateTime& modificationTime, DateTime& lastAccessTime) const
	{
		FILETIME creationTimeFile = {};
		FILETIME modificationTimeFile = {};
		FILETIME lastAccessTimeFile = {};
		if (m_Handle && ::GetFileTime(m_Handle, &creationTimeFile, &lastAccessTimeFile, &modificationTimeFile))
		{
			creationTime = FileSystem::Private::ConvertDateTime(creationTimeFile);
			modificationTime = FileSystem::Private::ConvertDateTime(modificationTimeFile);
			lastAccessTime = FileSystem::Private::ConvertDateTime(lastAccessTimeFile);

			return true;
		}
		return false;
	}
	bool NativeFileStream::ChangeTimestamp(DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime)
	{
		if (m_Handle)
		{
			auto ToFileTime = [](DateTime dateTime)
			{
				return FileSystem::Private::ConvertDateTimeToFileTime(dateTime).value_or(FILETIME{0, 0});
			};

			FILETIME creationTimeFile = ToFileTime(creationTime);
			FILETIME modificationTimeFile = ToFileTime(modificationTime);
			FILETIME lastAccessTimeFile = ToFileTime(lastAccessTime);
			return ::SetFileTime(m_Handle, &creationTimeFile, &lastAccessTimeFile, &modificationTimeFile);
		}
		return false;
	}

	// IStreamOnFileSystem
	FSPath NativeFileStream::GetFilePath() const
	{
		return GetPathByHandle(m_Handle);
	}
	UniversallyUniqueID NativeFileStream::GetFileUniqueID() const
	{
		if (DoIsOpened())
		{
			BY_HANDLE_FILE_INFORMATION fileInfo = {};
			if (::GetFileInformationByHandle(m_Handle, &fileInfo))
			{
				return FileSystem::Private::GetFileUniqueID(m_Handle, fileInfo);
			}
		}
		return {};
	}

	// NativeFileStream
	bool NativeFileStream::Open(const FSPath& path, FlagSet<IOStreamAccess> access, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<IOStreamFlag> flags)
	{
		DoClose();

		if (path)
		{
			m_LastError = Win32Error::Success();
			m_AccessMode = access;
			m_Disposition = disposition;
			m_ShareMode = share;
			m_Flags = flags;

			String pathString = path.GetFullPathTryNS(FSPathNamespace::Win32File);
			m_Handle = ::CreateFileW(pathString.wc_str(),
									 *FileSystem::Private::MapFileAccessMode(m_AccessMode),
									 *FileSystem::Private::MapFileShareMode(m_ShareMode),
									 nullptr,
									 FileSystem::Private::MapFileDisposition(m_Disposition),
									 *FileSystem::Private::MapFileFlags(flags),
									 nullptr);
			if (DoIsOpened())
			{
				m_LastError = Win32Error::Success();
				return true;
			}
			else
			{
				m_LastError = Win32Error::GetLastError();
			}
		}
		else
		{
			m_LastError = ERROR_INVALID_PARAMETER;
		}
		return false;
	}
}
