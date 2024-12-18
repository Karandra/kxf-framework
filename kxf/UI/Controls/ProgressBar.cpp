#include "kxf-pch.h"
#include "ProgressBar.h"
#include "kxf/Drawing/GDIRenderer/GDIWindowContext.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ProgressBar, wxGauge);

	void ProgressBar::OnPaint(wxPaintEvent& event)
	{
		if (!m_InPulseMode && HasFlag(wxGA_TEXT))
		{
			GDIPaintContext dc(*this);
			wxGauge::OnPaint(event);
			Rect rect(Point(0, 0), Size(GetSize()));

			// Draw label
			const String label = GetLabel();
			if (!label.IsEmpty())
			{
				dc.DrawLabel(Format("{:.2f}", (static_cast<double>(DoGetValue()) / static_cast<double>(DoGetRange())) * 100.0), rect, Alignment::Center);
			}
		}
	}

	bool ProgressBar::Create(wxWindow* parent,
							 wxWindowID id,
							 int range,
							 FlagSet<ProgressBarStyle> style
	)
	{
		if (wxGauge::Create(parent, id, range, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt()))
		{
			Bind(wxEVT_PAINT, &ProgressBar::OnPaint, this);
			return true;
		}
		return false;
	}
}
