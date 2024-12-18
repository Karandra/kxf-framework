#include "kxf-pch.h"
#include "Frame.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Frame, wxFrame);

	bool Frame::Create(wxWindow* parent,
					   wxWindowID id,
					   const String& title,
					   const Point& pos,
					   const Size& size,
					   FlagSet<FrameStyle> style
	)
	{
		if (wxFrame::Create(parent, id, title, pos, size, style.ToInt()))
		{
			SetStatusBarPane(-1);
			SetDefaultBackgroundColor();
			Center(wxBOTH);

			return true;
		}
		return false;
	}
}
