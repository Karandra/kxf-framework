#include "kxf-pch.h"
#include "SearchBox.h"
#include "../../Events/SearchWidgetEvent.h"

namespace kxf::WXUI
{
	void SearchBox::OnText(wxCommandEvent& event)
	{
		if (HasFocus())
		{
			m_Value = GetValue();
		}
		else
		{
			ChangeValue(m_Value);
		}
		event.Skip();
	}
	void SearchBox::OnEnter(wxCommandEvent& event)
	{
		m_Value = GetValue();
		m_Widget.ProcessEvent(SearchWidgetEvent::EvtSearch, m_Widget, m_Value);

		event.Skip();
	}
	void SearchBox::OnKillFocus(wxFocusEvent& event)
	{
		ChangeValue(m_Value);
		event.Skip();
	}
	void SearchBox::OnCancelSearch(wxCommandEvent& event)
	{
		m_Value.clear();
		ChangeValue({});

		event.Skip();
	}

	bool SearchBox::DoTryBefore(wxEvent& event)
	{
		const auto eventType = event.GetEventType();
		if (eventType == wxEVT_SEARCH)
		{
			auto& eventWX = static_cast<wxCommandEvent&>(event);
			return m_Widget.ProcessEvent(SearchWidgetEvent::EvtSearch, m_Widget, GetValue());
		}
		else if (eventType == wxEVT_SEARCH_CANCEL)
		{
			auto& eventWX = static_cast<wxCommandEvent&>(event);
			return m_Widget.ProcessEvent(SearchWidgetEvent::EvtCancel, m_Widget);
		}
		return TranslateTextEvent(m_Widget, event);
	}
	WXLRESULT SearchBox::MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		// Ctrl + A (A is 1)
		if (msg == WM_CHAR && wParam == 1)
		{
			::SendMessageW(GetHandle(), EM_SETSEL, 0, -1);
			return 1;
		}
		return wxSearchCtrl::MSWWindowProc(msg, wParam, lParam);
	}

	bool SearchBox::Create(wxWindow* parent,
						   const String& label,
						   const Point& pos,
						   const Size& size
	)
	{
		if (wxSearchCtrl::Create(parent, wxID_NONE, label, pos, size, wxTE_LEFT|wxTE_PROCESS_ENTER, wxDefaultValidator))
		{
			m_EvtHandler.Bind(wxEVT_TEXT, &SearchBox::OnText, this);
			m_EvtHandler.Bind(wxEVT_TEXT_ENTER, &SearchBox::OnEnter, this);
			m_EvtHandler.Bind(wxEVT_KILL_FOCUS, &SearchBox::OnKillFocus, this);
			m_EvtHandler.Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, &SearchBox::OnCancelSearch, this);

			PushEventHandler(&m_EvtHandler);
			m_EvtHandler.SetClientData(this);
			return true;
		}
		return false;
	}
}
