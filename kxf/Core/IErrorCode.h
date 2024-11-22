#pragma once
#include "Common.h"
#include "Private/ErrorCode.h"
#include "kxf/Localization/Locale.h"
#include "kxf/Serialization/BinarySerializer.h"

namespace kxf
{
	class KX_API IErrorCode: public RTTI::Interface<IErrorCode>
	{
		KxRTTI_DeclareIID(IErrorCode, {0x79dd38bf, 0xec09, 0x4d32, {0x99, 0x4b, 0xa7, 0x82, 0x51, 0x90, 0x34, 0x12}});

		public:
			IErrorCode() noexcept = default;
			virtual ~IErrorCode() = default;

		public:
			virtual bool IsSuccess() const noexcept = 0;
			virtual bool IsFail() const noexcept = 0;

			virtual uint32_t GetValue() const noexcept = 0;
			virtual void SetValue(uint32_t value) noexcept = 0;
			
			virtual String ToString() const = 0;
			virtual String GetMessage(const Locale& locale = {}) const = 0;

		public:
			explicit operator bool() const noexcept
			{
				return IsSuccess();
			}
			bool operator!() const noexcept
			{
				return IsFail();
			}

			uint32_t operator*() const noexcept
			{
				return GetValue();
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<IErrorCode> final
	{
		uint64_t Serialize(IOutputStream& stream, const IErrorCode& value) const
		{
			return Serialization::WriteObject(stream, value.GetValue());
		}
		uint64_t Deserialize(IInputStream& stream, IErrorCode& value) const
		{
			uint32_t buffer = 0;
			auto read = Serialization::ReadObject(stream, buffer);
			value.SetValue(buffer);

			return read;
		}
	};
}

namespace std
{
	template<std::derived_from<kxf::IErrorCode> T>
	struct formatter<T, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(const T& error, TFormatContext& formatContext) const
		{
			auto formatted = kxf::Private::FormatErrorCode(error);
			return std::formatter<std::string_view, char>::format(formatted.utf8_str(), formatContext);
		}
	};

	template<std::derived_from<kxf::IErrorCode> T>
	struct formatter<T, wchar_t>: std::formatter<std::wstring_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(const T& error, TFormatContext& formatContext) const
		{
			auto formatted = kxf::Private::FormatErrorCode(error);
			return std::formatter<std::wstring_view, wchar_t>::format(formatted.view(), formatContext);
		}
	};
}
