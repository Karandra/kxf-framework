#pragma once
#include "Kx/UI/Common.h"
#include "StdDialog.h"
#include "Kx/Utility/Common.h"
#include <CommCtrl.h>

namespace KxFramework::UI
{
	enum KxTD_Options
	{
		KxTD_NONE = 0,

		KxTD_HYPERLINKS_ENABLED = 1 << 0,
		KxTD_CMDLINKS_ENABLED = 1 << 1,
		KxTD_CMDLINKS_NO_ICON = 1 << 2,
		KxTD_EXMESSAGE_IN_FOOTER = 1 << 3,
		KxTD_EXMESSAGE_EXPANDED = 1 << 4,
		KxTD_PB_SHOW = 1 << 5,
		KxTD_PB_PULSE = 1 << 6,
		KxTD_CALLBACK_TIMER = 1 << 7,
		KxTD_RB_NO_DEFAULT = 1 << 8,
		KxTD_CHB_CHECKED = 1 << 9,
		KxTD_CHB_ENABLED = 1 << 10,
		KxTD_SIZE_TO_CONTENT = 1 << 11
	};
}

namespace KxFramework::UI
{
	class KX_API KxTaskDialog: public Dialog, public IStdDialog
	{
		public:
			using ButtonSpecArray = std::vector<TASKDIALOG_BUTTON>;

		private:
			TASKDIALOGCONFIG m_DialogConfig = {0};
			KxTD_Options m_Options = KxTD_NONE;
			wxWindow* m_Parent = nullptr;
			HWND m_Handle = nullptr;
			HRESULT m_Result = S_FALSE;

			wxIcon m_MainIcon;
			wxIcon m_FooterIcon;
			StdIcon m_MainIconID = StdIcon::Information;
			StdIcon m_FooterIconID = StdIcon::None;

			String m_Title;
			String m_Caption;
			String m_ExMessage;
			String m_Message;
			String DialogExpandButtonExpand;
			String DialogExpandButtonCollapse;
			String m_FooterMessage;
			String m_CheckBoxLabel;
			BOOL m_CheckBoxChecked = FALSE;
			wxWindowID m_SelectedRadioButton = wxID_NONE;

			std::vector<String> m_ButtonLabels;
			std::vector<String> m_RadioButtonLabels;
			std::vector<TASKDIALOG_BUTTON> m_ButtonsID;
			std::vector<TASKDIALOG_BUTTON> m_RadioButtonsID;

		private:
			static HRESULT CALLBACK EventsCallback(HWND handle, UINT notification, WPARAM wParam, LPARAM lParam, LONG_PTR refData);
			int DoShowDialog(bool isModal = true);
			void OnClose(wxCloseEvent& event);

			int SetStdButtonsFromWx(StdButton buttons);
			int SetAutoDefaultButton(StdButton buttons);

			wxWindowID TranslateButtonID_WinToWx(int idWin) const;
			int TranslateButtonID_WxToWin(wxWindowID idWx) const;
			LPCWSTR TranslateIconIDToTDI(StdIcon id) const;

			const wxChar* GetStringOrNull(const String& text)
			{
				return text.IsEmpty() ? nullptr : text.wc_str();
			}
			void SetFlagTDI(TASKDIALOG_FLAGS flag, bool set = true)
			{
				KxFramework::Utility::ModFlagRef(m_DialogConfig.dwFlags, flag, set);
			}
			bool IsFlagSetTDI(TASKDIALOG_FLAGS flag) const
			{
				return KxFramework::Utility::HasFlag(m_DialogConfig.dwFlags, flag);
			}

			void UpdateButtonArrays(const std::vector<String>& labels, ButtonSpecArray& array);

		public:
			static const KxTD_Options DefaultStyle = KxTD_NONE;
			static const StdButton DefaultButtons = StdButton::OK;
		
			KxTaskDialog() = default;
			KxTaskDialog(wxWindow* parent,
						 wxWindowID id,
						 const String& caption = {},
						 const String& message = {},
						 StdButton buttons = DefaultButtons,
						 StdIcon mainIcon = StdIcon::None,
						 wxPoint pos = wxDefaultPosition,
						 wxSize size = wxDefaultSize,
						 long style = DefaultStyle
			)
			{
				Create(parent, id, caption, message, buttons, mainIcon, pos, size, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& caption = {},
						const String& message = {},
						StdButton buttons = DefaultButtons,
						StdIcon mainIcon = StdIcon::None,
						wxPoint pos = wxDefaultPosition,
						wxSize size = wxDefaultSize,
						long style = DefaultStyle
			);

		public:
			bool IsOK()
			{
				return SUCCEEDED(m_Result);
			}
			WXHWND GetHandle() const override
			{
				return m_Handle;
			}
			bool Show(bool show = true) override;
			int ShowModal() override;
			bool Realize()
			{
				if (GetHandle())
				{
					return SendMessageW(GetHandle(), TDM_NAVIGATE_PAGE, 0, (LPARAM)&m_DialogConfig) == S_OK;
				}
				return false;
			}

			// Options
			bool IsOptionEnabled(KxTD_Options flag) const
			{
				return m_Options & flag;
			}
			void SetOptionEnabled(KxTD_Options flag, bool isEnabled = true)
			{
				SetOptions(KxFramework::Utility::ModFlag(m_Options, flag, isEnabled));
			}
			KxTD_Options GetOptions() const
			{
				return m_Options;
			}
			void SetOptions(KxTD_Options options);

			// Icons
			StdIcon GetMainIconID() const override
			{
				return m_MainIconID;
			}
			wxBitmap GetMainIcon() const override
			{
				if (m_MainIcon.IsOk())
				{
					return m_MainIcon;
				}
				else
				{
					return wxArtProvider::GetMessageBoxIcon(TranslateIconIDToWx(GetMainIconID()));
				}
			}
			void SetMainIcon(StdIcon iconID) override
			{
				if (iconID != StdIcon::Question)
				{
					m_MainIconID = iconID;
					m_MainIcon = wxNullIcon;

					SetFlagTDI(TDF_USE_HICON_MAIN, false);
					m_DialogConfig.hMainIcon = nullptr;
					m_DialogConfig.pszMainIcon = TranslateIconIDToTDI(m_MainIconID);
				}
				else
				{
					SetMainIcon(wxArtProvider::GetMessageBoxIcon(wxICON_QUESTION));
					m_MainIconID = iconID;
				}
			}
			void SetMainIcon(const wxBitmap& icon) override
			{
				m_MainIconID = StdIcon::None;
				m_MainIcon.CopyFromBitmap(icon);

				SetFlagTDI(TDF_USE_HICON_MAIN, true);
				m_DialogConfig.pszMainIcon = nullptr;
				m_DialogConfig.hMainIcon = m_MainIcon.GetHICON();
			}
		
			StdIcon GetFooterIconID()
			{
				return m_FooterIconID;
			}
			wxBitmap GetFooterIcon()
			{
				if (m_FooterIcon.IsOk())
				{
					return m_FooterIcon;
				}
				else
				{
					return wxArtProvider::GetMessageBoxIcon(TranslateIconIDToWx(GetFooterIconID()));
				}
			}
			void SetFooterIcon(StdIcon iconID)
			{
				m_FooterIconID = iconID;
				m_FooterIcon = wxNullIcon;

				SetFlagTDI(TDF_USE_HICON_FOOTER, false);
				m_DialogConfig.pszFooterIcon = TranslateIconIDToTDI(m_FooterIconID);
				m_DialogConfig.hFooterIcon = nullptr;
			}
			void SetFooterIcon(const wxBitmap& icon)
			{
				m_FooterIconID = StdIcon::None;
				m_FooterIcon.CopyFromBitmap(icon);

				SetFlagTDI(TDF_USE_HICON_FOOTER, true);
				m_DialogConfig.pszFooterIcon = nullptr;
				m_DialogConfig.hFooterIcon = m_FooterIcon.GetHICON();
			}

			// Strings
			wxString GetTitle() const override
			{
				return m_Title;
			}
			void SetTitle(const wxString& string = {}) override
			{
				if (string.IsEmpty())
				{
					m_Title = wxTheApp->GetAppDisplayName();
				}
				else
				{
					m_Title = string;
				}
				m_DialogConfig.pszWindowTitle = m_Title.wc_str();
			}

			wxString GetCaption() const override
			{
				return m_Caption;
			}
			void SetCaption(const wxString& string) override
			{
				m_Caption = string;
				m_DialogConfig.pszMainInstruction = GetStringOrNull(m_Caption);
			}

			wxString GetLabel() const override
			{
				return GetMessage();
			}
			void SetLabel(const wxString& string) override
			{
				SetMessage(string);
			}

			String GetMessage() const
			{
				return m_Message;
			}
			void SetMessage(const String& string)
			{
				m_Message = string;
				m_DialogConfig.pszContent = GetStringOrNull(m_Message);
			}
			
			String GetExMessage() const
			{
				return m_ExMessage;
			}
			void SetExMessage(const String& string)
			{
				m_ExMessage = string;
				m_DialogConfig.pszExpandedInformation = GetStringOrNull(m_ExMessage);
			}
			
			String GetFooter() const
			{
				return m_FooterMessage;
			}
			void SetFooter(const String& string)
			{
				m_FooterMessage = string;
				m_DialogConfig.pszFooter = GetStringOrNull(m_FooterMessage);
			}
			
			String GetCheckBoxLabel() const
			{
				return m_CheckBoxLabel;
			}
			void SetCheckBoxLabel(const String& text)
			{
				m_CheckBoxLabel = text;
				m_DialogConfig.pszVerificationText = GetStringOrNull(m_CheckBoxLabel);
			}

			// Buttons
			void SetStandardButtons(StdButton buttons)
			{
				SetStdButtonsFromWx(buttons);
			}
			void SetDefaultButton(wxWindowID id) override
			{
				m_DialogConfig.nDefaultButton = TranslateButtonID_WxToWin(id);
			}
			void SetDefaultRadioButton(wxWindowID id)
			{
				m_DialogConfig.nDefaultRadioButton = id;
			}
		
			StdDialogControl GetButton(wxWindowID id) const override
			{
				return id;
			}
			StdDialogControl AddButton(wxWindowID id, const String& label = {}, bool prepend = false) override;
			StdDialogControl AddRadioButton(wxWindowID id, const String& label = {});
		
			bool IsCheckBoxChecked() const
			{
				return m_CheckBoxChecked;
			}
			wxWindowID GetSelectedRadioButton() const
			{
				return m_SelectedRadioButton;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(KxTaskDialog);
	};
}