#pragma once
#include "Common.h"
#include "kxf/Drawing/Color.h"
#include "kxf/Core/String.h"
#include <wx/sysopt.h>

namespace kxf::wxWidgets::SystemOption
{
	// If set to non-zero value, abort the program if an assertion fails. The default behaviour
	// in case of assertion failure depends on the build mode and can be changed by overriding
	// wxApp::OnAssertFailure() but setting this option allows changing it without modifying
	// the program code and also applies to asserts which may happen before the wxApp object
	// creation or after its destruction.
	constexpr char ExitOnAssert[] = "exit-on-assert";

	// If 1 (the default), wxToolBar bitmap colours will be remapped to the current theme's values.
	// Set this to 0 to disable this functionality, for example if you're using more than 16 colours in your tool bitmaps
	constexpr char Remap[] = "msw.remap";

	// 1 to never use WIN32's MaskBlt function, 0 to allow it to be used where possible.
	// Default: 0. In some circumstances the MaskBlt function can be slower than using
	// the fallback code, especially if using DC caching. By default, MaskBlt will be used
	// where it is implemented by the operating system and driver.
	constexpr char NoMaskBlt[] = "no-maskblt";

	// If 1, windows will not automatically get the WS_CLIPCHILDREN style. This restores the way windows are refreshed back
	// to the method used in versions of wxWidgets earlier than 2.5.4, and for some complex window hierarchies it can reduce
	// apparent refresh delays. You may still specify wxCLIP_CHILDREN for individual windows.
	constexpr char WindowNoClipChildren[] = "msw.window.no-clip-children";

	// If set to 0, globally disables themed backgrounds on notebook pages. Note that this won't disable the theme on the actual
	// notebook background (noticeable only if there are no pages).
	constexpr char NotebookThemedBackground[] = "msw.notebook.themed-background";

	// If set to 0, switches off optimized wxStaticBox painting. Setting this to 0 causes more flicker, but allows applications
	// to paint graphics on the parent of a static box (the optimized refresh causes any such drawing to disappear).
	constexpr char StaticBoxOptimizedPaint[] = "msw.staticbox.optimized-paint";

	// If set to 1, use default fonts quality instead of proof quality when creating fonts. With proof quality the fonts have
	// slightly better appearance but not all fonts are available in this quality, e.g. the Terminal font in small sizes is not
	// and this option may be used if wider fonts selection is more important than higher quality.
	constexpr char NoProofQuality[] = "msw.font.no-proof-quality";
}

namespace kxf::wxWidgets
{
	class KXF_API SystemOptions final
	{
		public:
			static String GetOption(const String& name)
			{
				return wxSystemOptions::GetOption(name);
			}
			static Color GetOptionColor(const String& name)
			{
				if (wxSystemOptions::HasOption(name))
				{
					return Color::FromCOLORREF(static_cast<uint32_t>(wxSystemOptions::GetOptionInt(name)));
				}
				return {};
			}
			static std::optional<bool> GetOptionBool(const String& name)
			{
				if (wxSystemOptions::HasOption(name))
				{
					return wxSystemOptions::GetOptionInt(name) != 0;
				}
				return {};
			}
			static std::optional<int> GetOptionInt(const String& name)
			{
				if (wxSystemOptions::HasOption(name))
				{
					return wxSystemOptions::GetOptionInt(name);
				}
				return {};
			}

			static void SetOption(const String& name, const String& value)
			{
				wxSystemOptions::SetOption(name, value);
			}
			static void SetOption(const String& name, const Color& value)
			{
				wxSystemOptions::SetOption(name, static_cast<int>(value.GetCOLORREF()));
			}
			static void SetOption(const String& name, bool value)
			{
				wxSystemOptions::SetOption(name, value ? 1 : 0);
			}
			static void SetOption(const String& name, int value)
			{
				wxSystemOptions::SetOption(name, value);
			}

		public:
			SystemOptions() = delete;
			~SystemOptions() = delete;
	};
}
