#include "kxf-pch.h"
#include "Utility.h"

#include <Windows.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::Application::Private
{
	void OnUnhandledException()
	{
		// We're called from an exception handler so we can re-throw the exception to recover its type
		String what;
		try
		{
			throw;
		}
		catch (std::exception& e)
		{
			what = Format("standard exception of type \"{}\" with message \"{}\"", typeid(e).name(), e.what());
		}
		catch (...)
		{
			what = "unknown exception";
		}

		auto message = Format("Unhandled {}; terminating {}.\n", what, wxIsMainThread() ? "the application" : "the thread in which it happened");
		wxMessageOutputBest().Printf("%s", message.data());
	}
	void OnFatalException()
	{
		// Nothing to do
	}

	bool OnMainLoopException()
	{
		throw;
	}
	bool OnMainLoopExceptionGUI()
	{
		// Ask the user about what to do: use the WinAPI function here as it could be dangerous to use any framework code in this state.
		// TODO: Use 'TaskDialog[Indirect]' instead.
		const auto result = ::MessageBoxW(nullptr,
										  L"An unhandled exception occurred. Press \"Abort\" to terminate the program,\r\n\"Retry\" to exit the program normally and \"Ignore\" to try to continue.",
										  L"Unhandled exception",
										  MB_ABORTRETRYIGNORE|MB_ICONERROR|MB_TASKMODAL);
		switch (result)
		{
			case IDABORT:
			{
				throw;
			}
			case IDIGNORE:
			{
				return true;
			}
		};
		return false;
	}
}
