#pragma once
#include "Common.h"

#define KX_DefineLogCategory(name)						constexpr kxf::StringView name = kxfS(#name)
#define KX_DefineLogModuleCategory(module, name)		constexpr kxf::StringView module##_##name = kxfS(#module "/" #name)

namespace kxf::LogCategory
{
	KX_DefineLogCategory(kxf);
	KX_DefineLogCategory(WinAPI);
	KX_DefineLogCategory(wxWidgets);

	KX_DefineLogModuleCategory(kxf, Application);
	KX_DefineLogModuleCategory(kxf, Async);
	KX_DefineLogModuleCategory(kxf, Compression);
	KX_DefineLogModuleCategory(kxf, Core);
	KX_DefineLogModuleCategory(kxf, Crypto);
	KX_DefineLogModuleCategory(kxf, Drawing);
	KX_DefineLogModuleCategory(kxf, EventSystem);
	KX_DefineLogModuleCategory(kxf, FileSystem);
	KX_DefineLogModuleCategory(kxf, IO);
	KX_DefineLogModuleCategory(kxf, IPC);
	KX_DefineLogModuleCategory(kxf, Localization);
	KX_DefineLogModuleCategory(kxf, Network);
	KX_DefineLogModuleCategory(kxf, RTTI);
	KX_DefineLogModuleCategory(kxf, Sciter);
	KX_DefineLogModuleCategory(kxf, Serialization);
	KX_DefineLogModuleCategory(kxf, System);
	KX_DefineLogModuleCategory(kxf, Threading);
	KX_DefineLogModuleCategory(kxf, UI);
	KX_DefineLogModuleCategory(kxf, Utility);
	KX_DefineLogModuleCategory(kxf, wxWidgets);
}
