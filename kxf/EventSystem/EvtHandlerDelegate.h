#pragma once
#include "Common.h"
#include "IEvtHandler.h"
#include "EvtHandlerAccessor.h"
#include "kxf/Core/OptionalPtr.h"

namespace kxf
{
	class KXF_API EvtHandlerDelegate: public RTTI::Implementation<EvtHandlerDelegate, IEvtHandler>
	{
		private:
			optional_ptr<IEvtHandler> m_EvtHandler;

		private:
			auto Access()
			{
				return EventSystem::EvtHandlerAccessor(*m_EvtHandler);
			}

		protected:
			// IEvtHandler
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<BindEventFlag> flags = {}) override
			{
				return Access().DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return Access().DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return Access().DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventItem& eventItem) override
			{
				return Access().OnDynamicBind(eventItem);
			}
			bool OnDynamicUnbind(EventItem& eventItem) override
			{
				return Access().OnDynamicUnbind(eventItem);
			}

			std::unique_ptr<IEvent> DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}) override
			{
				return Access().DoQueueEvent(std::move(event), eventID, uuid, flags);
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) override
			{
				return Access().DoProcessEvent(event, eventID, uuid, flags, onlyIn);
			}

			bool TryBefore(IEvent& event) override
			{
				return Access().TryBefore(event);
			}
			bool TryAfter(IEvent& event) override
			{
				return Access().TryAfter(event);
			}

		public:
			// EvtHandlerDelegate
			EvtHandlerDelegate() noexcept = default;
			EvtHandlerDelegate(IEvtHandler& evtHandler) noexcept
				:m_EvtHandler(evtHandler)
			{
			}
			EvtHandlerDelegate(std::unique_ptr<IEvtHandler> evtHandler) noexcept
				:m_EvtHandler(std::move(evtHandler))
			{
			}
			EvtHandlerDelegate(EvtHandlerDelegate&&) noexcept = default;
			EvtHandlerDelegate(const EvtHandlerDelegate&) = delete;
			~EvtHandlerDelegate() = default;

		public:
			// EvtHandlerDelegate
			bool IsNull() const noexcept
			{
				return m_EvtHandler.is_null();
			}
			IEvtHandler* Get() const noexcept
			{
				return m_EvtHandler.get();
			}
			EvtHandlerDelegate ShallowClone() const noexcept
			{
				if (m_EvtHandler)
				{
					return *m_EvtHandler.get();
				}
				return {};
			}

			// IEvtHandler: Event queuing and processing
			bool ProcessPendingEvents() override
			{
				return m_EvtHandler->ProcessPendingEvents();
			}
			size_t DiscardPendingEvents() override
			{
				return m_EvtHandler->DiscardPendingEvents();
			}

			bool IsEventProcessingEnabled() const override
			{
				return m_EvtHandler->IsEventProcessingEnabled();
			}
			void EnableEventProcessing(bool enable = true) override
			{
				m_EvtHandler->EnableEventProcessing(enable);
			}

			// IEvtHandler: Event handlers chain
			IEvtHandler* GetPrevHandler() const override
			{
				return m_EvtHandler->GetPrevHandler();
			}
			IEvtHandler* GetNextHandler() const override
			{
				return m_EvtHandler->GetNextHandler();
			}
			void SetPrevHandler(IEvtHandler* evtHandler) override
			{
				m_EvtHandler->SetPrevHandler(evtHandler);
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				m_EvtHandler->SetNextHandler(evtHandler);
			}

			void Unlink() override
			{
				m_EvtHandler->Unlink();
			}
			bool IsUnlinked() const override
			{
				return m_EvtHandler->IsUnlinked();
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			const IEvtHandler& operator*() const noexcept
			{
				return *m_EvtHandler;
			}
			IEvtHandler& operator*() noexcept
			{
				return *m_EvtHandler;
			}

			EvtHandlerDelegate& operator=(EvtHandlerDelegate&&) noexcept = default;
			EvtHandlerDelegate& operator=(const EvtHandlerDelegate&) = delete;
	};
}
