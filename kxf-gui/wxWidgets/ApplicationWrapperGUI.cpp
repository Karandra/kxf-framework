#include "kxf-pch.h"
#include "ApplicationWrapperGUI.h"
#include "kxf-gui/Widgets/ITopLevelWidget.h"

namespace kxf::wxWidgets
{
	wxWindow* ApplicationWrapperGUI::GetTopWindow() const
	{
		if (auto widget = m_GUIApp.GetTopWidget())
		{
			return widget->GetWxWindow();
		}
		return nullptr;
	}

	bool ApplicationWrapperGUI::SafeYield(wxWindow* window, bool onlyIfNeeded)
	{
		if (window)
		{
			if (auto widget = kxf::Private::FindByWXObject(*window))
			{
				return m_GUIApp.Yield(*widget, FlagSet<EventYieldFlag>().Add(EventYieldFlag::OnlyIfRequired, onlyIfNeeded));
			}
		}
		return false;
	}
	bool ApplicationWrapperGUI::SafeYieldFor(wxWindow* window, long eventsToProcess)
	{
		if (window)
		{
			if (auto widget = kxf::Private::FindByWXObject(*window))
			{
				return m_GUIApp.YieldFor(*widget, static_cast<EventCategory>(eventsToProcess));
			}
		}
		return false;
	}
}
