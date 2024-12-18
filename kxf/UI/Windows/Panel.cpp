#include "kxf-pch.h"
#include "Panel.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Panel, wxPanel);

	bool Panel::Create(wxWindow* parent,
					   wxWindowID id,
					   FlagSet<WindowStyle> style
	)
	{
		if (wxPanel::Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt()))
		{
			EnableSystemTheme();
			return true;
		}
		return false;
	}
}
