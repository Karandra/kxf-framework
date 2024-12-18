#pragma once
#include "Common.h"
#include "kxf/Core/StdID.h"
#include <wx/stockitem.h>

namespace kxf::wxWidgets
{
	String LocalizeLabelString(StdID id, FlagSet<wxStockLabelQueryFlag> flags = wxStockLabelQueryFlag::wxSTOCK_NOFLAGS);
	String LocalizeHelpString(StdID id, wxStockHelpStringClient client);
}
