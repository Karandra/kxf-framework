#pragma once
#include "kxf/Core/FlagSet.h"
#include "kxf/Core/StdID.h"
#include "kxf/Core/StdIcon.h"
#include "kxf/Core/StdButton.h"
#include "kxf/Core/KeyCode.h"
enum wxStandardID;
enum wxKeyCode;
enum wxKeyModifier;
enum wxKeyCategoryFlags;

namespace kxf::wxWidgets
{
	StdID MapStdID(wxStandardID id) noexcept;
	wxStandardID MapStdID(StdID id) noexcept;

	FlagSet<StdIcon> MapStdIcon(uint32_t stdWxIcon) noexcept;
	FlagSet<uint32_t> MapStdIcon(FlagSet<StdIcon> stdIcon) noexcept;

	StdButton MapStdButton(uint32_t stdWxButton) noexcept;
	uint32_t MapStdButton(StdButton stdButton) noexcept;
}

namespace kxf::wxWidgets
{
	wxKeyCode MapKeyCode(KeyCode code) noexcept;
	KeyCode MapKeyCode(wxKeyCode code) noexcept;

	FlagSet<KeyModifier> MapKeyModifier(FlagSet<wxKeyModifier> keyModifier) noexcept;
	FlagSet<wxKeyModifier> MapKeyModifier(FlagSet<KeyModifier> keyModifier) noexcept;

	FlagSet<KeyCategory> MapKeyCategory(FlagSet<wxKeyCategoryFlags> keyCategory) noexcept;
	FlagSet<wxKeyCategoryFlags> MapKeyCategory(FlagSet<KeyCategory> keyCategory) noexcept;
}
