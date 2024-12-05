#pragma once
#include "Common.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/INativeStream.h"
#include "kxf/IO/IStreamOnFileSystem.h"
#include "kxf/System/Win32Error.h"

namespace kxf
{
	class KX_API NativeFileStream: public RTTI::Implementation<NativeFileStream, IInputStream, IOutputStream, IReadableOutputStream, INativeStream, IStreamOnFileSystem>
	{
		private:
			void* m_Handle = nullptr;
			Win32Error m_LastError = Win32Error::Fail();
			DataSize m_LastRead;
			DataSize m_LastWrite;

			// These variables aren't directly used, they're mostly for debug purposes
			DataSize m_StreamOffset;
			FlagSet<IOStreamAccess> m_AccessMode;
			FlagSet<IOStreamShare> m_ShareMode;
			FlagSet<IOStreamFlag> m_Flags;
			IOStreamDisposition m_Disposition = IOStreamDisposition::OpenExisting;

		protected:
			bool DoClose();
			bool DoIsOpened() const;
			bool DoIsEndOfStream() const;

		public:
			NativeFileStream() noexcept = default;
			NativeFileStream(const FSPath& path,
							 FlagSet<IOStreamAccess> access,
							 IOStreamDisposition disposition,
							 FlagSet<IOStreamShare> share,
							 FlagSet<IOStreamFlag> flags = IOStreamFlag::None
			)
			{
				Open(path, access, disposition, share, flags);
			}
			NativeFileStream(const NativeFileStream&) = delete;
			NativeFileStream(NativeFileStream&& other) noexcept
			{
				*this = std::move(other);
			}
			~NativeFileStream()
			{
				DoClose();
			}
			
		public:
			// IStream
			void Close() override
			{
				DoClose();
			}

			StreamError GetLastError() const override;
			void SetLastError(StreamError lastError) override;

			bool IsSeekable() const override;
			DataSize GetSize() const override;

			// IInputStream
			bool CanRead() const override
			{
				return m_LastError.IsSuccess() && !DoIsEndOfStream();
			}
			
			DataSize LastRead() const override
			{
				return m_LastRead;
			}
			void SetLastRead(DataSize lastRead) override
			{
				m_LastRead = lastRead;
			}
			
			std::optional<uint8_t> Peek() override;
			IInputStream& Read(void* buffer, size_t size) override;
			using IInputStream::Read;

			DataSize TellI() const override;
			DataSize SeekI(DataSize offset, IOStreamSeek seek) override;

			// IOutputStream
			DataSize LastWrite() const override
			{
				return m_LastWrite;
			}
			void SetLastWrite(DataSize lastWrite) override
			{
				m_LastWrite = lastWrite;
			}
			
			IOutputStream& Write(const void* buffer, size_t size) override;
			using IOutputStream::Write;

			DataSize TellO() const override;
			DataSize SeekO(DataSize offset, IOStreamSeek seek) override;

			bool Flush() override;
			bool SetAllocationSize(DataSize allocationSize) override;

			// IReadableOutputStream
			std::unique_ptr<IInputStream> CreateInputStream() const override;

			// INativeStream
			void* GetHandle() const override
			{
				return m_Handle;
			}
			bool AttachHandle(void* handle) override;
			bool ReopenHandle(FlagSet<IOStreamAccess> access, FlagSet<IOStreamShare> share, FlagSet<IOStreamFlag> flags = IOStreamFlag::None) override;
			void* DetachHandle() override;

			ErrorCode GetLastNativeError() const override
			{
				return m_LastError;
			}

			FlagSet<FileAttribute> GetAttributes() const override;
			bool SetAttributes(FlagSet<FileAttribute> attributes) override;

			bool GetTimestamp(DateTime& creationTime, DateTime& modificationTime, DateTime& lastAccessTime) const override;
			bool ChangeTimestamp(DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) override;

			// IStreamOnFileSystem
			FSPath GetFilePath() const override;
			UniversallyUniqueID GetFileUniqueID() const override;

			// NativeFileStream
			bool Open(const FSPath& path,
					  FlagSet<IOStreamAccess> access,
					  IOStreamDisposition disposition,
					  FlagSet<IOStreamShare> share,
					  FlagSet<IOStreamFlag> flags = IOStreamFlag::None
			);

		public:
			explicit operator bool() const
			{
				return m_LastError.IsSuccess();
			}
			bool operator!() const
			{
				return m_LastError.IsFail();
			}

			NativeFileStream& operator=(const NativeFileStream&) = delete;
			NativeFileStream& operator=(NativeFileStream&& other) noexcept
			{
				DoClose();

				m_Handle = std::exchange(other.m_Handle, nullptr);
				m_LastError = std::move(other.m_LastError);
				m_LastRead = std::exchange(other.m_LastRead, DataSize());
				m_LastWrite = std::exchange(other.m_LastWrite, DataSize());

				m_StreamOffset = std::exchange(other.m_StreamOffset, DataSize());
				m_AccessMode = std::exchange(other.m_AccessMode, FlagSet<IOStreamAccess>());
				m_ShareMode = std::exchange(other.m_ShareMode, FlagSet<IOStreamShare>());
				m_Flags = std::exchange(other.m_Flags, FlagSet<IOStreamFlag>());
				m_Disposition = std::exchange(other.m_Disposition, IOStreamDisposition::OpenExisting);

				return *this;
			}
	};
}
