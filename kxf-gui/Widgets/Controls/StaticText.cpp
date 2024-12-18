#include "kxf-pch.h"
#include "StaticText.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(StaticText, wxStaticText);

	void StaticText::OnEnter(wxMouseEvent& event)
	{
		SetForegroundColour(m_ColorHighlight);
		Refresh();
		event.Skip();
	}
	void StaticText::OnLeave(wxMouseEvent& event)
	{
		SetForegroundColour(m_ColorNormal);
		Refresh();
		event.Skip();
	}
	void StaticText::OnMouseDown(wxMouseEvent& event)
	{
		SetForegroundColour(m_ColorClick);
		Refresh();
		event.Skip();
	}

	bool StaticText::Create(wxWindow* parent,
							wxWindowID id,
							const String& label,
							FlagSet<StaticTextStyle> style
	)
	{
		if (wxStaticText::Create(parent, id, label, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt()))
		{
			m_ColorNormal = GetForegroundColour();
			m_ColorHighlight = m_ColorNormal;
			m_ColorClick = m_ColorNormal;

			Bind(wxEVT_ENTER_WINDOW, &StaticText::OnEnter, this);
			Bind(wxEVT_LEAVE_WINDOW, &StaticText::OnLeave, this);
			Bind(wxEVT_LEFT_UP, &StaticText::OnLeave, this);
			Bind(wxEVT_LEFT_DOWN, &StaticText::OnMouseDown, this);
			return true;
		}
		return false;
	}
}
