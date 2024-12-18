#include "kxf-pch.h"
#include "Check.h"
#include "../../Events/WidgetCheckEvent.h"

namespace kxf::WXUI
{
	bool Check::DoTryBefore(wxEvent& event)
	{
		const auto eventType = event.GetEventType();
		if (eventType == wxEVT_CHECKBOX)
		{
			auto& eventWx = static_cast<wxCommandEvent&>(event);
			switch (static_cast<wxCheckBoxState>(eventWx.GetInt()))
			{
				case wxCheckBoxState::wxCHK_CHECKED:
				{
					return m_Widget.ProcessEvent(WidgetCheckEvent::EvtStateChanged, m_Widget, CheckWidgetValue::Checked);
				}
				case wxCheckBoxState::wxCHK_UNCHECKED:
				{
					return m_Widget.ProcessEvent(WidgetCheckEvent::EvtStateChanged, m_Widget, CheckWidgetValue::Unchecked);
				}
				case wxCheckBoxState::wxCHK_UNDETERMINED:
				{
					return m_Widget.ProcessEvent(WidgetCheckEvent::EvtStateChanged, m_Widget, CheckWidgetValue::Indeterminate);
				}
			};
		}
		return false;
	}

	bool Check::Create(wxWindow* parent,
					   const String& label,
					   const Point& pos,
					   const Size& size
	)
	{
		if (wxCheckBox::Create(parent, wxID_NONE, label, pos, size, wxCHK_3STATE))
		{
			return true;
		}
		return false;
	}
}
