#include "kxf-pch.h"
#include "StatusBar.h"

namespace
{
	constexpr int g_DefaultMinHeight = 23;
}

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(StatusBar, wxStatusBar);

	void StatusBar::SetFieldsCount(int count, const int* widths)
	{
		if (!widths)
		{
			if (count = GetFieldsCount(); count > 0)
			{
				std::vector<int> newWidths;
				newWidths.resize(count);
				for (size_t i = 0; i < static_cast<size_t>(count); i++)
				{
					newWidths[i] = GetStatusWidth(i);
				}

				wxStatusBar::SetStatusWidths(count, newWidths.data());
				return;
			}
		}
		wxStatusBar::SetFieldsCount(count, widths);
	}

	bool StatusBar::Create(wxWindow* parent,
						   wxWindowID id,
						   int fieldCount,
						   FlagSet<StatusBarStyle> style
	)
	{
		if (wxStatusBar::Create(parent, id, style.ToInt()))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);
			SetMinHeight(FromDIP(g_DefaultMinHeight));

			return true;
		}
		return false;
	}

	void StatusBar::SetStatusWidth(int width, int index)
	{
		int count = GetFieldsCount();
		if (count > 0 && index < count)
		{
			std::vector<int> widths(count, 0);
			for (int i = 0; i < count; i++)
			{
				widths[i] = GetStatusWidth(i);
			}
			widths[index] = width;

			wxStatusBar::SetStatusWidths(count, widths.data());
		}
	}
}
