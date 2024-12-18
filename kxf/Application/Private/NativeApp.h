#pragma once
#include "../Common.h"
#include "../ICoreApplication.h"
#include "../IGUIApplication.h"
#include "kxf/wxWidgets/Application.h"

namespace kxf::Application::Private
{
	class KXF_API NativeApp final: public wxWidgets::Application
	{
		public:
			static NativeApp* GetInstance()
			{
				return static_cast<NativeApp*>(wxAppConsole::GetInstance());
			}

		private:
			ICoreApplication& m_App;
			std::shared_ptr<IGUIApplication> m_GUIApp;

		private:
			void OnCreate();
			void OnDestroy();

		public:
			NativeApp(ICoreApplication& app)
				:m_App(app), m_GUIApp(app.QueryInterface<IGUIApplication>())
			{
				OnCreate();
			}
			~NativeApp()
			{
				OnDestroy();
			}

		public:
			// Exceptions support
			bool OnExceptionInMainLoop() override
			{
				return m_App.OnMainLoopException();
			}
			void OnUnhandledException() override
			{
				m_App.OnUnhandledException();
			}
			void OnFatalException() override
			{
				m_App.OnFatalException();
			}
			void OnAssertFailure(const wxChar* file, int line, const wxChar* function, const wxChar* condition, const wxChar* message) override
			{
				m_App.OnAssertFailure(file, line, function, condition, message);
			}

			bool StoreCurrentException() override
			{
				return m_App.StoreCurrentException();
			}
			void RethrowStoredException() override
			{
				m_App.RethrowStoredException();
			}

			// Callbacks for application-wide events
			bool OnInit() override
			{
				return false;
			}
			int OnExit() override
			{
				return -1;
			}
			int OnRun() override
			{
				return m_App.OnRun();
			}

			// Event handling
			int MainLoop() override
			{
				return -1;
			}
			void ExitMainLoop() override
			{
				m_App.ExitMainLoop();
			}
			void OnEventLoopEnter(wxEventLoopBase* loop) override
			{
				Application::OnEventLoopEnter(loop);
			}
			void OnEventLoopExit(wxEventLoopBase* loop) override
			{
				Application::OnEventLoopExit(loop);
			}

			bool Pending() override
			{
				return m_App.Pending();
			}
			bool Dispatch() override
			{
				return m_App.Dispatch();
			}
			bool ProcessIdle() override
			{
				return m_App.DispatchIdle();
			}
			void WakeUpIdle() override
			{
				m_App.WakeUp();
			}

			// Pending events
			void ProcessPendingEvents() override
			{
				m_App.ProcessPendingEventHandlers();
				Application::ProcessPendingEvents();
			}
			void DeletePendingObjects()
			{
				Application::DeletePendingObjects();
			}

			// Command line
			void OnInitCmdLine(wxCmdLineParser& parser) override
			{
			}
			bool OnCmdLineParsed(wxCmdLineParser& parser) override
			{
				return true;
			}
			bool OnCmdLineError(wxCmdLineParser& parser) override
			{
				return true;
			}
			bool OnCmdLineHelp(wxCmdLineParser& parser) override
			{
				return true;
			}

			// GUI
			bool IsActive() const override
			{
				return m_GUIApp ? m_GUIApp->IsActive() : false;
			}
			wxWindow* GetTopWindow() const override;
			
			wxLayoutDirection GetLayoutDirection() const override
			{
				if (m_GUIApp)
				{
					switch (m_GUIApp->GetLayoutDirection())
					{
						case LayoutDirection::LeftToRight:
						{
							return wxLayoutDirection::wxLayout_LeftToRight;
						}
						case LayoutDirection::RightToLeft:
						{
							return wxLayoutDirection::wxLayout_RightToLeft;
						}
					};
				}
				return wxLayoutDirection::wxLayout_Default;
			}
			bool SetNativeTheme(const wxString& themeName) override
			{
				return m_GUIApp ? m_GUIApp->SetNativeTheme(themeName) : false;
			}

			bool SafeYield(wxWindow* window, bool onlyIfNeeded) override;
			bool SafeYieldFor(wxWindow* window, long eventsToProcess) override;
	};
}
