#include "kxf-pch.h"
#include "ConstantsMapping.h"
#include <wx/defs.h>
#include <wx/event.h>

namespace kxf
{
	static_assert(ToInt(StdID::WX_AUTO_LOWEST) == wxID_AUTO_LOWEST);
	static_assert(ToInt(StdID::WX_AUTO_HIGHEST) == wxID_AUTO_HIGHEST);
	static_assert(ToInt(StdID::WX_LOWEST) == wxID_LOWEST);
	static_assert(ToInt(StdID::WX_HIGHEST) == wxID_HIGHEST);

	static_assert(ToInt(StdID::None) == wxID_NONE);
	static_assert(ToInt(StdID::Separator) == wxID_SEPARATOR);
	static_assert(ToInt(StdID::Any) == wxID_ANY);
	static_assert(ToInt(StdID::Open) == wxID_OPEN);
	static_assert(ToInt(StdID::Edit) == wxID_EDIT);

	static_assert(ToInt(StdID::OK) == wxID_OK);
	static_assert(ToInt(StdID::SystemMenu) == wxID_SYSTEM_MENU);
	static_assert(ToInt(StdID::MDIWindowFirst) == wxID_MDI_WINDOW_FIRST);
	static_assert(ToInt(StdID::MDIWindowLast) == wxID_MDI_WINDOW_LAST);

	static_assert(ToInt(StdID::FileDialog) == wxID_FILEDLGG);
	static_assert(ToInt(StdID::FileControl) == wxID_FILECTRL);
	static_assert(ToInt(StdID::KX_LOWEST) == wxID_HIGHEST + 1);
}

namespace kxf::wxWidgets
{
	StdID MapStdID(wxStandardID id) noexcept
	{
		return static_cast<StdID>(id);
	}
	wxStandardID MapStdID(StdID id) noexcept
	{
		return static_cast<wxStandardID>(id);
	}

	FlagSet<StdIcon> MapStdIcon(uint32_t stdWxIcon) noexcept
	{
		FlagSet<StdIcon> stdIcon;
		stdIcon.Add(StdIcon::Error, stdWxIcon & wxICON_ERROR);
		stdIcon.Add(StdIcon::Warning, stdWxIcon & wxICON_WARNING);
		stdIcon.Add(StdIcon::Question, stdWxIcon & wxICON_QUESTION);
		stdIcon.Add(StdIcon::Information, stdWxIcon & wxICON_INFORMATION);
		stdIcon.Add(StdIcon::Authentication, stdWxIcon & wxICON_AUTH_NEEDED);

		return stdIcon;
	}
	FlagSet<uint32_t> MapStdIcon(FlagSet<StdIcon> stdIcon) noexcept
	{
		FlagSet<uint32_t> wxStdIcon = wxICON_NONE;
		wxStdIcon.Add(wxICON_ERROR, stdIcon & StdIcon::Error);
		wxStdIcon.Add(wxICON_WARNING, stdIcon & StdIcon::Warning);
		wxStdIcon.Add(wxICON_QUESTION, stdIcon & StdIcon::Question);
		wxStdIcon.Add(wxICON_INFORMATION, stdIcon & StdIcon::Information);
		wxStdIcon.Add(wxICON_AUTH_NEEDED, stdIcon & StdIcon::Authentication);

		return wxStdIcon;
	}

	StdButton MapStdButton(uint32_t stdWxButton) noexcept
	{
		return FromInt<StdButton>(stdWxButton);
	}
	uint32_t MapStdButton(StdButton stdButton) noexcept
	{
		return ToInt(stdButton);
	}
}

namespace kxf::wxWidgets
{
	wxKeyCode MapKeyCode(KeyCode code) noexcept
	{
		static_assert(ToInt(KeyCode::Control_A) == wxKeyCode::WXK_CONTROL_A);
		static_assert(ToInt(KeyCode::Control_Z) == wxKeyCode::WXK_CONTROL_Z);
		static_assert(ToInt(KeyCode::START) == wxKeyCode::WXK_START);
		static_assert(ToInt(KeyCode::Special1) == wxKeyCode::WXK_SPECIAL1);
		static_assert(ToInt(KeyCode::BrowserBack) == wxKeyCode::WXK_BROWSER_BACK);

		return static_cast<wxKeyCode>(code);
	}
	KeyCode MapKeyCode(wxKeyCode code) noexcept
	{
		return static_cast<KeyCode>(code);
	}

	FlagSet<KeyModifier> MapKeyModifier(FlagSet<wxKeyModifier> keyModifier) noexcept
	{
		FlagSet<KeyModifier> result;
		result.Add(KeyModifier::Alt, keyModifier.Contains(wxKeyModifier::wxMOD_ALT));
		result.Add(KeyModifier::AltGR, keyModifier.Contains(wxKeyModifier::wxMOD_ALTGR));
		result.Add(KeyModifier::Ctrl, keyModifier.Contains(wxKeyModifier::wxMOD_CONTROL));
		result.Add(KeyModifier::Shift, keyModifier.Contains(wxKeyModifier::wxMOD_SHIFT));
		result.Add(KeyModifier::Meta, keyModifier.Contains(wxKeyModifier::wxMOD_META));

		return result;
	}
	FlagSet<wxKeyModifier> MapKeyModifier(FlagSet<KeyModifier> keyModifier) noexcept
	{
		FlagSet<wxKeyModifier> result;
		result.Add(wxKeyModifier::wxMOD_ALT, keyModifier.Contains(KeyModifier::Alt));
		result.Add(wxKeyModifier::wxMOD_ALTGR, keyModifier.Contains(KeyModifier::AltGR));
		result.Add(wxKeyModifier::wxMOD_CONTROL, keyModifier.Contains(KeyModifier::Ctrl));
		result.Add(wxKeyModifier::wxMOD_SHIFT, keyModifier.Contains(KeyModifier::Shift));
		result.Add(wxKeyModifier::wxMOD_META, keyModifier.Contains(KeyModifier::Meta));

		return result;
	}

	FlagSet<KeyCategory> MapKeyCategory(FlagSet<wxKeyCategoryFlags> keyCategory) noexcept
	{
		FlagSet<KeyCategory> result;
		result.Add(KeyCategory::Arrow, keyCategory.Contains(wxKeyCategoryFlags::WXK_CATEGORY_ARROW));
		result.Add(KeyCategory::Paging, keyCategory.Contains(wxKeyCategoryFlags::WXK_CATEGORY_PAGING));
		result.Add(KeyCategory::Jump, keyCategory.Contains(wxKeyCategoryFlags::WXK_CATEGORY_JUMP));
		result.Add(KeyCategory::Tab, keyCategory.Contains(wxKeyCategoryFlags::WXK_CATEGORY_TAB));
		result.Add(KeyCategory::Delete, keyCategory.Contains(wxKeyCategoryFlags::WXK_CATEGORY_CUT));

		return result;
	}
	FlagSet<wxKeyCategoryFlags> MapKeyCategory(FlagSet<KeyCategory> keyCategory) noexcept
	{
		FlagSet<wxKeyCategoryFlags> result;
		result.Add(wxKeyCategoryFlags::WXK_CATEGORY_ARROW, keyCategory.Contains(KeyCategory::Arrow));
		result.Add(wxKeyCategoryFlags::WXK_CATEGORY_PAGING, keyCategory.Contains(KeyCategory::Paging));
		result.Add(wxKeyCategoryFlags::WXK_CATEGORY_JUMP, keyCategory.Contains(KeyCategory::Jump));
		result.Add(wxKeyCategoryFlags::WXK_CATEGORY_TAB, keyCategory.Contains(KeyCategory::Tab));
		result.Add(wxKeyCategoryFlags::WXK_CATEGORY_CUT, keyCategory.Contains(KeyCategory::Delete));

		return result;
	}
}
