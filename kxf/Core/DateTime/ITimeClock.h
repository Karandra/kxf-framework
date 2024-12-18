#pragma once
#include "Common.h"
#include "TimeSpan.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class KXF_API ITimeClock: public RTTI::Interface<ITimeClock>
	{
		kxf_RTTI_DeclareIID(ITimeClock, {0x19e8c288, 0x4b5b, 0x4c8b, {0x95, 0x77, 0x47, 0xfa, 0x4f, 0x2e, 0x7d, 0xbc}});

		public:
			virtual TimeSpan Now() const noexcept = 0;
	};
}
