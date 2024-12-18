#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class KXF_API SizeEvent: public SciterEvent
	{
		public:
			kxf_EVENT_MEMBER(SizeEvent, Size);

		public:
			SizeEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<SizeEvent>(std::move(*this));
			}
	};
}

namespace kxf::Sciter
{
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(SizeEvent, Size);
}
