#include "kxf-pch.h"
#include "SearchBox.h"
#include "kxf/Application/ICoreApplication.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(SearchBox, wxSearchCtrl);

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
	}
	void SearchBox::OnEnter(wxCommandEvent& event)
	{
		m_Value = GetValue();

		wxCommandEvent searchEvent(event);
		searchEvent.SetEventType(wxEVT_SEARCHCTRL_SEARCH_BTN);
		searchEvent.SetString(m_Value);
		ProcessWindowEvent(searchEvent);
	}
	void SearchBox::OnKillFocus(wxFocusEvent& event)
	{
		ChangeValue(m_Value);
		event.Skip();
	}
	void SearchBox::OnCancelSearch(wxCommandEvent& event)
	{
		m_Value.clear();
		ChangeValue(m_Value);

		event.Skip();
	}

	bool SearchBox::Create(wxWindow* parent,
						   wxWindowID id,
						   const String& value,
						   long style,
						   const wxValidator& validator
	)
	{
		m_Value = value;
		PushEventHandler(&m_EvtHandler);

		if (wxSearchCtrl::Create(parent, id, value, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style, validator))
		{
			ShowCancelButton(true);
			ShowSearchButton(true);
			if (auto app = ICoreApplication::GetInstance())
			{
				SetDescriptiveText(app->GetLocalizationPackage().GetItem(StdID::Search).GetString());
			}

			Refresh();

			m_EvtHandler.Bind(wxEVT_TEXT, &SearchBox::OnText, this);
			m_EvtHandler.Bind(wxEVT_TEXT_ENTER, &SearchBox::OnEnter, this);
			m_EvtHandler.Bind(wxEVT_KILL_FOCUS, &SearchBox::OnKillFocus, this);
			m_EvtHandler.Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, &SearchBox::OnCancelSearch, this);
			return false;
		}
		return true;
	}
	SearchBox::~SearchBox()
	{
		PopEventHandler();
	}
}
