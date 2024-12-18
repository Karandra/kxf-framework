#pragma once
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/EvtHandlerAccessor.h"
#include "kxf/Network/WebRequest/IWebRequest.h"

namespace kxf::Private
{
	class BasicWebSession;
}

namespace kxf::Private
{
	class KXF_API BasicWebRequest: public RTTI::PrivateStub<BasicWebRequest, IWebRequest>
	{
		friend class BasicWebSession;

		protected:
			EvtHandler m_EvtHandler;
			std::weak_ptr<BasicWebRequest> m_WeakRef;

		protected:
			EventSystem::EvtHandlerAccessor AccessEvtHandler() noexcept
			{
				return m_EvtHandler;
			}
			virtual void PerformRequest() = 0;

		protected:
			// IEvtHandler
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<BindEventFlag> flags = {}) override
			{
				return AccessEvtHandler().DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return AccessEvtHandler().DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return AccessEvtHandler().DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventItem& eventItem) override
			{
				return AccessEvtHandler().OnDynamicBind(eventItem);
			}
			bool OnDynamicUnbind(EventItem& eventItem) override
			{
				return AccessEvtHandler().OnDynamicUnbind(eventItem);
			}

			std::unique_ptr<IEvent> DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}) override
			{
				return AccessEvtHandler().DoQueueEvent(std::move(event), eventID, uuid, flags);
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) override
			{
				return AccessEvtHandler().DoProcessEvent(event, eventID, uuid, flags, onlyIn);
			}

			bool TryBefore(IEvent& event) override
			{
				return AccessEvtHandler().TryBefore(event);
			}
			bool TryAfter(IEvent& event) override
			{
				return AccessEvtHandler().TryAfter(event);
			}

		public:
			BasicWebRequest() noexcept = default;
			BasicWebRequest(const BasicWebRequest&) = delete;

		public:
			// IEvtHandler
			bool ProcessPendingEvents() override
			{
				return m_EvtHandler.ProcessPendingEvents();
			}
			size_t DiscardPendingEvents() override
			{
				return m_EvtHandler.DiscardPendingEvents();
			}

			IEvtHandler* GetPrevHandler() const override
			{
				return m_EvtHandler.GetPrevHandler();
			}
			IEvtHandler* GetNextHandler() const override
			{
				return m_EvtHandler.GetNextHandler();
			}
			void SetPrevHandler(IEvtHandler* evtHandler) override
			{
				m_EvtHandler.SetPrevHandler(evtHandler);
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				m_EvtHandler.SetNextHandler(evtHandler);
			}

			void Unlink() override
			{
				m_EvtHandler.Unlink();
			}
			bool IsUnlinked() const override
			{
				return m_EvtHandler.IsUnlinked();
			}

			bool IsEventProcessingEnabled() const override
			{
				return m_EvtHandler.IsEventProcessingEnabled();
			}
			void EnableEventProcessing(bool enable = true) override
			{
				m_EvtHandler.EnableEventProcessing(enable);
			}

		public:
			// BasicWebRequest
			void WeakRef(const std::shared_ptr<BasicWebRequest>& selfRef)
			{
				m_WeakRef = selfRef;
			}
			std::shared_ptr<BasicWebRequest> LockRef() const
			{
				return m_WeakRef.lock();
			}

		public:
			BasicWebRequest& operator=(const BasicWebRequest&) = delete;
	};
}
