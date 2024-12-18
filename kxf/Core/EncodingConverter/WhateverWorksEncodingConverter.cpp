#include "kxf-pch.h"
#include "WhateverWorksEncodingConverter.h"
#include "NativeEncodingConverter.h"
#include <Windows.h>

namespace
{
	// ISO 8859-1/Latin 1
	constexpr int g_ISO8859_1 = 28591;
}

namespace kxf
{
	// IEncodingConverter
	size_t WhateverWorksEncodingConverter::ToMultiByteBuffer(std::span<const wchar_t> source, std::span<std::byte> destination)
	{
		// UTF-8
		size_t result = EncodingConverter_UTF8.ToMultiByte(source, destination);
		if (result != 0)
		{
			return result;
		}

		// Current locale
		result = EncodingConverter_Local.ToMultiByte(source, destination);
		if (result != 0)
		{
			return result;
		}

		result = NativeEncodingConverter(g_ISO8859_1).ToMultiByte(source, destination);
		return result;
	}
	size_t WhateverWorksEncodingConverter::ToWideCharBuffer(std::span<const std::byte> source, std::span<wchar_t> destination)
	{
		// UTF-8
		size_t result = EncodingConverter_UTF8.ToWideChar(source, destination);
		if (result != 0)
		{
			return result;
		}

		// Current locale
		result = EncodingConverter_Local.ToWideChar(source, destination);
		if (result != 0)
		{
			return result;
		}

		result = NativeEncodingConverter(g_ISO8859_1).ToWideChar(source, destination);
		return result;
	}

	// IEncodingConverter
	String WhateverWorksEncodingConverter::GetEncodingName() const
	{
		return {};
	}
}
