#include "kxf-pch.h"
#include "DataViewComboBox.h"
#include "ComboControl.h"
#include "kxf/Widgets/Windows/Panel.h"
#include "kxf/Widgets/Controls//DataView/MainWindow.h"

namespace kxf::UI::DataView
{
	wxIMPLEMENT_ABSTRACT_CLASS(ComboCtrl, View);

	int ComboCtrl::CalculateItemsHeight() const
	{
		int height = 0;
		if (m_MaxVisibleItems != -1)
		{
			if (const auto header = GetHeaderCtrl())
			{
				height += header->GetSize().GetHeight() + FromDIP(4);
			}

			int items = std::min<int>(m_MaxVisibleItems, GetMainWindow()->GetRowCount());
			height += (items * FromDIP(4)) + (items * GetUniformRowHeight());
		}
		else
		{
			height = GetUniformRowHeight();
		}
		return std::clamp(height, 0, wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
	}
	void ComboCtrl::UpdatePopupHeight()
	{
		if (m_MaxVisibleItems == -1)
		{
			m_ComboCtrl->SetPopupMaxHeight(-1);
		}
		else
		{
			int height = CalculateItemsHeight();

			// Setting max height to zero doesn't seems to work
			m_ComboCtrl->SetPopupMaxHeight(height == 0 ? 1 : height);
		}
	}

	void ComboCtrl::OnInternalIdle()
	{
		View::OnInternalIdle();
	}
	void ComboCtrl::OnSelectItem(ItemEvent& event)
	{
		event.Skip();

		m_Selection = event.GetNode();
		if (ContainsOption(ComboCtrlOption::DismissOnSelect))
		{
			m_ComboCtrl->Dismiss();
		}
	}
	void ComboCtrl::OnScroll(wxMouseEvent& event)
	{
		event.Skip();

		int rateX = 0;
		int rateY = 0;
		GetScrollPixelsPerUnit(&rateX, &rateY);
		Point startPos = Point(GetViewStart());

		wxCoord value = -event.GetWheelRotation();
		if (event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
		{
			Scroll(wxDefaultCoord, startPos.GetY() + static_cast<float>(value) / (rateY != 0 ? rateY : 1));
		}
		else
		{
			Scroll(startPos.GetX() + static_cast<float>(value) / (rateX != 0 ? rateX : 1), wxDefaultCoord);
		}
	}

	bool ComboCtrl::FindItem(const wxString& value, wxString* trueItem)
	{
		return true;
	}
	void ComboCtrl::OnPopup()
	{
		SetFocus();
	}
	void ComboCtrl::OnDismiss()
	{
		if (m_Selection)
		{
			m_Selection->SelectItem();
		}

		if (ContainsOption(ComboCtrlOption::ForceGetStringOnDismiss))
		{
			m_ComboCtrl->SetText(GetStringValue());
		}
	}
	void ComboCtrl::OnDoShowPopup()
	{
		UpdatePopupHeight();
	}
	bool ComboCtrl::Create(wxWindow* window)
	{
		m_Sizer = new wxBoxSizer(ContainsOption(ComboCtrlOption::HorizontalLayout) ? wxHORIZONTAL : wxVERTICAL);
		m_BackgroundWindow = new kxf::UI::Panel(window, wxID_NONE, WindowStyle::None|WindowBorder::Theme|*Panel::DefaultStyle);
		m_BackgroundWindow->SetSizer(m_Sizer);
		if (ShouldInheritColours())
		{
			m_BackgroundWindow->SetBackgroundColour(m_ComboCtrl->GetBackgroundColour());
			m_BackgroundWindow->SetForegroundColour(m_ComboCtrl->GetForegroundColour());
		}

		if (View::Create(m_BackgroundWindow, wxID_NONE, m_DataViewFlags))
		{
			View::SetPosition(Point(0, 0));
			m_Sizer->Add(this, 1, wxEXPAND);

			// DataView events
			m_EvtHandler_DataView.Bind(ItemEvent::EvtItemSelected.ToWxTag(), &ComboCtrl::OnSelectItem, this);
			PushEventHandler(&m_EvtHandler_DataView);

			// ComboCtrl Events
			m_ComboCtrl->PushEventHandler(&m_EvtHandler_ComboCtrl);
			if (!ContainsOption(ComboCtrlOption::AltPopupWindow))
			{
				m_EvtHandler_ComboCtrl.Bind(wxEVT_MOUSEWHEEL, &ComboCtrl::OnScroll, this);
			}
			return true;
		}
		return false;
	}

	bool ComboCtrl::Create(wxWindow* parent,
						   wxWindowID id,
						   FlagSet<ComboBoxStyle>  style,
						   const wxValidator& validator
	)
	{
		m_ComboCtrl = new ComboControl();
		m_ComboCtrl->UseAltPopupWindow(ContainsOption(ComboCtrlOption::AltPopupWindow));

		if (m_ComboCtrl->Create(parent, id, {}, style, validator))
		{
			m_ComboCtrl->SetFocusDrawMode(ComboControl::DrawFocus::Never);
			m_ComboCtrl->SetPopupControl(this);

			return true;
		}
		return false;
	}
	ComboCtrl::~ComboCtrl()
	{
		PopEventHandler();
		m_ComboCtrl->PopEventHandler();
	}

	wxWindow* ComboCtrl::GetControl()
	{
		return m_BackgroundWindow;
	}
	wxComboCtrl* ComboCtrl::GetComboControl()
	{
		return m_ComboCtrl;
	}
	wxWindow* ComboCtrl::ComboGetBackgroundWindow()
	{
		return m_BackgroundWindow;
	}

	wxString ComboCtrl::GetStringValue() const
	{
		ItemEvent event(EvtGetStringValue, GetId());
		event.SetEventObject(const_cast<ComboCtrl*>(this));
		event.SetNode(m_Selection);

		HandleWindowEvent(event);
		return event.GetString();
	}
	void ComboCtrl::SetStringValue(const wxString& value)
	{
		ItemEvent event(EvtSetStringValue, GetId());
		event.SetEventObject(const_cast<ComboCtrl*>(this));
		event.SetString(value);

		HandleWindowEvent(event);
		if (Node* node = event.GetNode())
		{
			node->SelectItem();
		}
	}

	void ComboCtrl::ComboPopup()
	{
		m_ComboCtrl->Popup();
	}
	void ComboCtrl::ComboDismiss()
	{
		m_ComboCtrl->Dismiss();
	}
	void ComboCtrl::ComboRefreshLabel()
	{
		m_ComboCtrl->SetText(GetStringValue());
	}

	int ComboCtrl::ComboGetMaxVisibleItems() const
	{
		return m_MaxVisibleItems;
	}
	void ComboCtrl::ComboSetMaxVisibleItems(int count)
	{
		m_MaxVisibleItems = count;
	}

	void ComboCtrl::ComboSetPopupExtents(int nLeft, int nRight)
	{
		m_ComboCtrl->SetPopupExtents(nLeft, nRight);
	}
	void ComboCtrl::ComboSetPopupMinWidth(int width)
	{
		m_ComboCtrl->SetPopupMinWidth(width);
	}
	void ComboCtrl::ComboSetPopupAnchor(wxDirection nSide)
	{
		m_ComboCtrl->SetPopupAnchor(nSide);
	}
}
