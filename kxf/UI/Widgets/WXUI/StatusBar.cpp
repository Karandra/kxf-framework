#include "kxf-pch.h"
#include "StatusBar.h"

namespace
{
	constexpr int g_DefaultMinHeight = 23;
}

namespace kxf::WXUI
{
	bool StatusBar::Create(wxWindow* parent,
						   const String& label,
						   const Point& pos,
						   const Size& size
	)
	{
		if (wxStatusBar::Create(parent, wxID_NONE, wxSTB_SIZEGRIP|wxSTB_SHOW_TIPS|wxSTB_ELLIPSIZE_END|wxFULL_REPAINT_ON_RESIZE))
		{
			SetLabel(label);
			if (pos.IsFullySpecified())
			{
				SetPosition(pos);
			}
			if (size.IsFullySpecified())
			{
				SetSize(size);
			}

			EnableSystemTheme();
			SetDoubleBuffered(true);
			SetMinHeight(FromDIP(g_DefaultMinHeight));

			return true;
		}
		return false;
	}

	// wxStatusBar
	void StatusBar::SetMinHeight(int height)
	{
		SetMinSize(Size(wxDefaultCoord, height));
		wxStatusBar::SetMinHeight(height);

		ScheduleRefresh();
	}

	// StatusBar
	void StatusBar::SetPaneWidth(size_t index, int width)
	{
		size_t count = GetFieldsCount();
		if (index < count)
		{
			std::vector<int> widths(count, 0);
			for (size_t i = 0; i < count; i++)
			{
				widths[i] = GetStatusWidth(i);
			}
			widths[index] = width;

			wxStatusBar::SetStatusWidths(static_cast<int>(count), widths.data());
		}
	}
}
