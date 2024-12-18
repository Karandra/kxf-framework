#include "kxf-pch.h"
#include "MasterStylesheetStorage.h"
#include "../SciterAPI.h"
#include "../Widget.h"
#include "../Private/Conversion.h"
#include "kxf/Core/Enumerator.h"

namespace kxf::Sciter
{
	bool MasterStylesheetStorage::ApplyGlobally() const
	{
		// Reset the master CSS
		constexpr BYTE null[sizeof(void*)] = {};
		GetSciterAPI()->SciterSetMasterCSS(null, 0);
		
		// Apply new master styles
		for (const String& css: EnumItems())
		{
			auto utf8 = ToSciterUTF8(css);
			if (!GetSciterAPI()->SciterAppendMasterCSS(utf8.data(), utf8.size()))
			{
				return false;
			}
		}
		return true;
	}
}
