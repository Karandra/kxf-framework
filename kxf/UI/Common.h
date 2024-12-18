#pragma once
#include "kxf/Common.hpp"
#include "StdIcon.h"
#include "StdButton.h"
#include "KeyCode.h"
#include "MouseState.h"
#include "KeyboardState.h"
#include "WidgetID.h"
#include "kxf/Core/String.h"
#include "kxf/Core/DateTime.h"
#include "kxf/Core/StdID.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/Drawing/Color.h"
#include "kxf/Drawing/Angle.h"
#include "kxf/Drawing/Font.h"
#include "kxf/Drawing/BitmapImage.h"
#include "kxf/Drawing/ImageBundle.h"
#include "kxf/Drawing/GDIRenderer/GDIFont.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"
#include "kxf/Drawing/GDIRenderer/GDIIcon.h"
#include "kxf/EventSystem/Common.h"
#include "kxf/EventSystem/Event.h"

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

	enum class LayoutDirection
	{
		Default = -1,
		LeftToRight,
		RightToLeft
	};
}

namespace kxf::Widgets
{
	String RemoveMnemonics(const String& text);
	String EscapeMnemonics(const String& text);

	Point GetMouseCoordinates() noexcept;
	Point GetMouseCoordinates(const IWidget& widget) noexcept;
}

namespace kxf::UI
{
	enum class WindowStyle: uint32_t
	{
		None = 0,

		Transparent = wxTRANSPARENT_WINDOW,
		TabTraversal = wxTAB_TRAVERSAL,
		WantsChars = wxWANTS_CHARS,
		ClipChildren = wxCLIP_CHILDREN,
		FullRepaintOnResize = wxFULL_REPAINT_ON_RESIZE,
		NoFullRepaintOnResize = wxNO_FULL_REPAINT_ON_RESIZE,
		ScrollVertical = static_cast<uint32_t>(wxVSCROLL),
		ScrollHorizontal = static_cast<uint32_t>(wxHSCROLL),
		ScrollShowAlways = wxALWAYS_SHOW_SB,
	};
	enum class WindowExStyle: uint32_t
	{
		None = 0,

		Transient = wxWS_EX_TRANSIENT,
		BlockEvents = wxWS_EX_BLOCK_EVENTS,
		ContextHelp = wxWS_EX_CONTEXTHELP,
		ProcessIdle = wxWS_EX_PROCESS_IDLE,
		ProcessUIUpdates = wxWS_EX_PROCESS_UI_UPDATES,
	};
	enum class WindowBorder: uint32_t
	{
		None = 0,

		Default = wxBORDER_DEFAULT,
		Simple = wxBORDER_SIMPLE,
		Sunken = wxBORDER_SUNKEN,
		Raised = wxBORDER_RAISED,
		Static = wxBORDER_STATIC,
		Theme = wxBORDER_THEME,
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
