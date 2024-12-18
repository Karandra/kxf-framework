#pragma once
#include "kxf/RTTI/RTTI.h"
#include <wx/app.h>

namespace kxf::wxWidgets
{
	class KXF_API ApplicationConsole: public RTTI::Interface<ApplicationConsole>, public wxAppConsole
	{
		kxf_RTTI_DeclareIID(ApplicationConsole, {0x912d2b7f, 0x51aa, 0x43e5, {0xa6, 0x0, 0xc4, 0xad, 0xb8, 0x59, 0xdf, 0x69}});

		public:
			virtual ~ApplicationConsole() = default;
	};

	class KXF_API Application: public RTTI::Interface<Application>, public wxApp
	{
		kxf_RTTI_DeclareIID(Application, {0xa856ae88, 0x6783, 0x44b8, {0x96, 0x6c, 0xe8, 0x20, 0x32, 0x6, 0xd9, 0x94}});

		public:
			virtual ~Application() = default;
	};
}
