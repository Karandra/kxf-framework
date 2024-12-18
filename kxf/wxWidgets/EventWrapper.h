#pragma once
#include "Common.h"
#include "IWithEvent.h"
#include "kxf/Core/OptionalPtr.h"
#include "kxf/DateTime/TimeSpan.h"
#include "kxf/DateTime/TimeClock.h"
#include "kxf/EventSystem/IEvent.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/EventSystem/Private/EventWaitInfo.h"
#include <wx/object.h>
#include <wx/event.h>

namespace kxf::wxWidgets
{
	class KXF_API EventWrapper: public RTTI::Implementation<EventWrapper, IEvent, IWithEvent>, private IEventInternal
	{
		private:
			optional_ptr<wxEvent> m_Event;
			std::shared_ptr<IEvtHandler> m_EvtHandler;

			bool m_ProcessStarted = false;
			bool m_IsAsync = false;
			mutable bool m_WasReQueued = false;
			UniversallyUniqueID m_UniqueID;
			FlagSet<ProcessEventFlag> m_ProcessFlags;
			EventSystem::Private::EventWaitInfo m_WaitInfo;

		private:
			// IEventInternal
			bool IsAsync() const override
			{
				return m_IsAsync;
			}
			bool WasReQueued() const override
			{
				if (!m_WasReQueued)
				{
					m_WasReQueued = true;
					return false;
				}
				return true;
			}
			bool WasProcessed() const override
			{
				return m_Event->WasProcessed();
			}
			bool WillBeProcessedAgain() const override
			{
				return m_Event->WillBeProcessedAgain();
			}

			bool OnStartProcess(const EventID& eventID, const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, bool isAsync) override
			{
				if (!m_ProcessStarted)
				{
					m_Event->SetEventType(eventID.AsInt());
					m_UniqueID = std::move(uuid);
					m_Event->SetTimestamp(TimeSpan::Now(SteadyClock()).GetMilliseconds());
					m_ProcessFlags = flags;
					m_IsAsync = isAsync;

					return true;
				}
				return false;
			}
			FlagSet<ProcessEventFlag> GetProcessFlags() const override
			{
				return m_ProcessFlags;
			}

			std::unique_ptr<IEvent> WaitProcessed()
			{
				return m_WaitInfo.WaitProcessed();
			}
			void SignalProcessed(std::unique_ptr<IEvent> event)
			{
				m_WaitInfo.SignalProcessed(std::move(event));
			}

			void PutWaitResult(std::unique_ptr<IEvent> event) override
			{
				m_WaitInfo.PutWaitResult(std::move(event));
			}
			std::unique_ptr<IEvent> GetWaitResult()
			{
				return m_WaitInfo.GetWaitResult();
			}

		protected:
			// IObject
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				if (iid.IsOfType<IEventInternal>())
				{
					return *static_cast<IEventInternal*>(this);
				}
				return TBaseClass::DoQueryInterface(iid);
			}

		public:
			EventWrapper(wxEvent& evtHandler)
				:m_Event(evtHandler)
			{
			}
			EventWrapper(std::unique_ptr<wxEvent> evtHandler)
				:m_Event(std::move(evtHandler))
			{
			}
			EventWrapper(EventWrapper&& other) noexcept = default;
			EventWrapper(const EventWrapper&) = delete;

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<EventWrapper>(std::move(*this));
			}

			EventID GetEventID() const override
			{
				return m_Event->GetEventType();
			}
			TimeSpan GetTimestamp() const override
			{
				return TimeSpan::Milliseconds(m_Event->GetTimestamp());
			}
			UniversallyUniqueID GetUniqueID() const override
			{
				return m_UniqueID;
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				return static_cast<EventCategory>(m_Event->GetEventCategory());
			}

			std::shared_ptr<IEvtHandler> GetEventSource() const override
			{
				if (m_EvtHandler)
				{
					return m_EvtHandler;
				}
				else if (auto evtHandler = dynamic_cast<IEvtHandler*>(m_Event->GetEventObject()))
				{
					return evtHandler->QueryInterface<IEvtHandler>();
				}
				return nullptr;
			}
			void SetEventSource(std::shared_ptr<IEvtHandler> evtHandler) override
			{
				m_EvtHandler = evtHandler;
				m_Event->SetEventObject(dynamic_cast<wxObject*>(evtHandler.get()));
			}
			
			bool IsSkipped() const override
			{
				return m_Event->GetSkipped();
			}
			void Skip(bool skip = true) override
			{
				m_Event->Skip(skip);
			}

			bool IsAllowed() const override
			{
				if (m_Event->IsKindOf(wxCLASSINFO(wxNotifyEvent)))
				{
					return static_cast<const wxNotifyEvent&>(*m_Event).IsAllowed();
				}
				return true;
			}
			void Allow(bool allow = true) override
			{
				if (m_Event->IsKindOf(wxCLASSINFO(wxNotifyEvent)))
				{
					wxNotifyEvent& notifyEvent = static_cast<wxNotifyEvent&>(*m_Event);
					if (allow)
					{
						notifyEvent.Allow();
					}
					else
					{
						notifyEvent.Veto();
					}
				}
			}

			// IWithEvent
			wxEvent& GetEvent() override
			{
				return *m_Event;
			}

		public:
			EventWrapper& operator=(EventWrapper&& other) noexcept = default;
			EventWrapper& operator=(const EventWrapper&) = delete;
	};
}
