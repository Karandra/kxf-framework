#pragma once
#include "Common.h"

namespace kxf
{
	enum class StdButton: uint32_t
	{
		WX_LAST_STD = 0x00020000,
		None = 0,

		// Standard buttons
		OK = 0x00000004,
		Cancel = 0x00000010,
		Yes = 0x00000002,
		No = 0x00000008,
		Apply = 0x00000020,
		Close = 0x00000040,
		Help = 0x00001000,
		Forward = 0x00002000,
		Backward = 0x00004000,
		Reset = 0x00008000,
		More = 0x00010000,
		Setup = 0x00020000,

		// Extra buttons
		Abort = WX_LAST_STD << 1,
		Retry = WX_LAST_STD << 2,
		Save = WX_LAST_STD << 3,
		DoNotSave = WX_LAST_STD << 4,
		ContextHelp = WX_LAST_STD << 5,
	};
	kxf_FlagSet_Declare(StdButton);
}
