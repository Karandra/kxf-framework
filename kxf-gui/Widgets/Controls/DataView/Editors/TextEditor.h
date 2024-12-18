#pragma once
#include "../Editor.h"

namespace kxf::UI::DataView
{
	class KXF_API TextEditor: public Editor
	{
		protected:
			wxWindow* CreateControl(wxWindow& parent, const Rect& cellRect, Any value) override;
			Any GetValue(wxWindow& control) const override;
	};
}
