#pragma once
#include "kxf/UI/Common.h"
#include <wx/radiobut.h>

namespace kxf::UI
{
	class KXF_API RadioButton: public wxRadioButton
	{
		public:
			static constexpr long DefaultStyle = 0;

		public:
			RadioButton() = default;
			RadioButton(wxWindow* parent,
						wxWindowID id,
						const wxString& label,
						long style = DefaultStyle
			)
			{
				Create(parent, id, label, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const wxString& label,
						long style = DefaultStyle
			);

		public:
			wxDECLARE_DYNAMIC_CLASS(RadioButton);
	};
}
