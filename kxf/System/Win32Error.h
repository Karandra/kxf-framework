#pragma once
#include "Common.h"
#include "kxf/Core/IErrorCode.h"
#include "kxf/Serialization/BinarySerializer.h"

namespace kxf
{
	class HResult;
	class NtStatus;
}

namespace kxf
{
	class KXF_API Win32Error final: public RTTI::DynamicImplementation<Win32Error, IErrorCode>
	{
		kxf_RTTI_DeclareIID(Win32Error, {0x747f17c6, 0xea9d, 0x484d, {0xb9, 0xbe, 0xec, 0xc4, 0xa3, 0x72, 0x9f, 0x40}});

		public:
			static Win32Error Success() noexcept;
			static Win32Error Fail() noexcept;

			static Win32Error GetLastError() noexcept;
			static void SetLastError(Win32Error error) noexcept;

		private:
			uint32_t m_Value = std::numeric_limits<uint32_t>::max();

		public:
			Win32Error() noexcept;
			Win32Error(uint32_t value) noexcept
				:m_Value(value)
			{
			}
			
		public:
			// IErrorCode
			bool IsSuccess() const noexcept override
			{
				return *this == Success();
			}
			bool IsFail() const noexcept override
			{
				return !IsSuccess();
			}

			uint32_t GetValue() const noexcept override
			{
				return m_Value;
			}
			void SetValue(uint32_t value) noexcept override
			{
				m_Value = value;
			}

			String ToString() const override;
			String GetMessage(const Locale& locale = {}) const override;

			// Win32Error
			std::optional<HResult> ToHResult() const noexcept;
			std::optional<NtStatus> ToNtStatus() const noexcept;

		public:
			bool operator==(const Win32Error& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			bool operator!=(const Win32Error& other) const noexcept
			{
				return m_Value != other.m_Value;
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<Win32Error> final
	{
		uint64_t Serialize(IOutputStream& stream, const Win32Error& value) const
		{
			return Serialization::WriteObject(stream, value.GetValue());
		}
		uint64_t Deserialize(IInputStream& stream, Win32Error& value) const
		{
			uint32_t buffer = 0;
			auto read = Serialization::ReadObject(stream, buffer);
			value.SetValue(buffer);

			return read;
		}
	};
}
