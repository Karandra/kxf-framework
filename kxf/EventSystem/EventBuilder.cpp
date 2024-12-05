#include "KxfPCH.h"
#include "EventBuilder.h"
#include "IEvtHandler.h"
#include "EvtHandlerAccessor.h"

namespace kxf::EventSystem::Private
{
	std::shared_ptr<IEvtHandler> QueryEvtHandler(IEvtHandler& evtHandler) noexcept
	{
		return evtHandler.QueryInterface<IEvtHandler>();
	}
}

namespace kxf::EventSystem
{
	void EventBuilderBase::Move(EventBuilderBase&& other) noexcept
	{
		EventBuilderBase null;

		m_EvtHandler = std::exchange(other.m_EvtHandler, null.m_EvtHandler);
		m_Event = std::move(other.m_Event);
		m_EventID = std::move(other.m_EventID);
		m_IsSent = std::exchange(other.m_IsSent, null.m_IsSent);
		m_IsSkipped = std::exchange(other.m_IsSkipped, null.m_IsSkipped);
		m_IsAllowed = std::exchange(other.m_IsAllowed, null.m_IsAllowed);
	}
	void EventBuilderBase::SendEvent(UniversallyUniqueID uuid, FlagSet<ProcessEventFlag> flags)
	{
		if (!m_IsSent)
		{
			if (m_Event.is_owned())
			{
				m_IsSent = true;
				m_IsSkipped = false;
				m_IsAllowed = true;

				m_Event = EvtHandlerAccessor(*m_EvtHandler).DoQueueEvent(m_Event.get_unique(), m_EventID, std::move(uuid), flags);
			}
			else
			{
				m_IsSent = true;
				m_IsProcessed = EvtHandlerAccessor(*m_EvtHandler).DoProcessEvent(*m_Event, m_EventID, std::move(uuid), flags);

				m_IsSkipped = m_Event->IsSkipped();
				m_IsAllowed = m_Event->IsAllowed();
			}
		}
	}
}
