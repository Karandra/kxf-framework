#pragma once
#include "Common.h"
#include "kxf/Core/OptionalPtr.h"
#include "kxf/IO/IStream.h"
#include <wx/stream.h>

namespace kxf::wxWidgets
{
	class KXF_API InputStreamWrapper: public RTTI::Implementation<InputStreamWrapper, IInputStream>
	{
		protected:
			optional_ptr<wxInputStream> m_Stream;
			DataSize m_LastRead;
			std::optional<StreamError> m_LastError;

		protected:
			virtual void InvalidateInputCache()
			{
				m_LastRead = {};
				m_LastError = {};
			}

		public:
			InputStreamWrapper(wxInputStream& stream)
				:m_Stream(stream)
			{
			}
			InputStreamWrapper(std::unique_ptr<wxInputStream> stream)
				:m_Stream(std::move(stream))
			{
			}

		public:
			// IStream
			void Close() override
			{
				// Can't close input stream
			}

			StreamError GetLastError() const override;
			void SetLastError(StreamError lastError) override
			{
				m_LastError = lastError;
			}

			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}
			DataSize GetSize() const override
			{
				DataSize size = m_Stream->GetLength();
				if (!size)
				{
					size = m_Stream->GetSize();
				}

				return size;
			}

			// IInputStream
			bool CanRead() const override
			{
				return m_Stream->CanRead();
			}
			DataSize LastRead() const override
			{
				return m_LastRead ? m_LastRead : m_Stream->LastRead();
			}
			void SetLastRead(DataSize lastRead) override
			{
				m_LastRead = lastRead;
			}

			std::optional<uint8_t> Peek() override
			{
				InvalidateInputCache();
				return m_Stream->Peek();
			}
			IInputStream& Read(void* buffer, size_t size) override
			{
				InvalidateInputCache();
				m_Stream->Read(buffer, size);
				return *this;
			}
			using IInputStream::Read;

			DataSize TellI() const override
			{
				return m_Stream->TellI();
			}
			DataSize SeekI(DataSize offset, IOStreamSeek seek) override
			{
				InvalidateInputCache();

				if (auto seekWx = IO::ToWxSeekMode(seek))
				{
					return m_Stream->SeekI(offset.ToBytes(), *seekWx);
				}
				return {};
			}

			// InputStreamWrapper
			wxInputStream& AsWxStream() noexcept
			{
				return *m_Stream;
			}
			const wxInputStream& AsWxStream() const noexcept
			{
				return *m_Stream;
			}
	};

	class KXF_API OutputStreamWrapper: public RTTI::Implementation<OutputStreamWrapper, IOutputStream>
	{
		private:
			optional_ptr<wxOutputStream> m_Stream;
			DataSize m_LastWrite;
			std::optional<StreamError> m_LastError;

		protected:
			virtual void InvalidateOutputCache()
			{
				m_LastWrite = {};
				m_LastError = {};
			}

		public:
			OutputStreamWrapper(wxOutputStream& stream)
				:m_Stream(stream)
			{
			}
			OutputStreamWrapper(std::unique_ptr<wxOutputStream> stream)
				:m_Stream(std::move(stream))
			{
			}

		public:
			// IStream
			void Close() override
			{
				static_cast<void>(m_Stream->Close());
			}

			StreamError GetLastError() const override;
			void SetLastError(StreamError lastError) override
			{
				m_LastError = lastError;
			}

			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}
			DataSize GetSize() const override
			{
				DataSize size = m_Stream->GetLength();
				if (!size)
				{
					size = m_Stream->GetSize();
				}

				return size;
			}

			// IOutputStream
			DataSize LastWrite() const override
			{
				return m_LastWrite ? m_LastWrite : m_Stream->LastWrite();
			}
			void SetLastWrite(DataSize lastWrite) override
			{
				m_LastWrite = lastWrite;
			}

			IOutputStream& Write(const void* buffer, size_t size) override
			{
				InvalidateOutputCache();
				m_Stream->Write(buffer, size);
				return *this;
			}
			using IOutputStream::Write;

			DataSize TellO() const override
			{
				return m_Stream->TellO();
			}
			DataSize SeekO(DataSize offset, IOStreamSeek seek) override
			{
				InvalidateOutputCache();

				if (auto seekWx = IO::ToWxSeekMode(seek))
				{
					return m_Stream->SeekO(offset.ToBytes(), *seekWx);
				}
				return {};
			}

			bool Flush() override
			{
				return false;
			}
			bool SetAllocationSize(DataSize allocationSize) override
			{
				return false;
			}

			// OutputStream
			wxOutputStream& AsWxStream() noexcept
			{
				return *m_Stream;
			}
			const wxOutputStream& AsWxStream() const noexcept
			{
				return *m_Stream;
			}
	};
}

namespace kxf::wxWidgets
{
	class KXF_API InputStreamWrapperWx: public wxInputStream
	{
		protected:
			optional_ptr<IInputStream> m_Stream;

		protected:
			// wxStreamBase
			wxFileOffset OnSysTell() const override
			{
				return m_Stream->TellI().ToBytes<wxFileOffset>();
			}
			wxFileOffset OnSysSeek(wxFileOffset offset, wxSeekMode mode) override
			{
				if (auto seek = IO::FromWxSeekMode(mode))
				{
					return m_Stream->SeekI(offset, *seek).ToBytes<wxFileOffset>();
				}
				return wxInvalidOffset;
			}

			// wxInputStream
			size_t OnSysRead(void* buffer, size_t size) override
			{
				return m_Stream->Read(buffer, size).LastRead().ToBytes<size_t>();
			}

		public:
			InputStreamWrapperWx(IInputStream& stream)
				:m_Stream(stream)
			{
			}
			InputStreamWrapperWx(std::unique_ptr<IInputStream> stream)
				:m_Stream(std::move(stream))
			{
			}

		public:
			// wxStreamBase
			bool IsOk() const override
			{
				return static_cast<bool>(*m_Stream);
			}
			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}

			wxFileOffset GetLength() const override
			{
				return m_Stream->GetSize().ToBytes<wxFileOffset>();
			}
			size_t GetSize() const override
			{
				return m_Stream->GetSize().ToBytes<size_t>();
			}

			// wxInputStream
			bool Eof() const override
			{
				return !m_Stream->CanRead();
			}
			bool CanRead() const override
			{
				return m_Stream->CanRead();
			}

			size_t LastRead() const override
			{
				return m_Stream->LastRead().ToBytes<size_t>();
			}
			wxInputStream& Read(void* buffer, size_t size) override
			{
				m_Stream->Read(buffer, size);
				return *this;
			}
			char Peek() override
			{
				return m_Stream->Peek().value_or(0xFFu);
			}

			wxFileOffset TellI() const override
			{
				return m_Stream->TellI().ToBytes<wxFileOffset>();
			}
			wxFileOffset SeekI(wxFileOffset offset, wxSeekMode mode = wxSeekMode::wxFromStart) override
			{
				if (auto seek = IO::FromWxSeekMode(mode))
				{
					return m_Stream->SeekI(offset, *seek).ToBytes<wxFileOffset>();
				}
				return wxInvalidOffset;
			}

			// InputStreamWrapperWx
			IInputStream& AsKxfStream() noexcept
			{
				return *m_Stream;
			}
			const IInputStream& AsKxfStream() const noexcept
			{
				return *m_Stream;
			}
	};

	class KXF_API OutputStreamWrapperWx: public wxOutputStream
	{
		protected:
			optional_ptr<IOutputStream> m_Stream;

		protected:
			// wxStreamBase
			wxFileOffset OnSysTell() const override
			{
				return m_Stream->TellO().ToBytes<wxFileOffset>();
			}
			wxFileOffset OnSysSeek(wxFileOffset offset, wxSeekMode mode) override
			{
				if (auto seek = IO::FromWxSeekMode(mode))
				{
					return m_Stream->SeekO(offset, *seek).ToBytes<wxFileOffset>();
				}
				return wxInvalidOffset;
			}

			// wxOutputStream
			size_t OnSysWrite(const void* buffer, size_t size) override
			{
				return m_Stream->Write(buffer, size).LastWrite().ToBytes<size_t>();
			}

		public:
			OutputStreamWrapperWx(IOutputStream& stream)
				:m_Stream(stream)
			{
			}
			OutputStreamWrapperWx(std::unique_ptr<IOutputStream> stream)
				:m_Stream(std::move(stream))
			{
			}

		public:
			// wxStreamBase
			bool IsOk() const override
			{
				return static_cast<bool>(*m_Stream);
			}
			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}

			wxFileOffset GetLength() const override
			{
				return m_Stream->GetSize().ToBytes<wxFileOffset>();
			}
			size_t GetSize() const override
			{
				return m_Stream->GetSize().ToBytes<size_t>();
			}

			// wxOutputStream
			bool Close() override
			{
				const bool wasOk = IsOk();
				m_Stream->Close();
				return wasOk;
			}

			size_t LastWrite() const override
			{
				return m_Stream->LastWrite().ToBytes<size_t>();
			}
			wxOutputStream& Write(const void* buffer, size_t size) override
			{
				m_Stream->Write(buffer, size);
				return *this;
			}

			wxFileOffset TellO() const override
			{
				return m_Stream->TellO().ToBytes<wxFileOffset>();
			}
			wxFileOffset SeekO(wxFileOffset offset, wxSeekMode mode = wxSeekMode::wxFromStart) override
			{
				if (auto seek = IO::FromWxSeekMode(mode))
				{
					return m_Stream->SeekO(offset, *seek).ToBytes<wxFileOffset>();
				}
				return wxInvalidOffset;
			}

			// OutputStreamWrapperWx
			IOutputStream& AsKxfStream() noexcept
			{
				return *m_Stream;
			}
			const IOutputStream& AsKxfStream() const noexcept
			{
				return *m_Stream;
			}
	};
}
