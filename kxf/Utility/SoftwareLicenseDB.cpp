#include "KxfPCH.h"
#include "SoftwareLicenseDB.h"
#include "System.h"
#include "kxf/System/DynamicLibrary.h"

#if !defined(KXF_DYNAMIC_LIBRARY)
	#pragma message("SoftwareLicenseDB requires building as a DLL otherwise some license texts will not be available")
#endif

namespace
{
	kxf::SoftwareLicenseDB g_SoftwareLicenseDB;

	constexpr char g_MIT[] = R"~~~(%1

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.)~~~";

	constexpr char g_Zlib[] = R"~~~(%1

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.)~~~";\
}
namespace
{
	kxf::String LoadLicense(const kxf::String& name)
	{
		auto buffer = kxf::Utility::LoadResource(kxf::DynamicLibrary::GetCurrentModule(), wxS("GNU_GPLv3"), wxS("SOFTWARELICENSEDB"));
		if (buffer.length() != 0)
		{
			return {buffer.data(), buffer.length()};
		}
		return {};
	}
}

namespace kxf
{
	const SoftwareLicenseDB& SoftwareLicenseDB::Get()
	{
		return g_SoftwareLicenseDB;
	}

	String SoftwareLicenseDB::GetName(SoftwareLicenseType licenseType) const
	{
		switch (licenseType)
		{
			case SoftwareLicenseType::MIT:
			{
				return wxS("MIT");
			}
			case SoftwareLicenseType::ZLib:
			{
				return wxS("zlib");
			}
			case SoftwareLicenseType::GNU_GPLv3:
			{
				return wxS("GNU GPLv3");
			}
			case SoftwareLicenseType::GNU_LGPLv3:
			{
				return wxS("GNU LGPLv3");
			}
		};
		return {};
	}
	String SoftwareLicenseDB::GetText(SoftwareLicenseType licenseType, const String& copyright) const
	{
		switch (licenseType)
		{
			case SoftwareLicenseType::MIT:
			{
				return String::Format(g_MIT, copyright).Trim();
			}
			case SoftwareLicenseType::ZLib:
			{
				return String::Format(g_Zlib, copyright).Trim();
			}
			case SoftwareLicenseType::GNU_GPLv3:
			{
				return LoadLicense(wxS("GNU_GPLv3"));
			}
			case SoftwareLicenseType::GNU_LGPLv3:
			{
				return LoadLicense(wxS("GNU_LGPLv3"));
			}
		};
		return {};
	}
}
