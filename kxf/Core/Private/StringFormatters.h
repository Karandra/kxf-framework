#pragma once
#include "../Common.h"
#include <format>
#include <string>
#include <string_view>

namespace std
{
	// kxf::String
	template<>
	struct formatter<kxf::String, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(const kxf::String& value, TFormatContext& formatContext) const
		{
			auto utf8 = value.ToUTF8();
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	template<>
	struct formatter<kxf::String, wchar_t>: std::formatter<std::wstring_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(const kxf::String& value, TFormatContext& formatContext) const
		{
			return std::formatter<std::wstring_view, wchar_t>::format(value.xc_view(), formatContext);
		}
	};

	// Converting 'const char*/wchar_t*'
	template<>
	struct formatter<const wchar_t*, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(const wchar_t* value, TFormatContext& formatContext) const
		{
			auto utf8 = kxf::String(value).ToUTF8();
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	// Converting 'const char[N]/wchar_t[N]'
	template<size_t N>
	struct formatter<wchar_t[N], char>: std::formatter<std::string_view, char>
	{
		template<class T, class TFormatContext>
		auto format(const T& value, TFormatContext& formatContext) const
		{
			auto utf8 = kxf::String(value, N != 0 ? N - 1 : 0).ToUTF8();
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	// Converting const 'std::[w]string_view'
	template<>
	struct formatter<std::string_view, wchar_t>: std::formatter<std::wstring_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(std::string_view value, TFormatContext& formatContext) const
		{
			auto converted = kxf::String::FromUTF8(value);
			return std::formatter<std::wstring_view, wchar_t>::format(converted.xc_view(), formatContext);
		}
	};

	template<>
	struct formatter<std::wstring_view, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(std::wstring_view value, TFormatContext& formatContext) const
		{
			auto utf8 = kxf::String::FromView(value).ToUTF8();
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	// Converting 'const std::[w]string'
	template<>
	struct formatter<std::string, wchar_t>: formatter<std::string_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(const std::string& value, TFormatContext& formatContext) const
		{
			return formatter<std::string_view, wchar_t>::format(value, formatContext);
		}
	};

	template<>
	struct formatter<std::wstring, char>: formatter<std::wstring_view, char>
	{
		template<class TFormatContext>
		auto format(const std::wstring& value, TFormatContext& formatContext) const
		{
			return formatter<std::wstring_view, char>::format(value, formatContext);
		}
	};

	// Enumerations
	template<class TEnum, class TChar> requires(std::is_enum_v<TEnum>)
	struct formatter<TEnum, TChar>: formatter<std::underlying_type_t<TEnum>, TChar>
	{
		template<class TFormatContext>
		auto format(TEnum value, TFormatContext& formatContext) const
		{
			using Tx = std::underlying_type_t<TEnum>;
			return formatter<Tx, TChar>::format(static_cast<Tx>(value), formatContext);
		}
	};
}
