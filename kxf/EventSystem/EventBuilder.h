#pragma once
#include "Event.h"
#include "kxf/Core/OptionalPtr.h"

namespace kxf
{
	class IEvtHandler;
}

namespace kxf::EventSystem::Private
{
	std::shared_ptr<IEvtHandler> QueryEvtHandler(IEvtHandler& evtHandler) noexcept;

	template<class T, class TEvent>
	class EventBuilderCRTP
	{
		private:
			T& Self()
			{
				return static_cast<T&>(*this);
			}

		public:
			~EventBuilderCRTP()
			{
				if (!Self().m_IsSent)
				{
					Self().Process();
				}
			}

		public:
			template<class TFunc>
			T& Setup(TFunc&& func)
			{
				std::invoke(func, static_cast<TEvent&>(*Self().m_Event));
				return Self();
			}

			T& SetSourceToSelf()
			{
				Self().m_Event->SetEventSource(QueryEvtHandler(*Self().m_EvtHandler));
				return Self();
			}
			T& SetSource(IEvtHandler& source)
			{
				Self().m_Event->SetEventSource(QueryEvtHandler(source));
				return Self();
			}

		public:
			T& Process(FlagSet<ProcessEventFlag> flags = {})
			{
				Self().SendEvent({}, flags);
				return Self();
			}
			T& ProcessUnique(UniversallyUniqueID uuid, FlagSet<ProcessEventFlag> flags = {})
			{
				Self().SendEvent(std::move(uuid), flags);
				return Self();
			}
	};
}

namespace kxf::EventSystem
{
	class KXF_API EventBuilderBase
	{
		protected:
			IEvtHandler* m_EvtHandler = nullptr;
			optional_ptr<IEvent> m_Event;
			EventID m_EventID;

			bool m_IsSent = false;
			bool m_IsSkipped = false;
			bool m_IsAllowed = true;
			bool m_IsProcessed = false;

		private:
			void Move(EventBuilderBase&& other) noexcept;

		protected:
			void SendEvent(UniversallyUniqueID uuid, FlagSet<ProcessEventFlag> flags);

		private:
			EventBuilderBase() = default;

		protected:
			EventBuilderBase(IEvtHandler& evtHandler, std::unique_ptr<IEvent> event, const EventID& eventID = {}) noexcept
				:m_EvtHandler(&evtHandler), m_Event(std::move(event)), m_EventID(eventID)
			{
			}
			EventBuilderBase(IEvtHandler& evtHandler, IEvent& event, const EventID& eventID = {}) noexcept
				:m_EvtHandler(&evtHandler), m_Event(event), m_EventID(eventID)
			{
			}
			EventBuilderBase(EventBuilderBase&& other) noexcept
			{
				Move(std::move(other));
			}
			EventBuilderBase(const EventBuilderBase&) = delete;
			virtual ~EventBuilderBase() = default;

		public:
			bool IsAsync() const noexcept
			{
				return m_Event.is_owned();
			}
			bool IsSkipped() const noexcept
			{
				return m_IsSkipped;
			}
			bool IsAllowed() const noexcept
			{
				return m_IsAllowed;
			}
			bool IsProcessed() const noexcept
			{
				return m_IsProcessed;
			}

		public:
			EventBuilderBase& operator=(EventBuilderBase&& other) noexcept
			{
				Move(std::move(other));
				return *this;
			}
			EventBuilderBase& operator=(const EventBuilderBase&) = delete;
	};
}

namespace kxf::EventSystem
{
	template<class TEvent>
	class DirectEventBuilder: public EventBuilderBase, public Private::EventBuilderCRTP<DirectEventBuilder<TEvent>, TEvent>
	{
		friend class Private::EventBuilderCRTP<DirectEventBuilder, TEvent>;

		private:
			TEvent m_EventInstance;

		public:
			DirectEventBuilder(IEvtHandler& evtHandler, TEvent event, const EventID& eventID)
				:EventBuilderBase(evtHandler, m_EventInstance, eventID), m_EventInstance(std::move(event))
			{
			}
			DirectEventBuilder(DirectEventBuilder&& other)
				:EventBuilderBase(std::move(other))
			{
			}
			DirectEventBuilder(const DirectEventBuilder&) = delete;

		public:
			TEvent& GetEvent() noexcept
			{
				return m_EventInstance;
			}

		public:
			DirectEventBuilder& operator=(DirectEventBuilder&& other) noexcept
			{
				static_cast<EventBuilderBase&>(*this) = std::move(other);
				return *this;
			}
			DirectEventBuilder& operator=(const DirectEventBuilder&) = delete;
	};

	template<class TEvent>
	class QueuedEventBuilder: public EventBuilderBase, public Private::EventBuilderCRTP<QueuedEventBuilder<TEvent>, TEvent>
	{
		friend class Private::EventBuilderCRTP<QueuedEventBuilder, TEvent>;

		public:
			QueuedEventBuilder(IEvtHandler& evtHandler, std::unique_ptr<TEvent> event, const EventID& eventID)
				:EventBuilderBase(evtHandler, std::move(event), eventID)
			{
			}
			QueuedEventBuilder(QueuedEventBuilder&& other)
				:EventBuilderBase(std::move(other))
			{
			}
			QueuedEventBuilder(const QueuedEventBuilder&) = delete;

		public:
			TEvent* GetEvent() noexcept
			{
				return m_Event.get();
			}

		public:
			QueuedEventBuilder& operator=(QueuedEventBuilder&& other) noexcept
			{
				static_cast<EventBuilderBase&>(*this) = std::move(other);
				return *this;
			}
			QueuedEventBuilder& operator=(const QueuedEventBuilder&) = delete;
	};
}
