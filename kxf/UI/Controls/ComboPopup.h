#pragma once
#include "kxf/UI/Common.h"
#include <wx/combo.h>

namespace kxf::UI
{
	class ComboControl;
}

namespace kxf::UI
{
	class KXF_API ComboPopup: public wxComboPopup
	{
		friend class ComboControl;

		protected:
			virtual void OnDoShowPopup()
			{
			}
	};
}
