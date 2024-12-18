#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class KXF_API FocusEvent: public SciterEvent
	{
		public:
			kxf_EVENT_MEMBER(FocusEvent, SetFocus);
			kxf_EVENT_MEMBER(FocusEvent, KillFocus);
			kxf_EVENT_MEMBER(FocusEvent, ContainerSetFocus);
			kxf_EVENT_MEMBER(FocusEvent, ContainerKillFocus);
			kxf_EVENT_MEMBER(FocusEvent, RequestFocus);
			kxf_EVENT_MEMBER(FocusEvent, RequestFocusAdvance);

		public:
			FocusEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<FocusEvent>(std::move(*this));
			}
	};
}

namespace kxf::Sciter
{
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, SetFocus);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, KillFocus);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, ContainerSetFocus);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, ContainerKillFocus);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, RequestFocus);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, RequestFocusAdvance);
}
