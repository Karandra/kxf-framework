#pragma once
#include "Common.h"
#include "Element.h"
#include "EventDispatcher.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/EvtHandlerAccessor.h"
#include "kxf/EventSystem/EventHandlerStack.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf::Sciter
{
	class WidgetFactory;

	enum class LayoutFlow
	{
		None,
		Horizontal,
		Vertical,
		Text
	};
}

namespace kxf::Sciter
{
	class KXF_API Widget: public RTTI::ExtendInterface<Widget, IEvtHandler>
	{
		kxf_RTTI_DeclareIID(Widget, {0x45e7dd55, 0x5aa6, 0x417a, {0xae, 0xf6, 0xcb, 0x3b, 0x3b, 0xd1, 0xd9, 0x53}});

		friend class BasicEventDispatcher;
		friend class WidgetEventDispatcher;

		private:
			static Widget* DoFromElement(const Element& element);

		public:
			template<class TWidget = Widget, class = std::enable_if_t<std::is_base_of_v<Widget, TWidget>>>
			static TWidget* FromElement(const Element& element)
			{
				if (Widget* widget = DoFromElement(element))
				{
					if constexpr(std::is_same_v<Widget, TWidget>)
					{
						return widget;
					}
					else
					{
						return widget->QueryInterface<TWidget>().get();
					}
				}
				return nullptr;
			}

		private:
			WidgetEventDispatcher m_EventDispatcher;
			EvtHandlerStack m_EventHandlerStack;
			EvtHandler m_EvtHandler;

			Element m_Element;
			WidgetFactory& m_Factory;
			Host& m_Host;

		private:
			auto AccessThisEvtHandler()
			{
				return EventSystem::EvtHandlerAccessor(GetThisEvtHandler());
			}
			auto AccessTopEvtHandler()
			{
				return EventSystem::EvtHandlerAccessor(GetTopEvtHandler());
			}

		protected:
			IEvtHandler& GetThisEvtHandler() noexcept
			{
				return m_EvtHandler;
			}
			IEvtHandler& GetTopEvtHandler() noexcept
			{
				return *m_EventHandlerStack.GetTop();
			}

			// IEvtHandler
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<BindEventFlag> flags = {}) override
			{
				return AccessThisEvtHandler().DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return AccessThisEvtHandler().DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return AccessThisEvtHandler().DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventItem& eventItem) override
			{
				return AccessThisEvtHandler().OnDynamicBind(eventItem);
			}
			bool OnDynamicUnbind(EventItem& eventItem) override
			{
				return AccessThisEvtHandler().OnDynamicUnbind(eventItem);
			}

			std::unique_ptr<IEvent> DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}) override
			{
				return AccessTopEvtHandler().DoQueueEvent(std::move(event), eventID, uuid, flags);
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) override
			{
				return AccessTopEvtHandler().DoProcessEvent(event, eventID, uuid, flags, onlyIn);
			}

			bool TryBefore(IEvent& event) override
			{
				return AccessThisEvtHandler().TryBefore(event);
			}
			bool TryAfter(IEvent& event) override
			{
				return AccessThisEvtHandler().TryAfter(event);
			}

			// Widget
			virtual void OnAttached();
			virtual void OnDetached();

		public:
			Widget(Host& host, WidgetFactory& factory, const Element& element)
				:m_EventDispatcher(host, *this), m_EventHandlerStack(m_EvtHandler), m_Host(host), m_Factory(factory), m_Element(element)
			{
			}
			Widget(const Widget&) = delete;
			virtual ~Widget() = default;

		public:
			// General
			Element& GetElement()
			{
				return m_Element;
			}
			const Element& GetElement() const
			{
				return m_Element;
			}
			Host& GetHost() const
			{
				return m_Host;
			}

			// Factory
			WidgetFactory& GetFactory() const
			{
				return m_Factory;
			}

			// Layout
			LayoutFlow GetLayoutFlow() const;
			void SetLayoutFlow(LayoutFlow flow);

			// Size and position
			Rect GetRect() const
			{
				return m_Element.GetRect();
			}
			Size GetSize() const
			{
				return GetRect().GetSize();
			}
			void SetSize(const Size& size);

			Size GetMinSize() const
			{
				return m_Element.GetMinSize();
			}
			Size GetMaxSize() const
			{
				return m_Element.GetMaxSize();
			}

			Point GetPosition() const
			{
				return GetRect().GetPosition();
			}
			void SetPosition(const Point& pos);
			
			// Focus
			bool IsFocusable() const
			{
				return m_Element.IsFocusable();
			}
			bool HasFocus() const
			{
				return m_Element.HasFocus();
			}
			bool SetFocus()
			{
				return m_Element.SetFocus();
			}

		public:
			IEvtHandler& GetEventHandler()
			{
				return GetTopEvtHandler();
			}

			void PushEventHandler(IEvtHandler& evtHandler)
			{
				m_EventHandlerStack.Push(evtHandler);
			}
			IEvtHandler* PopEventHandler()
			{
				return m_EventHandlerStack.Pop();
			}
			bool RemoveEventHandler(IEvtHandler& evtHandler)
			{
				if (&evtHandler != &m_EvtHandler)
				{
					return m_EventHandlerStack.Remove(evtHandler);
				}
				return false;
			}

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
				// Can't chain widgets
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				// Can't chain widgets
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
			Widget& operator=(const Widget&) = delete;
	};
}
