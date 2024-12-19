#pragma once
#include "kxf/Common.hpp"
#include "WidgetID.h"
#include "kxf/Core/String.h"
#include "kxf/Core/StdID.h"
#include "kxf/Core/StdIcon.h"
#include "kxf/Core/StdButton.h"
#include "kxf/Core/KeyCode.h"
#include "kxf/Core/MouseState.h"
#include "kxf/Core/KeyboardState.h"
#include "kxf/EventSystem/Common.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/Drawing/Color.h"
#include "kxf/Drawing/Angle.h"
#include "kxf/Drawing/Font.h"
#include "kxf/Drawing/BitmapImage.h"
#include "kxf/Drawing/ImageBundle.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIFont.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIBitmap.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIIcon.h"

#include <wx/window.h>
#include "kxf/Win32/UndefMacros.h"
class wxObject;
class wxWindow;
class wxEvtHandler;

namespace kxf
{
	class IWidget;

	enum class WidgetStyle: uint64_t
	{
		None = 0,

		Invisible = FlagSetValue<WidgetStyle>(0),
		AllKeyEvents = FlagSetValue<WidgetStyle>(1),

		ScrollVertical = FlagSetValue<WidgetStyle>(2),
		ScrollHorizontal = FlagSetValue<WidgetStyle>(3),
		ScrollShowAlways = FlagSetValue<WidgetStyle>(4)
	};
	kxf_FlagSet_Declare(WidgetStyle);

	enum class WidgetExStyle: uint64_t
	{
		None = 0
	};
	kxf_FlagSet_Declare(WidgetExStyle);

	enum class WidgetBorder: uint64_t
	{
		None = 0,

		Default,
		Simple,
		Static,
		Theme
	};
	kxf_FlagSet_Declare(WidgetBorder);

	enum class WidgetSizeFlag: uint32_t
	{
		None = 0,

		Widget = FlagSetValue<WidgetSizeFlag>(0),
		WidgetMin = FlagSetValue<WidgetSizeFlag>(1),
		WidgetMax = FlagSetValue<WidgetSizeFlag>(2),
		WidgetBest = FlagSetValue<WidgetSizeFlag>(3),

		Client = FlagSetValue<WidgetSizeFlag>(4),
		ClientMin = FlagSetValue<WidgetSizeFlag>(5),
		ClientMax = FlagSetValue<WidgetSizeFlag>(6),

		Virtual = FlagSetValue<WidgetSizeFlag>(7),
		VirtualBest = FlagSetValue<WidgetSizeFlag>(8),

		Border = FlagSetValue<WidgetSizeFlag>(9)
	};
	kxf_FlagSet_Declare(WidgetSizeFlag);

	enum class WidgetColorFlag: uint32_t
	{
		None = 0,

		Background = FlagSetValue<WidgetColorFlag>(0),
		Foreground = FlagSetValue<WidgetColorFlag>(1),
		Border = FlagSetValue<WidgetColorFlag>(2),
		Text = FlagSetValue<WidgetColorFlag>(3)
	};
	kxf_FlagSet_Declare(WidgetColorFlag);

	enum class WidgetTextFlag: uint32_t
	{
		None = 0,

		WithMnemonics = FlagSetValue<WidgetTextFlag>(0)
	};
	kxf_FlagSet_Declare(WidgetTextFlag);
}

namespace kxf::Widgets
{
	KXF_API String RemoveMnemonics(const String& text);
	KXF_API String EscapeMnemonics(const String& text);

	KXF_API Point GetMouseCoordinates() noexcept;
	KXF_API Point GetMouseCoordinates(const IWidget& widget) noexcept;

	KXF_API std::shared_ptr<IWidget> GetFocusedWidget() noexcept;
}

namespace kxf::UI
{
	enum class WindowStyle: uint32_t
	{
		None = 0,

		Transparent = 0x00100000,
		TabTraversal = 0x00080000,
		WantsChars = 0x00040000,
		ClipChildren = 0x00400000,
		FullRepaintOnResize = 0x00010000,
		ScrollVertical = 0x80000000,
		ScrollHorizontal = 0x40000000,
		ScrollShowAlways = 0x00800000,
	};
	enum class WindowExStyle: uint32_t
	{
		None = 0,

		Transient = 0x00000004,
		BlockEvents = 0x00000002,
		ContextHelp = 0x00000080,
		ProcessIdle = 0x00000010,
		ProcessUIUpdates = 0x00000020,
	};
	enum class WindowBorder: uint32_t
	{
		None = 0,

		Default = 0,
		Theme = 0x10000000,
		Static = 0x01000000,
		Simple = 0x02000000,
		Raised = 0x04000000,
		Sunken = 0x08000000
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::WindowStyle);
	kxf_FlagSet_Declare(UI::WindowExStyle);

	kxf_FlagSet_Declare(UI::WindowBorder);
	kxf_FlagSet_Extend(UI::WindowBorder, UI::WindowStyle);
}

namespace kxf::UI
{
	// Get any valid HWND for functions that requires parent window in all cases.
	// Return HWND of windows in following chain:
	// window -> wxTheApp->GetTopWindow() -> wxGetTopLevelParent(wxGetActiveWindow()) -> ::GetShellWindow()
	KXF_API void* GetOwnerWindowHandle(const wxWindow* window) noexcept;
}
