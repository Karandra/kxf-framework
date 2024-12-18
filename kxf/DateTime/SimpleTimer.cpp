#include "kxf-pch.h"
#include "SimpleTimer.h"
#include <wx/timer.h>

namespace kxf::Private
{
	class SimpleTimerImpl final: public wxTimer
	{
		private:
			SimpleTimer& m_Owner;

		private:
			void OnTimer(wxTimerEvent& eventWx)
			{
				m_Owner.m_EvtHandler->ProcessEvent(TimerEvent::EvtNotify, RTTI::assume_non_owned(m_Owner), TimeSpan::Milliseconds(eventWx.GetInterval()), GetId());
			}

		public:
			SimpleTimerImpl(SimpleTimer& owner)
				:m_Owner(owner)
			{
			}
			SimpleTimerImpl(SimpleTimer& owner, int id)
				:wxTimer(this, id), m_Owner(owner)
			{
				Bind(wxEVT_TIMER, &SimpleTimerImpl::OnTimer, this);
			}

		public:
			void Notify() override
			{
				m_Owner.OnNotify();
			}
	};
}

namespace kxf
{
	void SimpleTimer::OnNotify()
	{
	}

	SimpleTimer::SimpleTimer() = default;
	SimpleTimer::SimpleTimer(std::shared_ptr<IEvtHandler> evtHandler, int id)
		:m_EvtHandler(std::move(evtHandler)), m_Timer(std::make_unique<Private::SimpleTimerImpl>(*this, id))
	{
	}
	SimpleTimer::SimpleTimer(SimpleTimer&&) = default;
	SimpleTimer::~SimpleTimer() = default;

	int SimpleTimer::GetID() const noexcept
	{
		if (m_Timer)
		{
			return m_Timer->GetId();
		}
		return -1;
	}
	bool SimpleTimer::IsRunning() const noexcept
	{
		return m_Timer && m_Timer->IsRunning();
	}
	TimeSpan SimpleTimer::GetInterval() const noexcept
	{
		if (m_Timer)
		{
			return TimeSpan::Milliseconds(m_Timer->GetInterval());
		}
		return {};
	}

	std::shared_ptr<IEvtHandler> SimpleTimer::GetEvtHandler() const noexcept
	{
		return m_EvtHandler;
	}
	void SimpleTimer::SetEvtHandler(std::shared_ptr<IEvtHandler> evtHandler, int id) noexcept
	{
		if (!m_Timer)
		{
			m_Timer = std::make_unique<Private::SimpleTimerImpl>(*this);
		}

		m_EvtHandler = std::move(evtHandler);
		m_Timer->SetOwner(m_Timer.get(), id);
	}

	void SimpleTimer::Start(TimeSpan interval, FlagSet<TimerFlag> flags) noexcept
	{
		if (!m_Timer)
		{
			m_Timer = std::make_unique<Private::SimpleTimerImpl>(*this);
		}
		m_Timer->Start(interval.IsNegative() ? -1 : interval.GetMilliseconds(), flags.Contains(TimerFlag::Continuous) ? wxTIMER_CONTINUOUS : wxTIMER_ONE_SHOT);
	}
	void SimpleTimer::Stop() noexcept
	{
		if (m_Timer)
		{
			m_Timer->Stop();
		}
	}

	SimpleTimer& SimpleTimer::operator=(SimpleTimer&&) = default;
}
