#pragma once
#include "Common.h"
#include "TimeSpan.h"
#include "kxf/EventSystem/BasicEvent.h"

namespace kxf
{
	class ITimer;
}

namespace kxf
{
	class KXF_API TimerEvent: public BasicEvent
	{
		public:
			kxf_EVENT_MEMBER(TimerEvent, Notify);

		private:
			std::shared_ptr<ITimer> m_Timer;
			TimeSpan m_Interval;
			int m_ID = -1;

		public:
			TimerEvent() = default;
			TimerEvent(TimeSpan interval, int id = -1)
				:m_Interval(interval), m_ID(id)
			{
			}
			TimerEvent(std::shared_ptr<ITimer> timer, TimeSpan interval, int id = -1)
				:m_Timer(std::move(timer)), m_Interval(interval), m_ID(id)
			{
			}

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<TimerEvent>(std::move(*this));
			}

			std::shared_ptr<ITimer> GetTimer() const noexcept
			{
				return m_Timer;
			}
			TimeSpan GetInterval() const noexcept
			{
				return m_Interval;
			}
			int GetID() const noexcept
			{
				return m_ID;
			}
	};
}
