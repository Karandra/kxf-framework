#pragma once
#include "WidgetEvent.h"
#include "../ICheckWidget.h"

namespace kxf
{
	class KXF_API WidgetCheckEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(WidgetCheckEvent, StateChanged);

		private:
			CheckWidgetValue m_Value = CheckWidgetValue::Unchecked;

		public:
			WidgetCheckEvent() noexcept = default;
			WidgetCheckEvent(ICheckWidget& widget, bool isChecked) noexcept
				:WidgetEvent(widget), m_Value(isChecked ? CheckWidgetValue::Checked : CheckWidgetValue::Unchecked)
			{
			}
			WidgetCheckEvent(ICheckWidget& widget, CheckWidgetValue value) noexcept
				:WidgetEvent(widget), m_Value(value)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetCheckEvent>(std::move(*this));
			}

			// WidgetCheckEvent
			std::shared_ptr<ICheckWidget> GetCheckWidget() const noexcept
			{
				return WidgetEvent::GetWidget()->QueryInterface<ICheckWidget>();
			}

			CheckWidgetValue GetValue() const noexcept
			{
				return m_Value;
			}
			bool IsChecked() const
			{
				return m_Value == CheckWidgetValue::Checked;
			}
	};
}
