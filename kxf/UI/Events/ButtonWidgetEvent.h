#pragma once
#include "WidgetEvent.h"
#include "../IButtonWidget.h"

namespace kxf
{
	class KXF_API ButtonWidgetEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(ButtonWidgetEvent, Click);
			kxf_EVENT_MEMBER(ButtonWidgetEvent, Dropdown);

		public:
			ButtonWidgetEvent(IButtonWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ButtonWidgetEvent>(std::move(*this));
			}

			// ButtonWidgetEvent
			std::shared_ptr<IButtonWidget> GetButtonWidget() const noexcept
			{
				return GetWidget()->QueryInterface<IButtonWidget>();
			}
	};
}
