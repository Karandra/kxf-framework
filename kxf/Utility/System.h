#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include <wx/buffer.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::Utility
{
	const wxScopedCharBuffer LoadResource(const String& resourceName, const String& resourceType = wxS("STRING"));
	const wxScopedCharBuffer LoadResource(int resourceID, const String& resourceType = wxS("STRING"));

	intptr_t ModWindowStyle(void* windowHandle, int index, intptr_t style, bool enable) noexcept;
}