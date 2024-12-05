#pragma once
#include "Common.h"

namespace kxf
{
	class KX_API WidgetEvent: public RTTI::DynamicImplementation<WidgetEvent, BasicEvent, IWidgetEvent>
	{
		KxRTTI_DeclareIID(WidgetEvent, {0xa7e0c7a2, 0xe2cf, 0x4b11, {0xa4, 0x1b, 0x9, 0x77, 0x47, 0x6a, 0xa7, 0x70}});

		private:
			std::shared_ptr<IWidget> m_Widget;
			WidgetID m_WidgetID;
			size_t m_PropagationLevel = PropagationLevel::Max;

		public:
			WidgetEvent() noexcept = default;
			WidgetEvent(IWidget& widget) noexcept;
			WidgetEvent(const WidgetEvent&) = default;
			WidgetEvent(WidgetEvent&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const noexcept override
			{
				return EventCategory::UI;
			}

			// IWidgetEvent
			std::shared_ptr<IWidget> GetWidget() const noexcept override
			{
				return m_Widget;
			}
			void SetWidget(std::shared_ptr<IWidget> widget) noexcept override
			{
				m_Widget = std::move(widget);
			}

			WidgetID GetWidgetID() const noexcept override;
			void SetWidgetID(WidgetID id) noexcept override
			{
				m_WidgetID = id;
			}

			size_t StopPropagation() noexcept override
			{
				return std::exchange(m_PropagationLevel, PropagationLevel::None);
			}
			void ResumePropagation(size_t level) noexcept override
			{
				m_PropagationLevel = level;
			}
			bool ShouldPropagate() const noexcept override
			{
				return m_PropagationLevel != PropagationLevel::None;
			}

		public:
			WidgetEvent& operator=(const WidgetEvent&) = default;
			WidgetEvent& operator=(WidgetEvent&& other) noexcept
			{
				static_cast<BasicEvent&>(*this) = std::move(other);

				m_Widget = std::move(other.m_Widget);
				m_WidgetID = std::exchange(other.m_WidgetID, StdID::None);
				m_PropagationLevel = std::exchange(other.m_PropagationLevel, PropagationLevel::Max);

				return *this;
			}
	};
}
