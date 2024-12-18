#pragma once
#include "Common.h"
#include "String.h"
#include "Version.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class URI;
}

namespace kxf
{
	class ILibraryInfo: public RTTI::Interface<ILibraryInfo>
	{
		kxf_RTTI_DeclareIID(ILibraryInfo, {0x66ffd9cb, 0x32cc, 0x47dd, {0xab, 0x37, 0xb0, 0x83, 0xf2, 0x61, 0x3e, 0xc1}});

		public:
			virtual String GetName() const = 0;
			virtual Version GetVersion() const = 0;
			virtual URI GetHomePage() const = 0;
			virtual uint32_t GetAPILevel() const = 0;

			virtual String GetLicense() const = 0;
			virtual String GetLicenseName() const = 0;
			virtual String GetCopyright() const = 0;
	};
}
