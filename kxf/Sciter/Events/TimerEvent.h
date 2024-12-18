#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class KXF_API TimerEvent: public SciterEvent
	{
		public:
			kxf_EVENT_MEMBER(TimerEvent, Timer);

		protected:
			uintptr_t m_TimerID = 0;

		public:
			TimerEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<TimerEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const noexcept override
			{
				return EventCategory::Timer;
			}

			// SciterEvent
			uintptr_t GetTimerID() const
			{
				return m_TimerID;
			}
			void SetTimerID(uintptr_t value)
			{
				m_TimerID = value;
			}
	};
}

namespace kxf::Sciter
{
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(TimerEvent, Timer);
}
