#include "kxf-pch.h"
#include "BasicEvent.h"
#include "kxf/DateTime/TimeClock.h"

namespace kxf
{
	// IEventInternal
	bool BasicEvent::OnStartProcess(const EventID& eventID, const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, bool isAsync) noexcept
	{
		if (!m_PrivateState.Contains(EventPrivateState::Started))
		{
			m_PrivateState.Add(EventPrivateState::Started);
			m_PrivateState.Mod(EventPrivateState::Async, isAsync);

			m_EventID = eventID;
			m_UniqueID = uuid;
			m_Timestamp = TimeSpan::Now(SteadyClock());
			m_ProcessFlags = flags;

			return true;
		}
		return false;
	}

	std::unique_ptr<kxf::IEvent> BasicEvent::WaitProcessed() noexcept
	{
		if (!m_WaitInfo.HasWaitInfo())
		{
			m_PrivateState.Add(EventPrivateState::Waitable);
			return m_WaitInfo.WaitProcessed();
		}
		return nullptr;
	}
	void BasicEvent::SignalProcessed(std::unique_ptr<IEvent> event) noexcept
	{
		if (m_PrivateState.Contains(EventPrivateState::Waitable))
		{
			m_WaitInfo.SignalProcessed(std::move(event));
		}
	}
}
