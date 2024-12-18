#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IEvent;
}

namespace kxf
{
	class KXF_API IEventFilter: public RTTI::Interface<IEventFilter>
	{
		kxf_RTTI_DeclareIID(IEventFilter, {0xc11e0406, 0xba4c, 0x4ba6, {0xa6, 0x87, 0x54, 0xb0, 0x80, 0x20, 0x91, 0x50}});

		public:
			enum class Result
			{
				// Process event as usual
				Skip = -1,

				// Don't process the event normally at all
				Ignore = 0,

				// Event was already handled, don't process it normally
				Processed = 1
			};

		public:
			virtual ~IEventFilter() = default;

		public:
			virtual Result FilterEvent(IEvent& event) = 0;
	};
}
