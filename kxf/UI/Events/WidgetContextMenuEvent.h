#pragma once
#include "WidgetEvent.h"

namespace kxf
{
	class KXF_API WidgetContextMenuEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(WidgetContextMenuEvent, Show);

		private:
			Point m_Position;

		public:
			WidgetContextMenuEvent() noexcept = default;
			WidgetContextMenuEvent(IWidget& widget, const Point& pos) noexcept
				:WidgetEvent(widget), m_Position(pos)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetContextMenuEvent>(std::move(*this));
			}

		public:
			// WidgetContextMenuEvent
			Point GetPosition() const noexcept
			{
				return m_Position;
			}
	};
}
