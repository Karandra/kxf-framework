#pragma once
#include "Common.h"
#include "kxf/Core/IErrorCode.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/Serialization/BinarySerializer.h"
struct IErrorInfo;

namespace kxf
{
	class NtStatus;
	class Win32Error;
}

namespace kxf
{
	class KXF_API HResult final: public RTTI::DynamicImplementation<HResult, IErrorCode>
	{
		kxf_RTTI_DeclareIID(HResult, {0xfcb86dbc, 0xa3f9, 0x4385, {0x83, 0x72, 0x9, 0x15, 0x36, 0x5c, 0xb, 0x29}});

		public:
			static HResult Success() noexcept;
			static HResult False() noexcept;
			static HResult Fail() noexcept;
			static HResult Abort() noexcept;
			static HResult Pending() noexcept;
			static HResult Unexpected() noexcept;
			static HResult OutOfMemory() noexcept;
			static HResult OutOfRange() noexcept;
			static HResult AccessDenied() noexcept;
			static HResult NoInterface() noexcept;
			static HResult NotImplemented() noexcept;
			static HResult StateChnaged() noexcept;
			static HResult InvalidArgument() noexcept;
			static HResult InvalidPointer() noexcept;
			static HResult InvalidHandle() noexcept;
			static HResult InvalidStateChnage() noexcept;
			static HResult InvalidMethodCall() noexcept;

		private:
			IErrorInfo* m_ErrorInfo = nullptr;
			int32_t m_Value = std::numeric_limits<int32_t>::min();

		public:
			HResult() noexcept;
			HResult(int32_t value, IErrorInfo* errorInfo = nullptr) noexcept;
			~HResult();

		public:
			// IErrorCode
			bool IsSuccess() const noexcept override;
			bool IsFail() const noexcept override
			{
				return !IsSuccess();
			}

			uint32_t GetValue() const noexcept override
			{
				return static_cast<uint32_t>(m_Value);
			}
			void SetValue(uint32_t value) noexcept override
			{
				m_Value = static_cast<int32_t>(value);
			}

			String ToString() const override;
			String GetMessage(const Locale& locale = {}) const override;

			// HResult
			bool IsOK() const noexcept
			{
				return *this == Success();
			}
			bool IsFalse() const noexcept
			{
				return *this == False();
			}

			String GetSource() const;
			String GetHelpFile() const;
			uint32_t GetHelpContext() const noexcept;
			String GetDescription() const;
			uint32_t GetFacility() const noexcept;
			UniversallyUniqueID GetUniqueID() const noexcept;

			std::optional<Win32Error> ToWin32() const noexcept;
			std::optional<NtStatus> ToNtStatus() const noexcept;

		public:
			bool operator==(const HResult& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			bool operator!=(const HResult& other) const noexcept
			{
				return m_Value != other.m_Value;
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<HResult> final
	{
		uint64_t Serialize(IOutputStream& stream, const HResult& value) const
		{
			return Serialization::WriteObject(stream, value.GetValue());
		}
		uint64_t Deserialize(IInputStream& stream, HResult& value) const
		{
			int32_t buffer = 0;
			auto read = Serialization::ReadObject(stream, buffer);
			value.SetValue(buffer);

			return read;
		}
	};
}
