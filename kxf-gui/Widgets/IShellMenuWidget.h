#pragma once
#include "Common.h"
#include "IMenuWidget.h"
#include "kxf/System/HResult.h"

namespace kxf
{
	class FSPath;
}

namespace kxf
{
	class KXF_API IShellMenuWidget: public RTTI::Interface<IShellMenuWidget>
	{
		kxf_RTTI_DeclareIID(IShellMenuWidget, {0x1ffdf7d1, 0xd1c4, 0x463a, {0xa3, 0x91, 0x89, 0x60, 0x9e, 0x48, 0x1c, 0x7d}});

		public:
			virtual HResult InitializeFromFSObject(const FSPath& path) = 0;
	};
}
