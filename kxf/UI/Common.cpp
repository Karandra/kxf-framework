#include "kxf-pch.h"
#include "Common.h"
#include "IWidget.h"
#include <wx/event.h>
#include <wx/object.h>
#include <wx/window.h>
#include <wx/control.h>

namespace kxf::Widgets
{
	String RemoveMnemonics(const String& text)
	{
		if (text.Contains('&'))
		{
			return wxControl::RemoveMnemonics(text);
		}
		return text;
	}
	String EscapeMnemonics(const String& text)
	{
		if (text.Contains('&'))
		{
			return wxControl::EscapeMnemonics(text);
		}
		return text;
	}

	Point GetMouseCoordinates() noexcept
	{
		return Point(::wxGetMousePosition());
	}
	Point GetMouseCoordinates(const IWidget& widget) noexcept
	{
		return widget.ScreenToClient(GetMouseCoordinates());
	}
}

namespace kxf::UI
{
	void* GetOwnerWindowHandle(const wxWindow* window) noexcept
	{
		if (window)
		{
			if (window = wxGetTopLevelParent(const_cast<wxWindow*>(window)))
			{
				return window->GetHandle();
			}
			else if (window = wxTheApp->GetTopWindow())
			{
				return window->GetHandle();
			}
			else if (window = wxGetTopLevelParent(wxGetActiveWindow()))
			{
				return window->GetHandle();
			}
		}
		return ::GetShellWindow();
	}
}
