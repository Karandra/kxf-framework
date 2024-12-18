#pragma once
#include "kxf/Widgets/Common.h"
#include <wx/srchctrl.h>

namespace kxf::UI
{
	class KXF_API SearchBox: public wxSearchCtrl
	{
		public:
			static constexpr long DefaultStyle = wxTE_LEFT|wxTE_PROCESS_ENTER|wxTE_NOHIDESEL;

		private:
			wxEvtHandler m_EvtHandler;
			String m_Value;

		private:
			void OnText(wxCommandEvent& event);
			void OnEnter(wxCommandEvent& event);
			void OnKillFocus(wxFocusEvent& event);
			void OnCancelSearch(wxCommandEvent& event);

		public:
			SearchBox() = default;
			SearchBox(wxWindow* parent,
					  wxWindowID id,
					  const String& value = {},
					  long style = DefaultStyle,
					  const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, value, style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& value = {},
						long style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			);
			~SearchBox();

		public:
			wxDECLARE_DYNAMIC_CLASS(SearchBox);
	};
}
