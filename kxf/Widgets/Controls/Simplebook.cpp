#include "kxf-pch.h"
#include "Simplebook.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Simplebook, wxSimplebook);

	bool Simplebook::Create(wxWindow* parent,
							  wxWindowID id,
							  long style
	)
	{
		if (wxSimplebook::Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style))
		{
			if (ShouldInheritColours())
			{
				SetBackgroundColour(parent->GetBackgroundColour());
			}
			return true;
		}
		return false;
	}
}
