#include "kxf-pch.h"
#include "RadioCheck.h"
#include "../../Events/WidgetCheckEvent.h"

namespace kxf::WXUI
{
	bool RadioCheck::DoTryBefore(wxEvent& event)
	{
		const auto eventType = event.GetEventType();
		if (eventType == wxEVT_RADIOBUTTON)
		{
			auto& eventWX = static_cast<wxCommandEvent&>(event);
			return m_Widget.ProcessEvent(WidgetCheckEvent::EvtStateChanged, m_Widget, eventWX.IsChecked());
		}
		return false;
	}

	bool RadioCheck::Create(wxWindow* parent,
							const String& label,
							const Point& pos,
							const Size& size
	)
	{
		if (wxRadioButton::Create(parent, wxID_NONE, label, pos, size, wxRB_SINGLE))
		{
			return true;
		}
		return false;
	}
}
