#include "kxf-pch.h"
#include "Common.h"
#include "kxf/Core/RegEx.h"

#include "OpenSSL/opensslv.h"
#include "kxf/Win32/LinkLibs-Crypto.h"

namespace kxf::Crypto
{
	String GetLibraryName()
	{
		return "OpenSSL";
	}
	Version GetLibraryVersion()
	{
		RegEx reg(R"(OpenSSL(?:\s+)([\d+\w+\.]+))");
		if (reg.Matches(OPENSSL_VERSION_TEXT))
		{
			return String(reg.GetMatch(OPENSSL_VERSION_TEXT, 1));
		}
		return OPENSSL_VERSION_TEXT;
	}
}
