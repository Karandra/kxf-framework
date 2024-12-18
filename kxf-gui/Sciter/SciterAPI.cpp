#include "kxf-pch.h"
#include "SciterAPI.h"
#include "Common.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/System/DynamicLibrary.h"

// Stylesheets
#include "Stylesheets/MasterStylesheetStorage.h"

// Widgets
#include "Widgets/TextBoxWidget.h"

// Native widgets
#include "Widgets/Native/TextBoxWidget.h"

namespace
{
	const constexpr std::wstring_view g_MasterStylesheet =
		#include "Stylesheets/MasterStylesheet.css"
		;

	ISciterAPI* g_SciterAPI = nullptr;
}

namespace kxf::Sciter::Private
{
	void RegisterAPI()
	{
		Log::Info("Registering Sciter API");

		// Register master stylesheets
		MasterStylesheetStorage& stylesheetStorage = MasterStylesheetStorage::GetInstance();
		stylesheetStorage.AddItem(g_MasterStylesheet);

		// Register widgets
		TextBoxWidgetFactory::RegisterInstance();

		// Register native widgets
		NativeTextBoxWidgetFactory::RegisterInstance();

		// Apply global styles
		stylesheetStorage.ApplyGlobally();

		Log::Info("Done registering Sciter API");
	}
}

namespace kxf::Sciter
{
	ISciterAPI* GetSciterAPI()
	{
		if (!g_SciterAPI)
		{
			Log::Info("Sciter isn't loaded yet, trying to load the library (if it's not already loaded) and initialize its API");

			// Try to load using default name to retain original Sciter usage pattern.
			// It just returns the handle, if the library is already loaded.
			DynamicLibrary library;
			library.AttachHandle(LoadLibrary("Sciter.dll"));

			if (library)
			{
				if (auto func = library.GetExportedFunction<SciterAPI_ptr>("SciterAPI"))
				{
					g_SciterAPI = func();
				}

				if (g_SciterAPI)
				{
					Private::RegisterAPI();
				}
				else
				{
					Log::Error("Failed to initialize Sciter API");
				}
			}
			else
			{
				Log::Error("Failed to load Sciter library");
			}
		}
		return g_SciterAPI;
	}
	SciterGraphicsAPI* GetGrapchicsAPI()
	{
		return GetSciterAPI()->GetSciterGraphicsAPI();
	}
	SciterRequestAPI* GetRequestAPI()
	{
		return GetSciterAPI()->GetSciterRequestAPI();
	}
}
