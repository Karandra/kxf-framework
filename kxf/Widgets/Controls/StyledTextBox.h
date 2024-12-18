#pragma once
#include "kxf/Widgets/Common.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include <wx/stc/stc.h>

namespace kxf::UI
{
	class KXF_API StyledTextBox: public wxStyledTextCtrl
	{
		public:
			static constexpr long DefaultStyle = 0;

		private:
			bool m_IsCreated = false;
			bool m_IsMultiline = true;
		
		public:
			StyledTextBox() = default;
			StyledTextBox(wxWindow* parent,
						  wxWindowID id,
						  long style = DefaultStyle,
						  const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						long style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			);
	
		public:
			void SetModified(bool modified = true)
			{
				modified ? wxStyledTextCtrl::MarkDirty() : wxStyledTextCtrl::SetSavePoint();
			}
		
			bool IsMultiline() const
			{
				return m_IsMultiline;
			}
			void SetMultiline(bool value)
			{
				m_IsMultiline = value;
			}

			bool SetBackgroundColour(const wxColour& color) override;
			bool SetForegroundColour(const wxColour& color) override;

		public:
			wxDECLARE_DYNAMIC_CLASS(StyledTextBox);
	};
}
