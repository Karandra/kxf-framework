#include "kxf-pch.h"
#include "ComboBox.h"
#include "../../Events/ComboBoxWidgetEvent.h"
#include <CommCtrl.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::WXUI
{
	bool ComboBox::DoTryBefore(wxEvent& event)
	{
		const auto eventType = event.GetEventType();
		if (eventType == wxEVT_COMBOBOX)
		{
			auto& eventWx = static_cast<wxCommandEvent&>(event);
			auto index = eventWx.GetSelection();

			return m_Widget.ProcessEvent(ComboBoxWidgetEvent::EvtItemSelected, m_Widget, index >= 0 ? index : IComboBoxWidget::npos);
		}
		else if (eventType == wxEVT_COMBOBOX_DROPDOWN)
		{
			return m_Widget.ProcessEvent(ComboBoxWidgetEvent::EvtDropdownShow, m_Widget);
		}
		else if (eventType == wxEVT_COMBOBOX_CLOSEUP)
		{
			return m_Widget.ProcessEvent(ComboBoxWidgetEvent::EvtDropdownDismiss, m_Widget);
		}
		return TranslateTextEvent(m_Widget, event);
	}

	bool ComboBox::Create(wxWindow* parent,
						const String& label,
						const Point& pos,
						const Size& size
	)
	{
		if (wxComboBox::Create(parent, wxID_NONE, {}, pos, size, 0, nullptr, wxCB_DROPDOWN|wxCB_READONLY|wxTE_PROCESS_ENTER, wxDefaultValidator))
		{
			EnableSystemTheme();
			return true;
		}
		return false;
	}

	bool ComboBox::SetVisibleItemCount(size_t count)
	{
		return ::SendMessageW(GetHandle(), CB_SETMINVISIBLE, static_cast<WPARAM>(count), 0) != 0;
	}
}
