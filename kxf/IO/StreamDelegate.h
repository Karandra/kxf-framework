#pragma once
#include "Common.h"
#include "IStream.h"

namespace kxf::Private
{
	template<class TBaseStream_>
	requires(std::is_base_of_v<IStream, TBaseStream_>)
	class DelegateStreamBase: public TBaseStream_
	{
		public:
			using TBaseStream = TBaseStream_;

		protected:
			std::shared_ptr<TBaseStream> m_Stream;

		protected:
			DelegateStreamBase() = default;
			DelegateStreamBase(std::nullopt_t)
			{
			}
			DelegateStreamBase(std::shared_ptr<TBaseStream> stream)
				:m_Stream(std::move(stream))
			{
			}
			DelegateStreamBase(DelegateStreamBase&&) noexcept = default;
			DelegateStreamBase(const DelegateStreamBase&) = delete;

		public:
			bool HasTargetStream() const
			{
				return m_Stream != nullptr;
			}
			std::shared_ptr<TBaseStream> TakeTargetStream() noexcept
			{
				return std::move(m_Stream);
			}
			TBaseStream* GetTargetStream() const
			{
				return m_Stream.get();
			}
			std::shared_ptr<TBaseStream> get_ptr() const noexcept
			{
				return m_Stream;
			}

			const TBaseStream& operator*() const
			{
				return *m_Stream;
			}
			TBaseStream& operator*()
			{
				return *m_Stream;
			}
			TBaseStream* operator->() const
			{
				return m_Stream.get();
			}

			DelegateStreamBase& operator=(DelegateStreamBase&&) noexcept = default;
			DelegateStreamBase& operator=(const DelegateStreamBase&) = delete;

			explicit operator bool() const
			{
				return HasTargetStream();
			}
			bool operator!() const
			{
				return !HasTargetStream();
			}

		public:
			// IStream
			void Close() override
			{
				m_Stream->Close();
			}
			
			StreamError GetLastError() const override
			{
				return m_Stream->GetLastError();
			}
			void SetLastError(StreamError lastError) override
			{
				m_Stream->SetLastError(std::move(lastError));
			}

			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}
			DataSize GetSize() const override
			{
				return m_Stream->GetSize();
			}
	};
}

namespace kxf
{
	class KX_API InputStreamDelegate: public Private::DelegateStreamBase<IInputStream>
	{
		public:
			InputStreamDelegate() = default;
			InputStreamDelegate(std::nullptr_t)
			{
			}
			InputStreamDelegate(std::shared_ptr<IInputStream> stream)
				:DelegateStreamBase(std::move(stream))
			{
			}
			InputStreamDelegate(IInputStream& ref)
				:DelegateStreamBase(RTTI::assume_non_owned(ref))
			{
			}

		public:
			// IInputStream
			bool CanRead() const override
			{
				return m_Stream->CanRead();
			}
			DataSize LastRead() const override
			{
				return m_Stream->LastRead();
			}
			void SetLastRead(DataSize lastRead) override
			{
				m_Stream->SetLastRead(lastRead);
			}

			std::optional<uint8_t> Peek() override
			{
				return m_Stream->Peek();
			}
			IInputStream& Read(void* buffer, size_t size) override
			{
				m_Stream->Read(buffer, size);
				return *this;
			}
			IInputStream& Read(IOutputStream& other) override
			{
				m_Stream->Read(other);
				return *this;
			}
			bool ReadAll(void* buffer, size_t size) override
			{
				return m_Stream->ReadAll(buffer, size);
			}

			DataSize TellI() const override
			{
				return m_Stream->TellI();
			}
			DataSize SeekI(DataSize offset, IOStreamSeek seek) override
			{
				return m_Stream->SeekI(offset, seek);
			}
	};

	class KX_API OutputStreamDelegate: public Private::DelegateStreamBase<IOutputStream>
	{
		public:
			OutputStreamDelegate() = default;
			OutputStreamDelegate(std::nullptr_t)
			{
			}
			OutputStreamDelegate(std::shared_ptr<IOutputStream> stream)
				:DelegateStreamBase(std::move(stream))
			{
			}
			OutputStreamDelegate(IOutputStream& ref)
				:DelegateStreamBase(RTTI::assume_non_owned(ref))
			{
			}

		public:
			// IOutputStream
			DataSize LastWrite() const override
			{
				return m_Stream->LastWrite();
			}
			void SetLastWrite(DataSize lastWrite) override
			{
				m_Stream->SetLastWrite(lastWrite);
			}

			IOutputStream& Write(const void* buffer, size_t size) override
			{
				m_Stream->Write(buffer, size);
				return *this;
			}
			IOutputStream& Write(IInputStream& other) override
			{
				m_Stream->Write(other);
				return *this;
			}
			bool WriteAll(const void* buffer, size_t size) override
			{
				return m_Stream->WriteAll(buffer, size);
			}

			DataSize TellO() const override
			{
				return m_Stream->TellO();
			}
			DataSize SeekO(DataSize offset, IOStreamSeek seek) override
			{
				return m_Stream->SeekO(offset, seek);
			}

			bool Flush() override
			{
				return m_Stream->Flush();
			}
			bool SetAllocationSize(DataSize allocationSize) override
			{
				return m_Stream->SetAllocationSize(allocationSize);
			}
	};
}
