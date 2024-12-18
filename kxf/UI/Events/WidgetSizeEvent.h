#pragma once
#include "WidgetEvent.h"

namespace kxf
{
	class KXF_API WidgetSizeEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(WidgetSizeEvent, Size);

		private:
			Rect m_Rect;

		public:
			WidgetSizeEvent() noexcept = default;
			WidgetSizeEvent(IWidget& widget, const Rect& rect) noexcept
				:WidgetEvent(widget), m_Rect(rect)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetSizeEvent>(std::move(*this));
			}

		public:
			// WidgetSizeEvent
			Rect GetRect() const noexcept
			{
				return m_Rect;
			}
			Size GetSize() const noexcept
			{
				return m_Rect.GetSize();
			}
	};
}
