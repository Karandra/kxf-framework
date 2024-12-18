#pragma once
#include "kxf-gui/Widgets/Common.h"
#include "kxf-gui/Widgets/Windows/TopLevelWindow.h"
#include "kxf-gui/Widgets/StdDialogButtonSizer.h"
#include <wx/dialog.h>

namespace kxf::UI
{
	enum class DialogStyle: uint32_t
	{
		None = 0,

		Shaped = wxFRAME_SHAPED,
		NoParent = wxDIALOG_NO_PARENT,

		DefaultStyle = wxDEFAULT_DIALOG_STYLE
	};
	enum class DialogExStyle: uint32_t
	{
		None = 0,

		ContextHelp = wxDIALOG_EX_CONTEXTHELP,
		Metal = wxDIALOG_EX_METAL,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::DialogStyle);
	kxf_FlagSet_Extend(UI::DialogStyle, UI::WindowStyle);

	kxf_FlagSet_Declare(UI::DialogExStyle);
	kxf_FlagSet_Extend(UI::DialogExStyle, UI::WindowExStyle);
}

namespace kxf::UI
{
	class KXF_API Dialog: public TopLevelWindow<wxDialog>
	{
		public:
			static constexpr FlagSet<DialogStyle> DefaultStyle = (DialogStyle::DefaultStyle|DialogStyle::Shaped)|(WindowStyle::None|TopLevelWindowStyle::Resizeable);

		public:
			static void SetStdLabels(wxSizer* sizer);

		public:
			Dialog() = default;
			Dialog(wxWindow* parent,
				   wxWindowID id,
				   const String& title,
				   const Point& pos = Point::UnspecifiedPosition(),
				   const Size& size = Size::UnspecifiedSize(),
				   FlagSet<DialogStyle> style = DefaultStyle
			)
			{
				Create(parent, id, title, pos, size, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& title,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						FlagSet<DialogStyle> style = DefaultStyle
			);

		public:
			virtual void CenterIfNoPosition(const Point& pos);
			StdDialogButtonSizer* CreateStdDialogButtonSizer(FlagSet<StdButton> buttons, FlagSet<StdButton> defaultButtons = StdButton::None);

		public:
			wxDECLARE_DYNAMIC_CLASS(Dialog);
	};
}
