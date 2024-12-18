#include "kxf-pch.h"
#include "TaskDialogNativeInfo.h"
#include "../TaskDialog.h"
#include "kxf/Core/ErrorCode.h"
#include "kxf/Utility/Common.h"

#include <CommCtrl.h>
#include "kxf/Win32/UndefMacros.h"

namespace
{
	using namespace kxf;
	using namespace kxf::UI;

	const XChar* GetStringOrNull(const String& text)
	{
		return text.IsEmpty() ? nullptr : text.wc_str();
	}

	template<class T>
	std::vector<TASKDIALOG_BUTTON> MakeButtonSpecs(const T& items)
	{
		std::vector<TASKDIALOG_BUTTON> buttonsSpec;
		buttonsSpec.reserve(items.size());
		for (const auto& item: items)
		{
			buttonsSpec.emplace_back(TASKDIALOG_BUTTON{item.ID, item.Label.wc_str()});
		}

		return buttonsSpec;
	}

	constexpr wxWindowID MapButtonIDFromNative(int id) noexcept
	{
		switch (id)
		{
			case IDCANCEL:
			{
				return wxID_CANCEL;
			}
			case IDABORT:
			{
				return wxID_ABORT;
			}
			case IDCLOSE:
			{
				return wxID_CLOSE;
			}
			case IDNO:
			{
				return wxID_NO;
			}
			case IDHELP:
			{
				return wxID_HELP;
			}
			case IDRETRY:
			{
				return wxID_RETRY;
			}
			case IDYES:
			{
				return wxID_YES;
			}
			case IDOK:
			{
				return wxID_OK;
			}
		};
		return id;
	}
	constexpr int MapButtonIDToNative(wxWindowID id) noexcept
	{
		switch (id)
		{
			case wxID_CANCEL:
			{
				return IDCANCEL;
			}
			case wxID_ABORT:
			{
				return IDABORT;
			}
			case wxID_CLOSE:
			{
				return IDCLOSE;
			}
			case wxID_NO:
			{
				return IDNO;
			}
			case wxID_HELP:
			{
				return IDHELP;
			}
			case wxID_RETRY:
			{
				return IDRETRY;
			}
			case wxID_YES:
			{
				return IDYES;
			}
			case wxID_OK:
			{
				return IDOK;
			}
		};
		return id;
	}

	constexpr const wchar_t* MapIconID(StdIcon id) noexcept
	{
		enum class TDIIcon
		{
			ICON_NONE = 0,
			ICON_QUESTION = -1,

			ICON_WARNING = 1,
			ICON_ERROR,
			ICON_INFORMATION,
			ICON_SHIELD,
			ICON_SHIELDHEADER,
			ICON_SHIELDWARNING,
			ICON_SHIELDERROR,
			ICON_SHIELDSUCCESS,
			ICON_SHIELDGRAY
		};

		if (id != StdIcon::Question)
		{
			if (id != StdIcon::None)
			{
				TDIIcon idTDI = TDIIcon::ICON_NONE;
				switch (id)
				{
					case StdIcon::Information:
					{
						idTDI = TDIIcon::ICON_INFORMATION;
						break;
					}
					case StdIcon::Warning:
					{
						idTDI = TDIIcon::ICON_WARNING;
						break;
					}
					case StdIcon::Error:
					{
						idTDI = TDIIcon::ICON_ERROR;
						break;
					}
					case StdIcon::Shield:
					{
						idTDI = TDIIcon::ICON_SHIELD;
						break;
					}
					case StdIcon::Shield|StdIcon::Warning:
					{
						idTDI = TDIIcon::ICON_SHIELDWARNING;
						break;
					}
					case StdIcon::Shield|StdIcon::Error:
					{
						idTDI = TDIIcon::ICON_SHIELDERROR;
						break;
					}
					case StdIcon::Shield|StdIcon::Success:
					{
						idTDI = TDIIcon::ICON_SHIELDSUCCESS;
						break;
					}
					case StdIcon::Shield|StdIcon::Neutral:
					{
						idTDI = TDIIcon::ICON_SHIELDGRAY;
						break;
					}
				};
				return MAKEINTRESOURCEW(-static_cast<int>(idTDI));
			}
			return nullptr;
		}
		return reinterpret_cast<LPCWSTR>(-1);
	}
	constexpr FlagSet<TASKDIALOG_FLAGS> MapTaskDialogStyle(FlagSet<TaskDialogStyle> style) noexcept
	{
		using namespace kxf;
		using namespace kxf::UI;

		FlagSet<TASKDIALOG_FLAGS> nativeFlags;
		nativeFlags.Add(TDF_ENABLE_HYPERLINKS, style & TaskDialogStyle::Hyperlinks);
		nativeFlags.Add(TDF_USE_COMMAND_LINKS, style & TaskDialogStyle::CommandLinks);
		nativeFlags.Add(TDF_USE_COMMAND_LINKS_NO_ICON, style & TaskDialogStyle::CommandLinksNoIcon);
		nativeFlags.Add(TDF_EXPAND_FOOTER_AREA, style & TaskDialogStyle::ExMessageInFooter);
		nativeFlags.Add(TDF_EXPANDED_BY_DEFAULT, style & TaskDialogStyle::ExMessageExpanded);
		nativeFlags.Add(TDF_SHOW_PROGRESS_BAR, style & TaskDialogStyle::ProgressBar);
		nativeFlags.Add(TDF_SHOW_MARQUEE_PROGRESS_BAR, style & TaskDialogStyle::ProgressBarPulse);
		nativeFlags.Add(TDF_CALLBACK_TIMER, style & TaskDialogStyle::CallbackTimer);
		nativeFlags.Add(TDF_VERIFICATION_FLAG_CHECKED, style & TaskDialogStyle::CheckBoxChecked);
		nativeFlags.Add(TDF_SIZE_TO_CONTENT, style & TaskDialogStyle::SizeToContent);

		return nativeFlags;
	}
}

namespace kxf::UI::Private
{
	wxNotifyEvent TaskDialogNativeInfo::CreateEvent(const EventID& eventID)
	{
		wxNotifyEvent event;
		event.SetEventType(eventID.AsInt());
		event.SetEventObject(&m_TaskDialog);
		event.SetId(wxID_NONE);
		event.Allow();

		return event;
	}
	wxWindowID TaskDialogNativeInfo::TranslateButtonIDFromNative(int id) const noexcept
	{
		return ::MapButtonIDToNative(id);
	}

	TaskDialogNativeInfo::TaskDialogNativeInfo(TaskDialog& dialog)
		:m_TaskDialog(dialog)
	{
		m_DialogConfig.cbSize = sizeof(m_DialogConfig);
		m_DialogConfig.cxWidth = 0;
		m_DialogConfig.hInstance = ::GetModuleHandleW(nullptr);
		m_DialogConfig.lpCallbackData = reinterpret_cast<LONG_PTR>(this);
		m_DialogConfig.pfCallback = [](HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR refData) -> HRESULT
		{
			TaskDialog& taskDialog = reinterpret_cast<TaskDialogNativeInfo*>(refData)->m_TaskDialog;
			return *taskDialog.OnDialogEvent(windowHandle, msg, reinterpret_cast<void*>(wParam), reinterpret_cast<void*>(lParam));
		};
	}

	void TaskDialogNativeInfo::UpdateBase()
	{
		m_DialogConfig.dwFlags = *MapTaskDialogStyle(m_TaskDialog.m_Style);
	}
	void TaskDialogNativeInfo::UpdateText()
	{
		m_DialogConfig.pszWindowTitle = m_TaskDialog.m_Title.wc_str();
		m_DialogConfig.pszMainInstruction = GetStringOrNull(m_TaskDialog.m_Caption);
		m_DialogConfig.pszContent = GetStringOrNull(m_TaskDialog.m_Message);
		m_DialogConfig.pszExpandedInformation = GetStringOrNull(m_TaskDialog.m_ExMessage);
		m_DialogConfig.pszFooter = GetStringOrNull(m_TaskDialog.m_FooterMessage);
		m_DialogConfig.pszVerificationText = GetStringOrNull(m_TaskDialog.m_CheckBoxLabel);
	}
	void TaskDialogNativeInfo::UpdateIcons()
	{
		FlagSet<decltype(m_DialogConfig.dwFlags)> flags = m_DialogConfig.dwFlags;

		if (m_TaskDialog.m_MainIcon)
		{
			m_DialogConfig.pszMainIcon = nullptr;
			m_DialogConfig.hMainIcon = reinterpret_cast<HICON>(m_TaskDialog.m_MainIcon.GetHandle());

			flags.Add(TDF_USE_HICON_MAIN);
		}
		else
		{
			m_DialogConfig.hMainIcon = nullptr;
			m_DialogConfig.pszMainIcon = MapIconID(m_TaskDialog.m_MainIconID);
			flags.Remove(TDF_USE_HICON_MAIN);
		}

		if (m_TaskDialog.m_FooterIcon)
		{
			m_DialogConfig.pszFooterIcon = nullptr;
			m_DialogConfig.hFooterIcon = reinterpret_cast<HICON>(m_TaskDialog.m_FooterIcon.GetHandle());
			flags.Add(TDF_USE_HICON_FOOTER);
		}
		else
		{
			m_DialogConfig.pszFooterIcon = MapIconID(m_TaskDialog.m_FooterIconID);
			m_DialogConfig.hFooterIcon = nullptr;
			flags.Remove(TDF_USE_HICON_FOOTER);
		}
		m_DialogConfig.dwFlags = *flags;
	}
	void TaskDialogNativeInfo::UpdateButtonSpecs()
	{
		// Push buttons
		UpdateStdButtons(m_TaskDialog.m_StandardButtons);
		m_ButtonSpecs = MakeButtonSpecs(m_TaskDialog.m_Buttons);
		m_DialogConfig.pButtons = m_ButtonSpecs.data();
		m_DialogConfig.cButtons = m_ButtonSpecs.size();

		// Default push button
		if (m_TaskDialog.m_DefaultButton != wxID_NONE)
		{
			m_DialogConfig.nDefaultButton = MapButtonIDToNative(m_TaskDialog.m_DefaultButton);
		}
		else
		{
			UpdateAutoDefaultButton(m_TaskDialog.m_StandardButtons);
		}

		// Radio buttons
		m_RadioButtonSpecs = MakeButtonSpecs(m_TaskDialog.m_RadioButtons);
		m_DialogConfig.pRadioButtons = m_RadioButtonSpecs.data();
		m_DialogConfig.cRadioButtons = m_RadioButtonSpecs.size();

		// Default radio button
		m_DialogConfig.nDefaultRadioButton = m_TaskDialog.m_DefaultRadioButton;
		if (m_TaskDialog.m_DefaultRadioButton == wxID_NONE)
		{
			m_DialogConfig.dwFlags |= TDF_NO_DEFAULT_RADIO_BUTTON;
		}
	}
	void TaskDialogNativeInfo::UpdateStdButtons(FlagSet<StdButton> buttons)
	{
		m_DialogConfig.dwCommonButtons = 0;
		if (buttons & StdButton::OK)
		{
			m_DialogConfig.dwCommonButtons |= TDCBF_OK_BUTTON;
		}
		if (buttons & StdButton::Yes)
		{
			m_DialogConfig.dwCommonButtons |= TDCBF_YES_BUTTON;
		}
		if (buttons & StdButton::No)
		{
			m_DialogConfig.dwCommonButtons |= TDCBF_NO_BUTTON;
		}
		if (buttons & StdButton::Cancel)
		{
			m_DialogConfig.dwCommonButtons |= TDCBF_CANCEL_BUTTON;
		}
		if (buttons & StdButton::Retry)
		{
			m_DialogConfig.dwCommonButtons |= TDCBF_RETRY_BUTTON;
		}
		if (buttons & StdButton::Close)
		{
			m_DialogConfig.dwCommonButtons |= TDCBF_CLOSE_BUTTON;
		}
		if (buttons & StdButton::Yes)
		{
			m_DialogConfig.dwCommonButtons |= TDCBF_YES_BUTTON;
		}
	}
	void TaskDialogNativeInfo::UpdateAutoDefaultButton(FlagSet<StdButton> buttons)
	{
		m_DialogConfig.nDefaultButton = 0;
		if (buttons & StdButton::Cancel)
		{
			m_DialogConfig.nDefaultButton = IDCANCEL;
		}
		if (buttons & StdButton::Abort)
		{
			m_DialogConfig.nDefaultButton = IDABORT;
		}
		else if (buttons & StdButton::Close)
		{
			m_DialogConfig.nDefaultButton = IDCLOSE;
		}
		else if (buttons & StdButton::No)
		{
			m_DialogConfig.nDefaultButton = IDNO;
		}
		else if (buttons & StdButton::Help)
		{
			m_DialogConfig.nDefaultButton = IDHELP;
		}
		else if (buttons & StdButton::Retry)
		{
			m_DialogConfig.nDefaultButton = IDRETRY;
		}
		else if (buttons & StdButton::Yes)
		{
			m_DialogConfig.nDefaultButton = IDYES;
		}
		else if (buttons & StdButton::OK)
		{
			m_DialogConfig.nDefaultButton = IDOK;
		}
	}

	void TaskDialogNativeInfo::Realize()
	{
		UpdateBase();
		UpdateText();
		UpdateIcons();
		UpdateButtonSpecs();
	}
	bool TaskDialogNativeInfo::UpdateWindowUI(bool sendNavigateEvents)
	{
		if (m_TaskDialog.m_Handle)
		{
			if (sendNavigateEvents)
			{
				auto event = CreateEvent(IStdDialog::EvtNavigating);
				if (m_TaskDialog.ProcessEvent(event) && !event.IsAllowed())
				{
					return false;
				}
			}

			if (::SendMessageW(reinterpret_cast<HWND>(m_TaskDialog.m_Handle), TDM_NAVIGATE_PAGE, 0, reinterpret_cast<LPARAM>(&m_DialogConfig)) == 0)
			{
				if (sendNavigateEvents)
				{
					auto event = CreateEvent(IStdDialog::EvtNavigate);
					m_TaskDialog.ProcessEvent(event);
				}
				return true;
			}
		}
		return false;
	}
	bool TaskDialogNativeInfo::UpdateTextElement(int id, const String& value)
	{
		return ::SendMessageW(reinterpret_cast<HWND>(m_TaskDialog.m_Handle), TDM_SET_ELEMENT_TEXT, id, value.wc_str().unsafe_intptr()) == 0;
	}

	wxWindowID TaskDialogNativeInfo::ShowDialog(bool isModal)
	{
		// Configure parent window and position
		wxWindow* parent = m_TaskDialog.m_Parent;
		if (isModal)
		{
			m_DialogConfig.hwndParent = parent ? parent->GetHandle() : nullptr;
		}
		else
		{
			m_DialogConfig.hwndParent = nullptr;
		}

		FlagSet<decltype(m_DialogConfig.dwFlags)> flags = m_DialogConfig.dwFlags;
		flags.Mod(TDF_CAN_BE_MINIMIZED, !isModal);
		flags.Mod(TDF_POSITION_RELATIVE_TO_WINDOW, parent != nullptr);
		m_DialogConfig.dwFlags = *flags;

		// Send show event
		{
			wxShowEvent event(wxEVT_SHOW, true);
			event.SetId(m_TaskDialog.GetId());
			event.SetEventObject(&m_TaskDialog);
			event.SetShow(true);
			m_TaskDialog.ProcessWindowEvent(event);
		}

		// Show the dialog
		int pushButtonCode = wxID_CANCEL;
		int radioButtonCode = wxID_NONE;

		BOOL isChecked = m_TaskDialog.m_Style & TaskDialogStyle::CheckBoxChecked;
		BOOL* checkedPtr = m_TaskDialog.m_Style & TaskDialogStyle::CheckBox ? &isChecked : nullptr;

		m_TaskDialog.SetReturnCode(wxID_CANCEL);
		m_TaskDialog.m_SelectedRadioButton = wxID_NONE;
		m_TaskDialog.m_CheckBoxValue = false;

		wxWindowID result = wxID_CANCEL;
		if (HResult(::TaskDialogIndirect(&m_DialogConfig, &pushButtonCode, &radioButtonCode, checkedPtr)))
		{
			result = MapButtonIDFromNative(pushButtonCode);

			m_TaskDialog.SetReturnCode(result);
			m_TaskDialog.m_SelectedRadioButton = radioButtonCode;
			m_TaskDialog.m_CheckBoxValue = checkedPtr ? *checkedPtr : false;
		}

		// Send close event
		{
			wxShowEvent event(wxEVT_SHOW, false);
			event.SetId(m_TaskDialog.GetId());
			event.SetEventObject(&m_TaskDialog);
			event.SetShow(false);
			m_TaskDialog.ProcessWindowEvent(event);
		}

		return result;
	}
}
