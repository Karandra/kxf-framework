#include "kxf-pch.h"
#include "GenericTimer.h"
#include <wx/timer.h>

namespace kxf::Private
{
	class GenericTimerImpl final: public wxTimer
	{
		private:
			GenericTimer& m_Owner;

		private:
			void OnTimer(wxTimerEvent& eventWx)
			{
				m_Owner.m_EvtHandler->ProcessEvent(TimerEvent::EvtNotify, RTTI::assume_non_owned(m_Owner), TimeSpan::Milliseconds(eventWx.GetInterval()), GetId());
			}

		public:
			GenericTimerImpl(GenericTimer& owner)
				:m_Owner(owner)
			{
			}
			GenericTimerImpl(GenericTimer& owner, int id)
				:wxTimer(this, id), m_Owner(owner)
			{
				Bind(wxEVT_TIMER, &GenericTimerImpl::OnTimer, this);
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
	void GenericTimer::OnNotify()
	{
	}

	GenericTimer::GenericTimer()
		:m_Timer(std::make_unique<Private::GenericTimerImpl>(*this))
	{
	}
	GenericTimer::GenericTimer(IEvtHandler* evtHandler, int id)
		:m_EvtHandler(evtHandler), m_Timer(std::make_unique<Private::GenericTimerImpl>(*this, id))
	{
	}
	GenericTimer::GenericTimer(GenericTimer&&) = default;
	GenericTimer::~GenericTimer() = default;

	int GenericTimer::GetID() const noexcept
	{
		return m_Timer->GetId();
	}
	bool GenericTimer::IsRunning() const noexcept
	{
		return m_Timer->IsRunning();
	}
	TimeSpan GenericTimer::GetInterval() const noexcept
	{
		return TimeSpan::Milliseconds(m_Timer->GetInterval());
	}

	IEvtHandler* GenericTimer::GetEvtHandler() const noexcept
	{
		return m_EvtHandler;
	}
	void GenericTimer::SetEvtHandler(IEvtHandler& evtHandler, int id) noexcept
	{
		m_Timer->SetOwner(m_Timer.get(), id);
	}

	void GenericTimer::Start(TimeSpan interval, FlagSet<TimerFlag> flags) noexcept
	{
		m_Timer->Start(interval.IsNegative() ? -1 : interval.GetMilliseconds(), flags.Contains(TimerFlag::Continuous) ? wxTIMER_CONTINUOUS : wxTIMER_ONE_SHOT);
	}
	void GenericTimer::Stop() noexcept
	{
		m_Timer->Stop();
	}

	GenericTimer& GenericTimer::operator=(GenericTimer&&) = default;
}
