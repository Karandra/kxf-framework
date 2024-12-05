#pragma once
#include "Common.h"
#include "IEvent.h"
#include "Private/EventWaitInfo.h"
#include "kxf/Core/DateTime.h"

namespace kxf::EventSystem
{
	enum class EventPublicState: uint32_t
	{
		None = 0,
		Skipped = 1 << 0
	};
	enum class EventPrivateState: uint32_t
	{
		None = 0,

		// Set after a call to 'OnStartProcess'.
		Started = 1 << 0,

		// Indicates that the event is executed asynchronously. On other words it was sent from 'IEvtHandler::Queue[Unique][Event|Signal]'.
		Async = 1 << 1,

		// Set as soon as 'WasQueued' is called for the first time when event is going to be queued from 'DoProcessEvent'.
		ReQueued = 1 << 2,

		// Set if the event is invoked with thread-blocking semantics.
		Waitable = 1 << 3,

		// Set after 'DoProcessEvent' was called at least once for this event.
		ProcessedOnce = 1 << 4,

		// Can be set to indicate that the event will be passed to another handler if it's not processed in this one.
		WillBeProcessedAgain = 1 << 5
	};
}
namespace kxf
{
	KxFlagSet_Declare(EventSystem::EventPublicState);
	KxFlagSet_Declare(EventSystem::EventPrivateState);
}

namespace kxf
{
	class KX_API BasicEvent: public RTTI::Implementation<BasicEvent, IEvent>, private IEventInternal
	{
		friend class IObject;

		private:
			using EventPublicState = EventSystem::EventPublicState;
			using EventPrivateState = EventSystem::EventPrivateState;

		private:
			EventID m_EventID;
			std::shared_ptr<IEvtHandler> m_EventSource;
			UniversallyUniqueID m_UniqueID;
			TimeSpan m_Timestamp;

			FlagSet<EventPublicState> m_PublicState;
			mutable FlagSet<EventPrivateState> m_PrivateState;
			FlagSet<ProcessEventFlag> m_ProcessFlags;
			EventSystem::Private::EventWaitInfo m_WaitInfo;

		private:
			bool TestAndSetPrivateState(EventPrivateState flag) const noexcept
			{
				if (!m_PrivateState.Contains(flag))
				{
					m_PrivateState.Add(flag);
					return false;
				}
				return true;
			}

			// IEventInternal
			bool IsAsync() const noexcept override
			{
				return m_PrivateState.Contains(EventPrivateState::Async);
			}
			bool WasReQueued() const noexcept override
			{
				return TestAndSetPrivateState(EventPrivateState::ReQueued);
			}
			bool WasProcessed() const noexcept override
			{
				return TestAndSetPrivateState(EventPrivateState::ProcessedOnce);
			}
			bool WillBeProcessedAgain() const noexcept override
			{
				return TestAndSetPrivateState(EventPrivateState::WillBeProcessedAgain);
			}

			bool OnStartProcess(const EventID& eventID, const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, bool isAsync) noexcept override
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
			FlagSet<ProcessEventFlag> GetProcessFlags() const noexcept override
			{
				return m_ProcessFlags;
			}

			std::unique_ptr<IEvent> WaitProcessed() noexcept override
			{
				if (!m_WaitInfo.HasWaitInfo())
				{
					m_PrivateState.Add(EventPrivateState::Waitable);
					return m_WaitInfo.WaitProcessed();
				}
				return nullptr;
			}
			void SignalProcessed(std::unique_ptr<IEvent> event) noexcept override
			{
				if (m_PrivateState.Contains(EventPrivateState::Waitable))
				{
					m_WaitInfo.SignalProcessed(std::move(event));
				}
			}

			void PutWaitResult(std::unique_ptr<IEvent> event) noexcept override
			{
				m_WaitInfo.PutWaitResult(std::move(event));
			}
			std::unique_ptr<IEvent> GetWaitResult() noexcept override
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
			BasicEvent() = default;
			BasicEvent(const BasicEvent&) noexcept = default;
			BasicEvent(BasicEvent&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<BasicEvent>(std::move(*this));
			}

			EventID GetEventID() const noexcept override
			{
				return m_EventID;
			}
			TimeSpan GetTimestamp() const noexcept override
			{
				return m_Timestamp;
			}
			UniversallyUniqueID GetUniqueID() const noexcept override
			{
				return m_UniqueID;
			}
			FlagSet<EventCategory> GetEventCategory() const noexcept override
			{
				return EventCategory::None;
			}

			std::shared_ptr<IEvtHandler> GetEventSource() const noexcept override
			{
				return m_EventSource;
			}
			void SetEventSource(std::shared_ptr<IEvtHandler> evtHandler) noexcept override
			{
				m_EventSource = std::move(evtHandler);
			}
			
			bool IsSkipped() const noexcept override
			{
				return m_PublicState.Contains(EventPublicState::Skipped);
			}
			void Skip(bool skip = true) noexcept override
			{
				m_PublicState.Mod(EventPublicState::Skipped, skip);
			}

			bool IsAllowed() const noexcept override
			{
				return true;
			}
			void Allow(bool allow = true) noexcept override
			{
			}

		public:
			BasicEvent& operator=(const BasicEvent&) noexcept = default;
			BasicEvent& operator=(BasicEvent&& other) noexcept
			{
				m_EventID = std::move(other.m_EventID);
				m_EventSource = std::move(other.m_EventSource);
				m_UniqueID = std::move(other.m_UniqueID);
				m_Timestamp = std::move(other.m_Timestamp);

				m_PublicState = std::exchange(other.m_PublicState, EventPublicState::None);
				m_PrivateState = std::exchange(other.m_PrivateState, EventPrivateState::None);
				m_WaitInfo = std::move(other.m_WaitInfo);

				return *this;
			}
	};
}
