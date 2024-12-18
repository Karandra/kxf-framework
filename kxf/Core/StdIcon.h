#pragma once
#include "Common.h"

namespace kxf
{
	enum class StdIcon: uint32_t
	{
		None = 0,

		// Icons
		Error = 1 << 0,
		Warning = 1 << 1,
		Question = 1 << 2,
		Information = 1 << 3,
		Authentication = 1 << 4,

		// Modifiers
		Shield = 1 << 5,
		Success = 1 << 6,
		Neutral = 1 << 7,
		Header = 1 << 8,
	};
	kxf_FlagSet_Declare(StdIcon);
}
