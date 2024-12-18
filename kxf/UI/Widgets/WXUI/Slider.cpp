#include "kxf-pch.h"
#include "Slider.h"
#include "kxf/Utility/Common.h"

#include <CommCtrl.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::WXUI
{
	bool Slider::DoTryBefore(wxEvent& event)
	{
		return TranslateScrollEvent(m_Widget, event);
	}

	bool Slider::Create(wxWindow* parent,
						const String& label,
						const Point& pos,
						const Size& size
	)
	{
		if (wxSlider::Create(parent, wxID_NONE, 0, 0, 100, pos, size, wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_SELRANGE, wxDefaultValidator))
		{
			SetLabel(label);
			return true;
		}
		return false;
	}
}
