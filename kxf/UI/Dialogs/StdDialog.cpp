#include "KxfPCH.h"
#include "StdDialog.h"
#include "kxf/System/SystemWindow.h" 
#include "kxf/Drawing/ArtProvider.h"
#include "kxf/Drawing/GDIRenderer/UxTheme.h"
#include "kxf/Utility/Common.h"

namespace
{
	int FromDIPX(const wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(value, wxDefaultCoord)).GetWidth();
	}
	int FromDIPY(const wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(wxDefaultCoord, value)).GetHeight();
	}
}

namespace kxf::UI
{
	bool IStdDialog::ShowNativeWindow(wxDialog* window, bool show)
	{
		HWND hWnd = window->GetHandle();
		bool ret = false;
		if (show)
		{
			if (hWnd)
			{
				ret = ::ShowWindow(hWnd, SW_SHOW);
			}
			else
			{
				window->ShowModal();
				ret = true;
			}
		}
		else
		{
			ret = ::ShowWindow(hWnd, SW_HIDE);
		}
		return ret;
	}
	int IStdDialog::TranslateIconIDToWx(StdIcon id) const
	{
		switch (id)
		{
			case StdIcon::Question:
			{
				return wxICON_QUESTION;
			}
			case StdIcon::Error:
			{
				return wxICON_ERROR;
			}
			case StdIcon::Warning:
			{
				return wxICON_WARNING;
			}
			case StdIcon::Information:
			{
				return wxICON_INFORMATION;
			}
		};
		return wxICON_NONE;
	}
	StdIcon IStdDialog::TranslateIconIDFromWx(int id) const
	{
		switch (id)
		{
			case wxICON_QUESTION:
			{
				return StdIcon::Question;
			}
			case wxICON_ERROR:
			{
				return StdIcon::Error;
			}
			case wxICON_WARNING:
			{
				return StdIcon::Warning;
			}
			case wxICON_INFORMATION:
			{
				return StdIcon::Information;
			}
		};
		return StdIcon::None;
	}
}

namespace kxf::UI
{
	StdDialogControl::StdDialogControl(Button* control)
		:m_Type(Type::Button)
	{
		m_Data.Control = control;
	}

	StdDialogControl::operator wxWindowID() const
	{
		if (m_Type != Type::ID)
		{
			return m_Data.Control->GetId();
		}
		else
		{
			return m_Data.ID;
		}
	}
	StdDialogControl::operator wxWindow* () const
	{
		if (m_Type == Type::Window)
		{
			return m_Data.Control;
		}
		return nullptr;
	}
	StdDialogControl::operator Button* () const
	{
		if (m_Type == Type::Button)
		{
			return static_cast<Button*>(m_Data.Control);
		}
		return nullptr;
	}
}

namespace kxf::UI
{
	wxIMPLEMENT_ABSTRACT_CLASS(StdDialog, wxDialog);

	const StdDialog::StdButtonsIDs StdDialog::ms_DefaultCloseIDs = {wxID_OK, wxID_YES, wxID_NO, wxID_CANCEL, wxID_APPLY, wxID_CLOSE};
	const StdDialog::StdButtonsIDs StdDialog::ms_DefaultEnterIDs = {wxID_OK, wxID_YES, wxID_APPLY};
	const Color StdDialog::ms_WindowBackgroundColor = wxSystemSettings::GetColour(wxSYS_COLOUR_MENU);
	const Color StdDialog::ms_LineBackgroundColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);

	wxWindowID StdDialog::TranslateButtonConstantsToIDs(int btnValue)
	{
		switch (FromInt<StdButton>(btnValue))
		{
			case StdButton::OK:
			{
				return wxID_OK;
			}
			case StdButton::Yes:
			{
				return wxID_YES;
			}
			case StdButton::No:
			{
				return wxID_NO;
			}
			case StdButton::Cancel:
			{
				return wxID_CANCEL;
			}
			case StdButton::Apply:
			{
				return wxID_APPLY;
			}
			case StdButton::Close:
			{
				return wxID_CLOSE;
			}
			case StdButton::Help:
			{
				return wxID_HELP;
			}
			case StdButton::Forward:
			{
				return wxID_FORWARD;
			}
			case StdButton::Backward:
			{
				return wxID_BACKWARD;
			}
			case StdButton::Reset:
			{
				return wxID_RESET;
			}
			case StdButton::More:
			{
				return wxID_MORE;
			}
			case StdButton::Setup:
			{
				return wxID_SETUP;
			}
			case StdButton::Retry:
			{
				return wxID_RETRY;
			}
		};
		return wxID_NONE;
	}
	bool StdDialog::MSWTranslateMessage(WXMSG* msg)
	{
		if (false && msg->message == WM_SYSKEYDOWN && msg->wParam == VK_F4 && ::GetKeyState(VK_MENU) < 0)
		{
			//wxMessageBox("MSWTranslateMessage: Alt+F4");
			return true; //Do nothing, we are processing, not the System
		}
		return Dialog::MSWTranslateMessage(msg);
	}

	void StdDialog::EnableGlassFrame()
	{
		SetGripperVisible(!m_IsGlassFrameEnabled);
		if (m_IsGlassFrameEnabled)
		{
			wxColour color = DWMGetColorKey();
			SetOwnBackgroundColour(color);
			m_ContentPanelLine->SetOwnBackgroundColour(color);
			DWMExtendFrame(GetGlassRect(), color);
			//GetContentWindow()->Bind(wxEVT_PAINT, &KxStdDialog::OnDrawFrameBorder, this);
		}
		else
		{
			SetOwnBackgroundColour(ms_WindowBackgroundColor);
			m_ContentPanelLine->SetOwnBackgroundColour(ms_LineBackgroundColor);
			DWMExtendFrame();
			//GetContentWindow()->Unbind(wxEVT_PAINT, &KxStdDialog::OnDrawFrameBorder, this);
		}
	}
	void* StdDialog::GetGripperWindow()
	{
		return ::FindWindowExW(this->GetHandle(), nullptr, L"ScrollBar", L"");
	}
	void StdDialog::OnDrawFrameBorder(wxPaintEvent& event)
	{
		using namespace kxf;

		wxAutoBufferedPaintDC dc(GetContentWindow());
		dc.Clear();
		dc.SetPen(DWMGetGlassColor().ChangeLightness(Angle::FromNormalized(0.6f)));
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetBackground(*wxTRANSPARENT_BRUSH);

		dc.DrawRectangle(GetContentWindow()->GetRect());
		event.Skip();
	}

	void StdDialog::OnStdButtonClick(wxCommandEvent& event)
	{
		const wxWindowID oldReturnCode = GetReturnCode();
		const wxWindowID buttonID = event.GetId();

		for (WidgetID closeID : m_CloseIDs)
		{
			if (closeID == buttonID)
			{
				const wxWindow* button = GetButton(buttonID).GetControl();

				wxNotifyEvent onEscapeEvent(EvtButton->AsInt(), buttonID);
				onEscapeEvent.SetEventObject(this);
				ProcessEvent(onEscapeEvent);
				if (onEscapeEvent.IsAllowed() && (button == nullptr || button->IsThisEnabled()))
				{
					SetReturnCode(buttonID);
					EndModal(buttonID);
					Close();
				}
				else
				{
					SetReturnCode(oldReturnCode);
				}
				break;
			}
		}
		event.Skip(false);
	}
	void StdDialog::OnClose(wxCloseEvent& event)
	{
		if (!event.GetVeto())
		{
			if (IsModal())
			{
				EndModal(GetReturnCode());
			}
			Destroy();
		}
	}
	void StdDialog::OnEscape(wxKeyEvent& event)
	{
		event.Skip();
		WidgetID id;
		if (event.GetKeyCode() == WXK_ESCAPE && IsEscapeAllowed())
		{
			wxNotifyEvent onEscapeEvent(EvtButton->AsInt(), wxID_CANCEL);
			onEscapeEvent.SetEventObject(this);
			ProcessEvent(onEscapeEvent);

			EndModal(wxID_CANCEL);
			Close();
		}
		else if (event.GetKeyCode() == WXK_RETURN && IsEnterAllowed(event, &id))
		{
			if (id != wxID_NONE)
			{
				wxCommandEvent buttonEvent(EvtButton->AsInt(), id.ToInt());
				OnStdButtonClick(buttonEvent);
				event.Skip(false);
			}
		}
		else
		{
			if (IsBellAllowed())
			{
				if (!GetDialogMainCtrl()->HasFocus() && !event.HasAnyModifiers() && event.GetKeyCode() != WXK_TAB && !IsUserWindowHasFocus())
				{
					wxBell();
				}
			}
		}
	}

	bool StdDialog::Create(wxWindow* parent,
						   wxWindowID id,
						   const String& caption,
						   const Point& pos,
						   const Size& size,
						   FlagSet<StdButton> buttons,
						   FlagSet<DialogStyle> style
	)
	{
		m_SelectedButtons = buttons;
		style.Remove(static_cast<DialogStyle>(TopLevelWindowStyle::CloseBox), !IsCloseBoxEnabled());

		if (Dialog::Create(parent, id, GetDefaultTitle(), pos, size, style))
		{
			// Default interface
			m_GripperWindow.Create(this, wxID_NONE, reinterpret_cast<wxNativeWindowHandle>(GetGripperWindow()));
			m_GripperWindowSize = Size(m_GripperWindow.GetSize());
			SetBackgroundColour(ms_WindowBackgroundColor);

			m_ContentPanel = new UI::Panel(this, wxID_NONE);
			m_ContentPanel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

			m_CaptionLabel = new Label(m_ContentPanel, wxID_NONE, caption, LabelStyle::Caption); // wxLLabel::DefaultStyle|KxLABEL_CAPTION
			m_CaptionLabel->SetMaxSize(FromDIP(Size(wxDefaultCoord, 23)));
			m_CaptionLabel->SetForegroundColour(UxTheme::GetDialogMainInstructionColor(*m_ContentPanel));

			m_ViewLabel = new Label(m_ContentPanel, wxID_NONE, {}, LabelStyle::None);
			m_ViewLabel->SetMaxSize(FromDIP(Size(wxDefaultCoord, 23)));

			m_ContentPanelLine = new UI::Panel(this, wxID_NONE, WindowStyle::None|WindowBorder::None);
			m_ContentPanelLine->SetPosition(Point(0, 0));
			m_ContentPanelLine->SetBackgroundColour(ms_LineBackgroundColor);
			m_ContentPanelLine->SetMaxSize(FromDIP(Size(wxDefaultCoord, 1)));

			m_IconView = new wxStaticBitmap(m_ContentPanel, wxID_NONE, m_MainIcon.ToWxBitmap());
			if (m_MainIcon)
			{
				m_IconView->SetMinSize(m_MainIcon.GetSize());
			}
			else
			{
				m_IconView->SetSize(m_MainIcon.GetSize());
			}

			// Default sizers
			m_DialogSizer = new wxBoxSizer(wxVERTICAL);
			SetSizer(m_DialogSizer);

			m_ContentSizerBase = new wxBoxSizer(wxHORIZONTAL);

			m_IconSizer = new wxBoxSizer(wxVERTICAL);
			m_ContentSizer = new wxBoxSizer(wxVERTICAL);
			m_ButtonsSizer = CreateStdDialogButtonSizer(m_SelectedButtons);

			// Events
			Bind(wxEVT_BUTTON, &StdDialog::OnStdButtonClick, this);
			Bind(wxEVT_CLOSE_WINDOW, &StdDialog::OnClose, this);
			Bind(wxEVT_CHAR_HOOK, &StdDialog::OnEscape, this);

			SetEscapeID(wxID_NONE);
			SetAffirmativeID(wxID_AUTO_LOWEST - 1);
			SetReturnCode(wxID_CANCEL);
			return true;
		}
		return false;
	}

	wxOrientation StdDialog::GetViewSizerOrientation() const
	{
		return wxVERTICAL;
	}
	wxOrientation StdDialog::GetViewLabelSizerOrientation() const
	{
		return wxVERTICAL;
	}
	bool StdDialog::IsEscapeAllowed(WidgetID* idOut) const
	{
		if (m_SelectedButtons & StdButton::OK || m_SelectedButtons & StdButton::Cancel || m_SelectedButtons & StdButton::No || m_SelectedButtons & StdButton::Close)
		{
			for (WidgetID id: m_CloseIDs)
			{
				if (id == wxID_CANCEL || id == wxID_NO || id == wxID_CLOSE)
				{
					Utility::SetIfNotNull(idOut, id);
					return true;
				}
			}
		}

		Utility::SetIfNotNull(idOut, wxID_NONE);
		return false;
	}
	bool StdDialog::IsEnterAllowed(wxKeyEvent& event, WidgetID* idOut) const
	{
		auto Check = [this](StdButton btn, WidgetID id, WidgetID enterId)
		{
			return m_SelectedButtons & btn && enterId == id;
		};

		if (m_SelectedButtons & StdButton::OK || m_SelectedButtons & StdButton::Yes || m_SelectedButtons & StdButton::Apply)
		{
			for (WidgetID id: m_EnterIDs)
			{
				if (Check(StdButton::OK, wxID_OK, id) || Check(StdButton::Yes, wxID_YES, id) || Check(StdButton::Apply, wxID_APPLY, id))
				{
					Utility::SetIfNotNull(idOut, id);
					return true;
				}
			}
		}

		Utility::SetIfNotNull(idOut, wxID_NONE);
		return false;
	}
	void StdDialog::SetResizingBehavior()
	{
		Size size = Size(GetMinSize());
		auto resizeSide = GetWindowResizeSide();
		switch (ToInt(resizeSide))
		{
			case wxBOTH:
			{
				SetMaxSize(Size::UnspecifiedSize());
				break;
			}
			case wxHORIZONTAL:
			{
				SetMaxSize(Size(wxDefaultCoord, size.GetHeight()));
				break;
			}
			case wxVERTICAL:
			{
				SetMaxSize(Size(size.GetWidth(), wxDefaultCoord));
				break;
			}
			case 0:
			{
				SetWindowStyle(GetWindowStyle() & ~wxRESIZE_BORDER);
				break;
			}
		};

		SystemWindow(GetHandle()).ModWindowStyle(GWL_STYLE, WS_MAXIMIZEBOX, resizeSide == wxBOTH);
	}
	void StdDialog::PostCreate(const Point& pos)
	{
		m_DialogSizer->Add(m_ContentPanel, 1, wxEXPAND);
		m_DialogSizer->Add(m_ContentPanelLine, 0, wxEXPAND);

		if (m_ContentSizerBase)
		{
			m_ContentPanel->SetSizer(m_ContentSizerBase);
			m_ContentSizerBase->Add(m_IconSizer, 0, wxEXPAND, FromDIPY(this, 5));
			m_ContentSizerBase->Add(m_ContentSizer, 1, wxEXPAND|wxALL, FromDIPY(this, 5));
		}

		if (m_IconSizer)
		{
			m_IconSizer->Add(m_IconView, 0, wxLEFT|wxTOP, FromDIPX(this, 10));
		}

		if (m_ContentSizer)
		{
			m_ContentSizer->AddSpacer(FromDIPY(this, 4));
			m_ContentSizer->Add(m_CaptionLabel, 0, wxLEFT|wxRIGHT|wxEXPAND, FromDIPX(this, 2));

			wxWindow* mainCtrl = GetDialogMainCtrl();
			mainCtrl->Reparent(m_ContentPanel);

			if (GetViewLabelSizerOrientation() == wxHORIZONTAL)
			{
				m_ContentSizer->AddSpacer(FromDIPY(this, 6));
				wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
				m_ContentSizer->Add(sizer, 1, wxEXPAND);

				m_ViewLabelSI = sizer->Add(m_ViewLabel, 0, wxLEFT|wxEXPAND, FromDIPX(this, 3));
				wxBoxSizer* viewSizer = new wxBoxSizer(GetViewSizerOrientation());
				sizer->Add(viewSizer, 1, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, FromDIPX(this, 5));
				viewSizer->Add(mainCtrl, GetViewSizerProportion(), wxEXPAND);
			}
			else
			{
				m_ViewLabelSpacerSI = m_ContentSizer->AddSpacer(FromDIPX(this, 6));
				m_ViewLabelSI = m_ContentSizer->Add(m_ViewLabel, 0, wxLEFT|wxEXPAND, FromDIPX(this, 3));

				wxBoxSizer* viewSizer = new wxBoxSizer(GetViewSizerOrientation());
				m_ContentSizer->Add(viewSizer, 1, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, FromDIPX(this, 5));
				viewSizer->Add(mainCtrl, GetViewSizerProportion(), wxEXPAND);
			}
			m_ViewLabelSI->Show(false);
		}

		if (m_ButtonsSizer)
		{
			m_DialogSizer->Add(m_ButtonsSizer, 0, wxEXPAND);
			m_ButtonsSizer->AddSpacer(FromDIPX(this, 6));
			m_ButtonsSizer->SetMinSize(wxDefaultCoord, FromDIPY(this, 40));
		}

		InitIcon();
		AdjustWindow(pos);
	}
	void StdDialog::AdjustWindow(const Point& pos, const Size& minSize)
	{
		if (IsAutoSizeEnabled())
		{
			if (minSize.IsFullySpecified())
			{
				SetMinSize(minSize);
			}
			else
			{
				SetMaxSize(Size::UnspecifiedSize());
				SetMinSize(m_DialogSizer->Fit(this));
			}
			SetSize(GetMinSize());
		}

		SetResizingBehavior();
		if (pos != Point(-2, -2))
		{
			CenterIfNoPosition(pos);
		}
	}

	void StdDialog::SetMainIcon(const GDIBitmap& icon)
	{
		if (icon)
		{
			m_MainIcon = icon;
			m_MainIconID = StdIcon::None;
			LoadIcon();
		}
		else
		{
			m_MainIconID = StdIcon::None;
		}
		SetIconVisibility();
	}
	void StdDialog::SetMainIcon(StdIcon iconID)
	{
		m_MainIconID = iconID;
		if (iconID != StdIcon::None)
		{
			m_MainIcon = ArtProvider::GetMessageBoxResource(iconID).ToGDIBitmap();
			LoadIcon();
		}
		SetIconVisibility();
	}

	int StdDialog::ShowModal()
	{
		GetDialogFocusCtrl()->SetFocus();
		return Dialog::ShowModal();
	}
	void StdDialog::SetLabel(const wxString& label)
	{
		m_ViewLabel->SetLabel(label);
		m_ViewLabelSI->Show(!label.IsEmpty());
		if (m_ViewLabelSpacerSI)
		{
			m_ViewLabelSpacerSI->Show(label.IsEmpty());
		}

		int width = m_ViewLabel->GetTextExtent(label).GetWidth();
		m_ViewLabelSI->SetMinSize(width + 0.05*width, wxDefaultCoord);
		AdjustWindow();
	}

	void StdDialog::SetDefaultButton(WidgetID id)
	{
		auto control = GetButton(id);
		if (control.IsControl())
		{
			Button* button = control.As<Button>();
			if (button)
			{
				button->SetDefault();
			}
			SetDefaultItem(control.GetControl());
			control.GetControl()->SetFocus();
		}
		else
		{
			wxWindow* window = wxWindow::FindWindowById(control, this);
			if (window)
			{
				window->SetFocus();
			}
		}
	}
	StdDialogControl StdDialog::GetButton(WidgetID id) const
	{
		for (wxSizerItem* item: m_ButtonsSizer->GetChildren())
		{
			wxWindow* window = item->GetWindow();
			if (window && window->GetId() == id.ToInt() && window->GetId() != wxID_NONE)
			{
				if (Button* button = dynamic_cast<Button*>(window))
				{
					return button;
				}
				else
				{
					return window->GetId();
				}
			}
		}
		return static_cast<wxWindow*>(nullptr);
	}
	StdDialogControl StdDialog::AddButton(WidgetID id, const String& label, bool prepend)
	{
		Button* button = m_ButtonsSizer->CreateButton(this, StdButton::WX_LAST_STD, StdButton::WX_LAST_STD, id, false);
		m_ButtonsSizer->AddCustomButton(button, label, prepend);
		m_CloseIDs.push_back(id);

		return button;
	}
}
