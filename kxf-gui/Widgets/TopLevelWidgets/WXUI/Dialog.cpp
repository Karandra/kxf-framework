#include "kxf-pch.h"
#include "Dialog.h"

namespace kxf::WXUI
{
	bool Dialog::Create(wxWindow* parent,
						const String& title,
						const Point& pos,
						const Size& size
	)
	{
		FlagSet<int> style = wxDEFAULT_DIALOG_STYLE|wxFRAME_SHAPED;
		style.Add(wxDIALOG_NO_PARENT, parent == nullptr);

		if (wxDialog::Create(parent, wxID_NONE, title, pos, size, *style))
		{
			SetInitialSize(size);
			SetLayoutAdaptationLevel(wxDIALOG_ADAPTATION_NONE);
			SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_DISABLED);

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
