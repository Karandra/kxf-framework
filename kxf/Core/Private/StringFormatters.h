#pragma once
#include "../Common.h"
#include "../IEncodingConverter.h"
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
			return std::formatter<std::wstring_view, wchar_t>::format(value.view(), formatContext);
		}
	};

	// Converting 'const char*/wchar_t*'
	template<>
	struct formatter<const wchar_t*, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(const wchar_t* value, TFormatContext& formatContext) const
		{
			auto utf8 = kxf::EncodingConverter_UTF8.ToMultiByte(value);
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	// Converting 'const char[N]/wchar_t[N]'
	template<size_t N>
	struct formatter<wchar_t[N], char>: std::formatter<std::string_view, char>
	{
		template<class T, class TFormatContext>
		auto format(const T (&value)[N], TFormatContext& formatContext) const
		{
			auto utf8 = kxf::EncodingConverter_UTF8.ToMultiByte(kxf::StringViewOf(value));
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	// Converting const 'std::[w]string_view'
	template<>
	struct formatter<std::wstring_view, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(std::wstring_view value, TFormatContext& formatContext) const
		{
			auto utf8 = kxf::EncodingConverter_UTF8.ToMultiByte(value);
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	// Converting 'const std::[w]string'
	template<>
	struct formatter<std::wstring, char>: formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(const std::wstring& value, TFormatContext& formatContext) const
		{
			auto utf8 = kxf::EncodingConverter_UTF8.ToMultiByte(value);
			return formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	// Enumerations
	template<class TEnum, class TChar>
	requires(std::is_enum_v<TEnum>)
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
