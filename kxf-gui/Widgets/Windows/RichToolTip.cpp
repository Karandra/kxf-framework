#include "kxf-pch.h"
#include "RichToolTip.h"
#include "kxf/wxWidgets/MapConstants.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(RichToolTip, wxObject);

	void RichToolTip::SetIcon(StdIcon iconID)
	{
		m_Icon = wxNullIcon;
		m_IconID = iconID;
		m_ToolTip.SetIcon(*wxWidgets::MapStdIcon(iconID));
	}
}
