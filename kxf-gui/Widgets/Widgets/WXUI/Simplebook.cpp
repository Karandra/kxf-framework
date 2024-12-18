#include "kxf-pch.h"
#include "Simplebook.h"

namespace kxf::WXUI
{
	bool Simplebook::DoTryBefore(wxEvent& event)
	{
		return TranslateBookEvent(m_Widget, event);
	}

	bool Simplebook::Create(wxWindow* parent,
							const String& label,
							const Point& pos,
							const Size& size
	)
	{
		if (wxSimplebook::Create(parent, wxID_NONE, pos, size))
		{
			SetLabel(label);
			if (ShouldInheritColours())
			{
				SetBackgroundColour(parent->GetBackgroundColour());
			}

			return true;
		}
		return false;
	}
}
