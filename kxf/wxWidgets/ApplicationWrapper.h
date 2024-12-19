#pragma once
#include "kxf/Application/ICoreApplication.h"
#include <wx/app.h>

namespace kxf::wxWidgets
{
	class KXF_API Application
	{
		public:
			virtual ~Application() = default;

		public:
			virtual wxAppConsole* Get() noexcept = 0;
	};

	class KXF_API ApplicationConsole: public RTTI::Interface<ApplicationConsole>, public Application, public wxAppConsole
	{
		kxf_RTTI_DeclareIID(ApplicationConsole, {0x912d2b7f, 0x51aa, 0x43e5, {0xa6, 0x0, 0xc4, 0xad, 0xb8, 0x59, 0xdf, 0x69}});

		public:
			static ApplicationConsole* GetInstance()
			{
				return static_cast<ApplicationConsole*>(wxAppConsole::GetInstance());
			}

		public:
			wxAppConsole* Get() noexcept override
			{
				return this;
			}
	};

	class KXF_API ApplicationGUI: public RTTI::Interface<ApplicationGUI>, public Application, public wxApp
	{
		kxf_RTTI_DeclareIID(ApplicationGUI, {0xa856ae88, 0x6783, 0x44b8, {0x96, 0x6c, 0xe8, 0x20, 0x32, 0x6, 0xd9, 0x94}});

		public:
			static ApplicationGUI* GetInstance()
			{
				return static_cast<ApplicationGUI*>(wxApp::GetInstance());
			}

		public:
			wxAppConsole* Get() noexcept override
			{
				return this;
			}
	};
}

namespace kxf::wxWidgets
{
	template<class TDerived, class TApp>
	class ApplicationWrapperCommon: public TApp
	{
		public:
			static TDerived* GetInstance()
			{
				return static_cast<TDerived*>(TApp::GetInstance());
			}

		protected:
			ICoreApplication& m_App;

		public:
			ApplicationWrapperCommon(ICoreApplication& app)
				:m_App(app)
			{
			}
			~ApplicationWrapperCommon() = default;

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
				TApp::OnEventLoopEnter(loop);
			}
			void OnEventLoopExit(wxEventLoopBase* loop) override
			{
				TApp::OnEventLoopExit(loop);
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
				TApp::ProcessPendingEvents();
			}
			void DeletePendingObjects()
			{
				TApp::DeletePendingObjects();
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
	};
}
