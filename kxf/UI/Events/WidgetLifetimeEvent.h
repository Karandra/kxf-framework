#pragma once
#include "WidgetEvent.h"

namespace kxf
{
	class KXF_API WidgetLifetimeEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(WidgetLifetimeEvent, Created);
			kxf_EVENT_MEMBER(WidgetLifetimeEvent, Destroyed);

		public:
			WidgetLifetimeEvent() noexcept = default;
			WidgetLifetimeEvent(IWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetLifetimeEvent>(std::move(*this));
			}
	};
}
