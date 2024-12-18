#include "kxf-pch.h"
#include "Frame.h"

namespace kxf::WXUI
{
	bool Frame::Create(wxWindow* parent,
					   const String& title,
					   const Point& pos,
					   const Size& size
	)
	{
		if (wxFrame::Create(parent, wxID_NONE, title, pos, size, wxDEFAULT_FRAME_STYLE|wxFRAME_SHAPED))
		{
			SetInitialSize(size);
			SetStatusBarPane(-1);

			if (!pos.IsFullySpecified())
			{
				if (parent)
				{
					CenterOnParent(wxBOTH);
				}
				else
				{
					CenterOnScreen(wxBOTH);
				}
			}
			return true;
		}
		return false;
	}
}
