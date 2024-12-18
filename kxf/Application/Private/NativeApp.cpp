#include "kxf-pch.h"
#include "NativeApp.h"
#include "kxf-gui/Widgets/ITopLevelWidget.h"

namespace kxf::Application::Private
{
	// NativeApp
	void NativeApp::OnCreate()
	{
	}
	void NativeApp::OnDestroy()
	{
	}

	// GUI
	wxWindow* NativeApp::GetTopWindow() const
	{
		if (m_GUIApp)
		{
			if (auto widget = m_GUIApp->GetTopWidget())
			{
				return widget->GetWxWindow();
			}
		}
		return nullptr;
	}

	bool NativeApp::SafeYield(wxWindow* window, bool onlyIfNeeded)
	{
		if (m_GUIApp && window)
		{
			if (auto widget = kxf::Private::FindByWXObject(*window))
			{
				return m_GUIApp->Yield(*widget, FlagSet<EventYieldFlag>().Add(EventYieldFlag::OnlyIfRequired, onlyIfNeeded));
			}
		}
		return false;
	}
	bool NativeApp::SafeYieldFor(wxWindow* window, long eventsToProcess)
	{
		if (m_GUIApp && window)
		{
			if (auto widget = kxf::Private::FindByWXObject(*window))
			{
				return m_GUIApp->YieldFor(*widget, static_cast<EventCategory>(eventsToProcess));
			}
		}
		return false;
	}
}
