#include "kxf-pch.h"
#include "ApplicationInitializer.h"
#include "kxf/wxWidgets/Application.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/Log/ScopedLogger.h"
#include "Private/NativeApp.h"
#include <wx/init.h>
#include <wx/except.h>

namespace kxf
{
	bool ApplicationInitializer::OnInitCommon()
	{
		wxDISABLE_DEBUG_SUPPORT();
		if (wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, m_Application.GetDisplayName().nc_str()))
		{
			// This will tell 'wxInitialize' to use already existing application instance instead of attempting to create a new one
			m_NativeApp = std::make_unique<Application::Private::NativeApp>(m_Application);
			wxAppConsole::SetInstance(m_NativeApp.get());

			// We're not using the dynamic 'wxApp' initialization
			wxAppConsole::SetInitializerFunction(nullptr);
			return true;
		}
		return false;
	}
	bool ApplicationInitializer::OnInit()
	{
		if (auto commandLine = ::GetCommandLineW())
		{
			if (m_CommandLine = ::CommandLineToArgvW(commandLine, &m_CommandLineCount))
			{
				m_Application.InitializeCommandLine(m_CommandLine, static_cast<size_t>(m_CommandLineCount));
				return wxInitialize(m_CommandLineCount, m_CommandLine);
			}
		}
		return wxInitialize();
	}
	bool ApplicationInitializer::OnInit(int argc, char** argv)
	{
		m_Application.InitializeCommandLine(argv, static_cast<size_t>(argc));
		return wxInitialize(argc, argv);
	}
	bool ApplicationInitializer::OnInit(int argc, wchar_t** argv)
	{
		m_Application.InitializeCommandLine(argv, static_cast<size_t>(argc));
		return wxInitialize(argc, argv);
	}
	void ApplicationInitializer::OnInitDone()
	{
		// Initialization is done successfully, we can assign 'ICoreApplication' instance now.
		ICoreApplication::SetInstance(&m_Application);
	}

	void ApplicationInitializer::OnTerminate()
	{
		KX_SCOPEDLOG_FUNC;

		if (m_IsCreated)
		{
			m_Application.OnDestroy();
			m_IsCreated = false;
		}

		if (m_IsInitializedCommon)
		{
			// Reset application object (and its pointer) prior to calling 'wxUninitialize' as it'll
			// try to call C++ 'operator delete' on the instance if it's still there assuming it was created
			// using 'operator new'.
			wxAppConsole::SetInstance(nullptr);
			m_NativeApp = nullptr;

			if (m_IsInitialized)
			{
				wxUninitialize();
				m_IsInitialized = false;

				// Reset 'ICoreApplication' instance
				ICoreApplication::SetInstance(nullptr);
			}
			m_IsInitializedCommon = false;
		}
		if (m_CommandLine)
		{
			::LocalFree(m_CommandLine);
			m_CommandLine = nullptr;
			m_CommandLineCount = 0;
		}

		KX_SCOPEDLOG.SetSuccess();
	}

	ApplicationInitializer::ApplicationInitializer(ICoreApplication& app)
		:m_Application(app)
	{
		KX_SCOPEDLOG_FUNC;

		RunInitSequence();

		KX_SCOPEDLOG.SetSuccess(IsInitialized());
	}
	ApplicationInitializer::ApplicationInitializer(ICoreApplication& app, int argc, char** argv)
		:m_Application(app)
	{
		KX_SCOPEDLOG_FUNC;

		RunInitSequence(argc, argv);

		KX_SCOPEDLOG.SetSuccess(IsInitialized());
	}
	ApplicationInitializer::ApplicationInitializer(ICoreApplication& app, int argc, wchar_t** argv)
		:m_Application(app)
	{
		KX_SCOPEDLOG_FUNC;

		RunInitSequence(argc, argv);

		KX_SCOPEDLOG.SetSuccess(IsInitialized());
	}
	ApplicationInitializer::~ApplicationInitializer()
	{
		KX_SCOPEDLOG_FUNC;

		OnTerminate();

		KX_SCOPEDLOG.SetSuccess();
	}

	int ApplicationInitializer::Run() noexcept
	{
		KX_SCOPEDLOG_FUNC;

		try
		{
			if (m_Application.OnInit())
			{
				// Ensure that 'OnExit' is called if 'OnInit' had succeeded
				Utility::ScopeGuard callOnExit = [&]()
				{
					// Don't call 'OnExit' if 'OnInit' failed
					m_Application.OnExit();
				};

				// Run the main loop
				int exitCode = m_Application.OnRun();

				KX_SCOPEDLOG.LogReturn(exitCode);
				return exitCode;
			}
		}
		catch (...)
		{
			m_Application.OnUnhandledException();
		}
		return m_Application.GetExitCode().value_or(-1);
	}
}
