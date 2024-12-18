#include "kxf-pch.h"
#include "Slider.h"
#include "kxf/System/SystemWindow.h" 

#include <CommCtrl.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Slider, wxSlider);

	bool Slider::Create(wxWindow* parent,
						wxWindowID id,
						int value,
						int minValue,
						int maxValue,
						FlagSet<SliderStyle> style,
						const wxValidator& validator
	)
	{
		style.Mod(SliderStyle::Vertical, style & (SliderStyle::Left|SliderStyle::Right));
		style.Mod(SliderStyle::Horizontal, style & (SliderStyle::Top|SliderStyle::Both));

		// Remove 'SliderStyle::NoThumb' from default wx flags. Just in case.
		if (wxSlider::Create(parent, id, value, minValue, maxValue, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.Remove(SliderStyle::NoThumb).ToInt(), validator))
		{
			SystemWindow window = GetHandle();
			if (style & SliderStyle::Both)
			{
				window.ModWindowStyle(GWL_STYLE, TBS_BOTH, true);
			}
			if (style & SliderStyle::NoThumb)
			{
				window.ModWindowStyle(GWL_STYLE, TBS_NOTHUMB, true);
			}
			return true;
		}
		return false;
	}
}
