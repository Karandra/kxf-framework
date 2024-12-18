#include "kxf-pch.h"
#include "IEncodingConverter.h"
#include "EncodingConverter/NativeEncodingConverter.h"
#include "EncodingConverter/WhateverWorksEncodingConverter.h"
#include <Windows.h>

namespace
{
	kxf::WhateverWorksEncodingConverter g_EncodingConverter_WhateverWorks;

	kxf::NativeEncodingConverter g_EncodingConverter_Local(CP_ACP);
	kxf::NativeEncodingConverter g_EncodingConverter_ASCII(20127);
	kxf::NativeEncodingConverter g_EncodingConverter_UTF8(CP_UTF8);

	kxf::NativeEncodingConverter g_EncodingConverter_UTF16LE(1200);
	kxf::NativeEncodingConverter g_EncodingConverter_UTF16BE(1201);

	kxf::NativeEncodingConverter g_EncodingConverter_UTF32LE(12000);
	kxf::NativeEncodingConverter g_EncodingConverter_UTF32BE(12001);
}

namespace kxf
{
	IEncodingConverter& EncodingConverter_WhateverWorks = g_EncodingConverter_WhateverWorks;

	IEncodingConverter& EncodingConverter_Local = g_EncodingConverter_Local;
	IEncodingConverter& EncodingConverter_ASCII = g_EncodingConverter_ASCII;
	IEncodingConverter& EncodingConverter_UTF8 = g_EncodingConverter_UTF8;

	IEncodingConverter& EncodingConverter_UTF16LE = g_EncodingConverter_UTF16LE;
	IEncodingConverter& EncodingConverter_UTF16BE = g_EncodingConverter_UTF16BE;

	IEncodingConverter& EncodingConverter_UTF32LE = g_EncodingConverter_UTF32LE;
	IEncodingConverter& EncodingConverter_UTF32BE = g_EncodingConverter_UTF32BE;

	std::unique_ptr<IEncodingConverter> GetNativeEncodingConverter(int codePage, FlagSet<uint32_t> flags)
	{
		return std::make_unique<NativeEncodingConverter>(codePage, flags);
	}
}
