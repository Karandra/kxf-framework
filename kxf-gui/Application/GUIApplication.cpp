#include "kxf-pch.h"
#include "GUIApplication.h"
#include "kxf/Application/Private/Utility.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/System/SystemAppearance.h"
#include "kxf/Utility/Container.h"
#include "kxf/Log/ScopedLogger.h"
#include "Private/Win32GUIEventLoop.h"
#include "kxf-gui/Widgets/ITopLevelWidget.h"
#include "kxf-gui/wxWidgets/ApplicationWrapperGUI.h"
#include <uxtheme.h>

namespace kxf
{
	// GUIApplication
	void GUIApplication::DeleteAllTopLevelWindows()
	{
		// TLWs remove themselves from 'wxTopLevelWindows' when destroyed, so iterate until none are left.
		while (!wxTopLevelWindows.empty())
		{
			// Do not use 'Destroy' here as it only puts the TLW in pending list but we want to delete them now.
			delete wxTopLevelWindows.GetFirst()->GetData();
		}
	}

	// ICoreApplication -> Main Event Loop
	std::shared_ptr<IEventLoop> GUIApplication::CreateMainLoop()
	{
		return std::make_shared<EventSystem::Private::Win32GUIEventLoop>();
	}

	// ICoreApplication -> Active Event Loop
	bool GUIApplication::DispatchIdle()
	{
		// Send an event to the application instance itself first
		bool needMore = CoreApplication::DispatchIdle();
		if (auto nativeApp = wxApp::GetInstance())
		{
			if (nativeApp->ProcessIdle())
			{
				needMore = true;
			}
		}

		ReadLockGuard lock(m_ScheduledForDestructionLock);
		for (wxWindow* window: wxTopLevelWindows)
		{
			// Don't send idle events to the windows that are about to be destroyed anyhow, this is wasteful and unexpected.
			wxIdleEvent event;
			if (window && !wxTheApp->IsScheduledForDestruction(window) && window->SendIdleEvents(event))
			{
				needMore = true;
			}
		}

		wxUpdateUIEvent::ResetUpdateTime();
		return needMore;
	}

	// ICoreApplication -> Pending Events
	void GUIApplication::FinalizeScheduledForDestruction()
	{
		if (auto app = wxWidgets::ApplicationWrapperGUI::GetInstance())
		{
			app->DeletePendingObjects();
		}

		if (WriteLockGuard lock(m_ScheduledForDestructionLock); !m_ScheduledForDestruction.empty())
		{
			for (auto& item: m_ScheduledForDestruction)
			{
				if (auto widget = item->QueryInterface<IWidget>())
				{
					widget->DestroyWidget();
				}
			}
			m_ScheduledForDestruction.clear();
		}
	}

	// ICoreApplication -> Exception Handler
	bool GUIApplication::OnMainLoopException()
	{
		return Application::Private::OnMainLoopExceptionGUI();
	}

	// ICoreApplication -> Application
	bool GUIApplication::OnCreate()
	{
		KXF_SCOPEDLOG_FUNC;

		m_LayoutDirection = GUIApplication::GetLayoutDirection();
		if (auto app = wxApp::GetInstance())
		{
			int argc = m_ArgC;
			m_NativeAppInitialized = app->Initialize(argc, m_ArgVW);

			if (!m_NativeAppInitialized)
			{
				return false;
			}
		}

		KXF_SCOPEDLOG.SetSuccess();
		return CoreApplication::OnCreate();
	}
	void GUIApplication::OnDestroy()
	{
		if (m_NativeAppInitialized)
		{
			if (auto app = wxApp::GetInstance())
			{
				app->CleanUp();
				m_NativeAppCleanedUp = true;
			}
		}
		CoreApplication::OnDestroy();
	}
	int GUIApplication::OnRun()
	{
		// If the initial value hasn't been changed, use the default 'ExitOnLastFrameDelete::Always' from now on.
		if (m_ExitWhenLastWidgetDestroyed == ExitWhenLastWidgetDestroyed::Later)
		{
			m_ExitWhenLastWidgetDestroyed = ExitWhenLastWidgetDestroyed::Always;

			// Notify the wxWidgets application
			if (auto app = static_cast<wxApp*>(wxApp::GetInstance()))
			{
				app->SetExitOnFrameDelete(true);
			}
		}

		// Run the main loop
		return CoreApplication::OnRun();
	}

	std::shared_ptr<kxf::wxWidgets::Application> GUIApplication::CreateWXApp()
	{
		return std::make_shared<kxf::wxWidgets::ApplicationWrapperGUI>(*this, *this);
	}

	// IGUIApplication
	std::shared_ptr<ITopLevelWidget> GUIApplication::GetTopWidget() const
	{
		ReadLockGuard lock(m_ScheduledForDestructionLock);

		// If there is no top window or it is about to be destroyed, we need to search for the first TLW which is not pending delete.
		if (m_TopWidget && !IsScheduledForDestruction(*m_TopWidget))
		{
			return m_TopWidget;
		}
		else
		{
			for (wxWindow* window: wxTopLevelWindows)
			{
				if (window && !wxTheApp->IsScheduledForDestruction(window))
				{
					return Private::FindByWXObject<ITopLevelWidget>(*window);
				}
			}
		}
		return nullptr;
	}
	void GUIApplication::SetTopWidget(std::shared_ptr<ITopLevelWidget> widget)
	{
		m_TopWidget = widget;

		if (auto app = static_cast<wxApp*>(wxApp::GetInstance()))
		{
			app->SetTopWindow(widget ? widget->GetWxWindow() : nullptr);
		}
	}

	bool GUIApplication::ShoudExitWhenLastWidgetDestroyed() const
	{
		return m_ExitWhenLastWidgetDestroyed == ExitWhenLastWidgetDestroyed::Always;
	}
	void GUIApplication::ExitWhenLastWidgetDestroyed(bool enable)
	{
		m_ExitWhenLastWidgetDestroyed = enable ? ExitWhenLastWidgetDestroyed::Always : ExitWhenLastWidgetDestroyed::Never;
		if (auto app = static_cast<wxApp*>(wxApp::GetInstance()))
		{
			app->SetExitOnFrameDelete(enable);
		}
	}

	bool GUIApplication::IsActive() const
	{
		return m_IsActive;
	}
	void GUIApplication::SetActive(bool active, std::shared_ptr<IWidget> widget)
	{
		if (m_IsActive != active)
		{
			m_IsActive = active;

			BuildProcessEvent(ICoreApplication::EvtActivated, active, ActivateEventReason::Unknown)
				.SetSourceToSelf()
				.Process();
		}
	}

	LayoutDirection GUIApplication::GetLayoutDirection() const
	{
		if (m_LayoutDirection != LayoutDirection::Default)
		{
			return m_LayoutDirection;
		}
		else if (const wxLocale* locale = wxGetLocale())
		{
			if (const wxLanguageInfo* info = wxLocale::GetLanguageInfo(locale->GetLanguage()))
			{
				switch (info->LayoutDirection)
				{
					case wxLayout_LeftToRight:
					{
						return LayoutDirection::LeftToRight;
					}
					case wxLayout_RightToLeft:
					{
						return LayoutDirection::RightToLeft;
					}
				};
			}

			DWORD layoutDirection = 0;
			if (::GetProcessDefaultLayout(&layoutDirection))
			{
				return layoutDirection == LAYOUT_RTL ? LayoutDirection::RightToLeft : LayoutDirection::LeftToRight;
			}
		}

		// We don't know
		return LayoutDirection::Default;
	}
	void GUIApplication::SetLayoutDirection(LayoutDirection direction)
	{
		m_LayoutDirection = direction;
		::SetProcessDefaultLayout(direction == LayoutDirection::RightToLeft ? LAYOUT_RTL : 0);
	}

	String GUIApplication::GetNativeTheme() const
	{
		if (System::IsWindowsVersionOrGreater(NamedSystemRelease::Windows11))
		{
			return SystemAppearance::IsDark() ? "FluentUI Dark" : "FluentUI Light";
		}
		else if (System::IsWindowsVersionOrGreater(NamedSystemRelease::Windows10))
		{
			return SystemAppearance::IsDark() ? "ModernUI Dark" : "ModernUI Light";
		}
		else if (System::IsWindowsVersionOrGreater(NamedSystemRelease::Windows8))
		{
			return "MetroUI";
		}
		else if (System::IsWindowsVersionOrGreater(NamedSystemRelease::WindowsVista))
		{
			return ::IsAppThemed() ? "Aero" : "Classic";
		}
		else if (System::IsWindowsVersionOrGreater(NamedSystemRelease::WindowsXP))
		{
			return ::IsAppThemed() ? "Luna" : "Classic";
		}
		return {};
	}
	bool GUIApplication::SetNativeTheme(const String& themeName)
	{
		return false;
	}

	bool GUIApplication::Yield(IWidget& widget, FlagSet<EventYieldFlag> flags)
	{
		wxWindowDisabler windowDisabler(widget.GetWxWindow());

		IEventLoop* activeLoop = GetActiveEventLoop();
		return activeLoop && activeLoop->Yield(flags);
	}
	bool GUIApplication::YieldFor(IWidget& widget, FlagSet<EventCategory> toProcess)
	{
		wxWindowDisabler windowDisabler(widget.GetWxWindow());

		IEventLoop* activeLoop = GetActiveEventLoop();
		return activeLoop && activeLoop->YieldFor(toProcess);
	}
}
