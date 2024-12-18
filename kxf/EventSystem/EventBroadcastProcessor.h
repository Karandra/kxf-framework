#pragma once
#include "IEvtHandler.h"
#include "EvtHandler.h"
#include "EventHandlerStack.h"
#include "EvtHandlerDelegate.h"

namespace kxf
{
	class EventBroadcastProcessor;
	class EventBroadcastReceiver;
}

namespace kxf::EventSystem
{
	class KXF_API BroadcastProcessorHandler final: public EvtHandler
	{
		private:
			EventBroadcastProcessor& m_Processor;

		protected:
			bool TryBefore(IEvent& event) override;

		public:
			BroadcastProcessorHandler(EventBroadcastProcessor& processor)
				:m_Processor(processor)
			{
			}
	};

	class KXF_API BroadcastReceiverHandler final: public EvtHandler
	{
		friend class EventBroadcastReceiver;

		protected:
			bool TryBefore(IEvent& event) override;
	};
}

namespace kxf
{
	class KXF_API EventBroadcastProcessor: public EvtHandlerDelegate
	{
		friend class EventBroadcastReceiver;

		public:
			using Order = EvtHandlerStack::Order;

		private:
			EventSystem::BroadcastProcessorHandler m_EvtHandler;
			EvtHandlerStack m_Stack;
			Order m_Order = Order::LastToFirst;

		protected:
			virtual bool PreProcessEvent(IEvent& event)
			{
				return true;
			}
			virtual void PostProcessEvent(IEvent& event)
			{
			}

		public:
			EventBroadcastProcessor()
				:EvtHandlerDelegate(m_EvtHandler), m_EvtHandler(*this), m_Stack(m_EvtHandler)
			{
			}
			virtual ~EventBroadcastProcessor() = default;

		public:
			bool AddReceiver(EventBroadcastReceiver& reciever);
			bool RemoveReceiver(EventBroadcastReceiver& reciever);

			bool HasReceiveres() const
			{
				return m_Stack.HasChainedItems();
			}
			size_t GetReceiveresCount() const
			{
				return m_Stack.GetCount();
			}

			Enumerator<IEvtHandler&> EnumReceiveres(Order order) const;
			Enumerator<IEvtHandler&> EnumReceiveres() const;

			Order GetReceiversOrder() const
			{
				return m_Order;
			}
			void SetReceiversOrder(Order order)
			{
				m_Order = order;
			}
	};
}

namespace kxf
{
	class KXF_API EventBroadcastReceiver: public EvtHandlerDelegate
	{
		friend class EventBroadcastProcessor;

		private:
			EventSystem::BroadcastReceiverHandler m_EvtHandler;
			EventBroadcastProcessor& m_Processor;

		private:
			bool PreProcessEvent(IEvent& event);
			void PostProcessEvent(IEvent& event);
			void FinalPostProcessEvent(IEvent& event);

			void StopPropagation(IEvent& event);

		protected:
			IEvtHandler& GetEvtHandler()
			{
				return m_EvtHandler;
			}

		public:
			EventBroadcastReceiver(EventBroadcastProcessor& processor)
				:EvtHandlerDelegate(m_EvtHandler), m_Processor(processor)
			{
				m_Processor.AddReceiver(*this);
			}
			virtual ~EventBroadcastReceiver()
			{
				m_Processor.RemoveReceiver(*this);
			}

		public:
			EventBroadcastProcessor& GetProcessor()
			{
				return m_Processor;
			}
			const EventBroadcastProcessor& GetProcessor() const
			{
				return m_Processor;
			}
	};
}
