#pragma once
#include "WidgetEvent.h"

namespace kxf
{
	class KXF_API WidgetScrollEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(WidgetScrollEvent, Top);
			kxf_EVENT_MEMBER(WidgetScrollEvent, Bottom);
			kxf_EVENT_MEMBER(WidgetScrollEvent, LineUp);
			kxf_EVENT_MEMBER(WidgetScrollEvent, LineDown);
			kxf_EVENT_MEMBER(WidgetScrollEvent, PageUp);
			kxf_EVENT_MEMBER(WidgetScrollEvent, PageDown);

			kxf_EVENT_MEMBER(WidgetScrollEvent, ThumbTrack);
			kxf_EVENT_MEMBER(WidgetScrollEvent, ThumbRelease);

		private:
			Orientation m_Orientation = Orientation::None;
			int m_Value = -1;

		public:
			WidgetScrollEvent() noexcept = default;
			WidgetScrollEvent(IWidget& widget, Orientation orientation, int value) noexcept
				:WidgetEvent(widget), m_Orientation(orientation), m_Value(value)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetScrollEvent>(std::move(*this));
			}

		public:
			// WidgetScrollEvent
			Orientation GetOrientation() const noexcept
			{
				return m_Orientation;
			}
			int GetValue() const noexcept
			{
				return m_Value;
			}
	};
}
