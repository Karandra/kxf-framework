#pragma once
#include "Kx/Common.hpp"
#include <wx/event.h>
#include <wx/windowid.h>

namespace KxFramework
{
	using EventID = wxEventType;
	using EventSourceID = wxWindowID;
	
	template<class T>
	using EventTag = wxEventTypeTag<T>;
}
