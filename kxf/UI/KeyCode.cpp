#include "kxf-pch.h"
#include "KeyCode.h"

namespace kxf::Private
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
