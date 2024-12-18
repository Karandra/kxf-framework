#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/Common.h"

namespace kxf
{
	enum class RPCExchangeFlag: uint32_t
	{
		None = 0,

		GlobalSession = 1 << 0
	};
	kxf_FlagSet_Declare(RPCExchangeFlag);
}
