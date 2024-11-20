#pragma once
#include "kxf/Core/OptionalPtr.h"
#include "kxf/Core/IAsyncTaskExecutor.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/EvtHandlerAccessor.h"
#include "kxf/Network/WebRequest/IWebSession.h"
#include "kxf/Network/WebRequest/WebRequestHeader.h"

namespace kxf::Private
{
	class BasicWebRequest;
}

namespace kxf::Private
{
	class KX_API BasicWebSession: public RTTI::PrivateStub<BasicWebSession, IWebSession>
	{
		friend class BasicWebRequest;

		public:
			static bool SetHeader(std::vector<WebRequestHeader>& headerStorage, const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags);

		protected:
			EvtHandler m_EvtHandler;
			std::shared_ptr<IAsyncTaskExecutor> m_TaskExecutor;

		protected:
			EventSystem::EvtHandlerAccessor AccessEvtHandler() noexcept
			{
				return m_EvtHandler;
			}
			void DoInitialize(std::shared_ptr<IAsyncTaskExecutor> taskExecutor = {});

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
			BasicWebSession() noexcept = default;

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
			// BasicWebSession
			bool IsNull() const noexcept;
			bool StartRequest(BasicWebRequest& request);
	};
}
