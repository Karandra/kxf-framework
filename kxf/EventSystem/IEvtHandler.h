#pragma once
#include "Common.h"
#include "Event.h"
#include "EventID.h"
#include "EventItem.h"
#include "EventBuilder.h"
#include "EventExecutor.h"
#include "SignalExecutor.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/wxWidgets/IWithEvent.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/Memory.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf
{
	class IEventExecutor;
	class EvtHandlerDelegate;
}
namespace kxf::EventSystem
{
	class EvtHandlerAccessor;
}

namespace kxf
{
	class KXF_API IEvtHandler: public RTTI::Interface<IEvtHandler>
	{
		friend class EventSystem::EvtHandlerAccessor;

		kxf_RTTI_DeclareIID(IEvtHandler, {0x96ae0970, 0x8cc5, 0x4288, {0xb1, 0x1b, 0x7a, 0xe6, 0x42, 0xf8, 0xdf, 0x8c}});

		protected:
			using EventItem = EventSystem::EventItem;

		private:
			template<class TEvent, class TFunc>
			static void DoCallWxEvent(IEvent& event, TFunc&& func) noexcept(std::is_nothrow_invocable_v<TFunc, TEvent&>)
			{
				if (auto withEvent = event.QueryInterface<wxWidgets::IWithEvent>())
				{
					std::invoke(func, static_cast<TEvent&>(withEvent->GetEvent()));
				}
				else
				{
					event.Skip();
				}
			}

			template<class TCallable, class... Args>
			std::unique_ptr<IEvent> DoCallAfter(const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, TCallable&& callable, Args&&... arg)
			{
				using namespace EventSystem;
				using Traits = typename Utility::CallableTraits<TCallable, Args...>;

				flags.Add(ProcessEventFlag::Locally);
				if constexpr(Traits::IsMemberFunction)
				{
					return DoQueueEvent(std::make_unique<MethodIndirectInvocation<TCallable, Args...>>(callable, std::forward<Args>(arg)...), IIndirectInvocationEvent::EvtIndirectInvocation, uuid, flags);
				}
				else if constexpr(Traits::IsInvokable)
				{
					return DoQueueEvent(std::make_unique<CallableIndirectInvocation<TCallable, Args...>>(std::forward<TCallable>(callable), std::forward<Args>(arg)...), IIndirectInvocationEvent::EvtIndirectInvocation, uuid, flags);
				}
				else
				{
					static_assert(sizeof(TCallable*) == 0, "Unsupported callable type or the type is not invocable");
				}
			}

			template<SignalParametersSemantics signalSemantics, class TFunc, class TMethod, class... Args>
			typename Utility::MethodTraits<TMethod>::TReturn DoProcessSignal(TFunc&& func, TMethod method, Args&&... arg)
			{
				using TResult = typename Utility::MethodTraits<TMethod>::TReturn;

				EventSystem::SignalInvocationEvent<signalSemantics, TMethod> event(std::forward<Args>(arg)...);
				if (std::invoke(func, event, method))
				{
					if constexpr(!std::is_void_v<TResult>)
					{
						auto TakeResult = [&](ISignalInvocationEvent& event)
						{
							alignas(TResult) uint8_t resultBuffer[sizeof(TResult)] = {};
							event.TakeResult(resultBuffer);

							Utility::ScopeGuard atExit = [&]()
							{
								Utility::DestroyAt<TResult>(resultBuffer);
							};
							return *std::launder(reinterpret_cast<TResult*>(resultBuffer));
						};

						// If we have a wait result then the event was executed as blocked queued, so get result from it.
						if (auto eventInternal = event.QueryInterface<IEventInternal>()->GetWaitResult())
						{
							return TakeResult(*eventInternal->QueryInterface<ISignalInvocationEvent>());
						}
						return TakeResult(event);
					}
				}

				// Suppress warning about missing return type
				if constexpr(!std::is_void_v<TResult>)
				{
					return {};
				}
			}

			template<SignalParametersSemantics signalSemantics, class TFunc, class TMethod, class... Args>
			void DoQueueSignal(TFunc&& func, TMethod method, Args&&... arg)
			{
				static_assert(std::is_void_v<typename Utility::MethodTraits<TMethod>::TReturn>, "Queued signal isn't allowed to return a value");

				std::invoke(func, std::make_unique<EventSystem::SignalInvocationEvent<signalSemantics, TMethod>>(std::forward<Args>(arg)...), method);
			}

		protected:
			virtual LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<BindEventFlag> flags = {}) = 0;
			virtual bool DoUnbind(const EventID& eventID, IEventExecutor& executor) = 0;
			virtual bool DoUnbind(const LocallyUniqueID& bindSlot) = 0;

			virtual bool OnDynamicBind(EventItem& eventItem) = 0;
			virtual bool OnDynamicUnbind(EventItem& eventItem) = 0;

			virtual std::unique_ptr<IEvent> DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}) = 0;
			virtual bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) = 0;

			virtual bool TryBefore(IEvent& event) = 0;
			virtual bool TryAfter(IEvent& event) = 0;

		public:
			IEvtHandler() = default;
			virtual ~IEvtHandler() = default;

		public:
			// Event queuing and processing
			virtual bool ProcessPendingEvents() = 0;
			virtual size_t DiscardPendingEvents() = 0;

			virtual bool IsEventProcessingEnabled() const = 0;
			virtual void EnableEventProcessing(bool enable = true) = 0;

			// Event handlers chain
			virtual IEvtHandler* GetPrevHandler() const = 0;
			virtual IEvtHandler* GetNextHandler() const = 0;
			virtual void SetPrevHandler(IEvtHandler* evtHandler) = 0;
			virtual void SetNextHandler(IEvtHandler* evtHandler) = 0;

			virtual void Unlink() = 0;
			virtual bool IsUnlinked() const = 0;

		public:
			// Bind free or static function
			template<class TEvent, class TEventArg>
			LocallyUniqueID Bind(const EventTag<TEvent>& eventTag, void(*func)(TEventArg&), FlagSet<BindEventFlag> flags = BindEventFlag::Direct)
			{
				return DoBind(eventTag, std::make_unique<EventSystem::CallableEventExecutor<TEvent, decltype(func)>>(func), flags);
			}

			// Unbind free or static function
			template<class TEvent, class TEventArg>
			bool Unbind(const EventTag<TEvent>& eventTag, void(*func)(TEventArg&))
			{
				EventSystem::CallableEventExecutor<TEvent, decltype(func)> executor(func);
				return DoUnbind(eventTag, executor);
			}

			// Bind a generic callable
			template<class TEvent, class TCallable>
			LocallyUniqueID Bind(const EventTag<TEvent>& eventTag, TCallable&& callable, FlagSet<BindEventFlag> flags = BindEventFlag::Direct)
			{
				return DoBind(eventTag, std::make_unique<EventSystem::CallableEventExecutor<TEvent, TCallable>>(std::forward<TCallable>(callable)), flags);
			}

			// Unbind a generic callable
			template<class TEvent, class TCallable>
			bool Unbind(const EventTag<TEvent>& eventTag, const TCallable& callable)
			{
				EventSystem::CallableEventExecutor<TEvent, TCallable> executor(std::forward<TCallable>(callable));
				return DoUnbind(eventTag, executor);
			}

			// Bind a member function
			template<class TEvent, class TClass, class TEventArg, class TEventHandler>
			LocallyUniqueID Bind(const EventTag<TEvent>& eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler, FlagSet<BindEventFlag> flags = BindEventFlag::Direct)
			{
				return DoBind(eventTag, std::make_unique<EventSystem::MethodEventExecutor<TEvent, TClass, TEventArg, TEventHandler>>(method, *handler), flags);
			}

			// Unbind a member function
			template<class TEvent, class TClass, class TEventArg, class TEventHandler>
			bool Unbind(const EventTag<TEvent>& eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler)
			{
				EventSystem::MethodEventExecutor<TEvent, TClass, TEventArg, TEventHandler> executor(method, *handler);
				return DoUnbind(eventTag, executor);
			}

			// Unbind by a given bind slot
			bool Unbind(LocallyUniqueID bindSlot)
			{
				return DoUnbind(bindSlot);
			}

			// Unbind all events
			bool UnbindAll()
			{
				return DoUnbind(IEvent::EvtAny, EventSystem::NullEventExecutor::Get());
			}

			// Unbind all events with this ID
			bool UnbindAll(const EventID& id)
			{
				return id != IEvent::EvtAny && DoUnbind(id, EventSystem::NullEventExecutor::Get());
			}

		public:
			// Bind a generic callable
			template<BindSignalFlag signalFlags = BindSignalFlag::None, class TSignal, class TCallable, class = std::enable_if_t<std::is_member_function_pointer_v<TSignal>>>
			LocallyUniqueID BindSignal(TSignal signal, TCallable&& callable, FlagSet<BindEventFlag> flags = BindEventFlag::Direct)
			{
				return DoBind(signal, std::make_unique<EventSystem::CallableSignalExecutor<TSignal, TCallable, signalFlags>>(std::forward<TCallable>(callable)), flags);
			}

			// Bind a member function
			template
			<
				BindSignalFlag signalFlags = BindSignalFlag::None,
				class TSignal,
				class THandlerMethod,
				class THandlerObject,
				class = std::enable_if_t
				<
					std::is_member_function_pointer_v<TSignal> &&
					std::is_same_v<typename Utility::MethodTraits<THandlerMethod>::TInstance, THandlerObject> &&
					std::is_convertible_v<typename Utility::MethodTraits<TSignal>::TArgsTuple, typename Utility::MethodTraits<THandlerMethod>::TArgsTuple>
				>
			>
			LocallyUniqueID BindSignal(TSignal signal, THandlerMethod targetMethod, THandlerObject* handler, FlagSet<BindEventFlag> flags = BindEventFlag::Direct)
			{
				return DoBind(signal, std::make_unique<EventSystem::MethodSignalExecutor<TSignal, THandlerMethod, THandlerObject, signalFlags>>(targetMethod, *handler), flags);
			}

			// Process a signal
			template
			<
				SignalParametersSemantics signalSemantics = SignalParametersSemantics::Copy,
				class TSignal,
				class... Args,
				class = std::enable_if_t<std::is_invocable_v<TSignal, typename Utility::MethodTraits<TSignal>::TInstance, Args...>>
			>
			typename Utility::MethodTraits<TSignal>::TReturn ProcessSignal(TSignal signal, Args&&... arg)
			{
				return DoProcessSignal<signalSemantics>([&](IEvent& event, const EventID& eventID)
				{
					return DoProcessEvent(event, eventID);
				}, signal, std::forward<Args>(arg)...);
			}

			template
			<
				SignalParametersSemantics signalSemantics = SignalParametersSemantics::Copy,
				class TSignal,
				class... Args,
				class = std::enable_if_t<std::is_invocable_v<TSignal, typename Utility::MethodTraits<TSignal>::TInstance, Args...>>
			>
			typename Utility::MethodTraits<TSignal>::TReturn ProcessSignal(FlagSet<ProcessEventFlag> flags, TSignal signal, Args&&... arg)
			{
				return DoProcessSignal<signalSemantics>([&](IEvent& event, const EventID& eventID)
				{
					return DoProcessEvent(event, eventID, {}, flags);
				}, signal, std::forward<Args>(arg)...);
			}

			template
			<
				SignalParametersSemantics signalSemantics = SignalParametersSemantics::Copy,
				class TSignal,
				class... Args,
				class = std::enable_if_t<std::is_invocable_v<TSignal, typename Utility::MethodTraits<TSignal>::TInstance, Args...>>
			>
			typename Utility::MethodTraits<TSignal>::TReturn ProcessUniqueSignal(const UniversallyUniqueID& uuid, TSignal signal, Args&&... arg)
			{
				return DoProcessSignal<signalSemantics>([&](IEvent& event, const EventID& eventID)
				{
					return DoProcessEvent(event, eventID, uuid);
				}, signal, std::forward<Args>(arg)...);
			}

			template
			<
				SignalParametersSemantics signalSemantics = SignalParametersSemantics::Copy,
				class TSignal,
				class... Args,
				class = std::enable_if_t<std::is_invocable_v<TSignal, typename Utility::MethodTraits<TSignal>::TInstance, Args...>>
			>
			typename Utility::MethodTraits<TSignal>::TReturn ProcessUniqueSignal(const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, TSignal signal, Args&&... arg)
			{
				return DoProcessSignal<signalSemantics>([&](IEvent& event, const EventID& eventID)
				{
					return DoProcessEvent(event, eventID, uuid, flags);
				}, signal, std::forward<Args>(arg)...);
			}

			// Queue a signal for later processing
			template
			<
				SignalParametersSemantics signalSemantics = SignalParametersSemantics::Copy,
				class TSignal,
				class... Args,
				class = std::enable_if_t<std::is_invocable_v<TSignal, typename Utility::MethodTraits<TSignal>::TInstance, Args...>>
			>
			void QueueSignal(TSignal signal, Args&&... arg)
			{
				DoQueueSignal<signalSemantics>([&](std::unique_ptr<IEvent> event, const EventID& eventID)
				{
					DoQueueEvent(std::move(event), eventID);
				}, signal, std::forward<Args>(arg)...);
			}

			template
			<
				SignalParametersSemantics signalSemantics = SignalParametersSemantics::Copy,
				class TSignal,
				class... Args,
				class = std::enable_if_t<std::is_invocable_v<TSignal, typename Utility::MethodTraits<TSignal>::TInstance, Args...>>
			>
			void QueueSignal(FlagSet<ProcessEventFlag> flags, TSignal signal, Args&&... arg)
			{
				DoQueueSignal<signalSemantics>([&](std::unique_ptr<IEvent> event, const EventID& eventID)
				{
					DoQueueEvent(std::move(event), eventID, {}, flags);
				}, signal, std::forward<Args>(arg)...);
			}

			template
			<
				SignalParametersSemantics signalSemantics = SignalParametersSemantics::Copy,
				class TSignal,
				class... Args,
				class = std::enable_if_t<std::is_invocable_v<TSignal, typename Utility::MethodTraits<TSignal>::TInstance, Args...>>
			>
			void QueueUniqueSignal(const UniversallyUniqueID& uuid, TSignal signal, Args&&... arg)
			{
				DoQueueSignal<signalSemantics>([&](std::unique_ptr<IEvent> event, const EventID& eventID)
				{
					DoQueueEvent(std::move(event), eventID, uuid);
				}, signal, std::forward<Args>(arg)...);
			}
			
			template
			<
				SignalParametersSemantics signalSemantics = SignalParametersSemantics::Copy,
				class TSignal,
				class... Args,
				class = std::enable_if_t<std::is_invocable_v<TSignal, typename Utility::MethodTraits<TSignal>::TInstance, Args...>>
			>
			void QueueUniqueSignal(const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, TSignal signal, Args&&... arg)
			{
				DoQueueSignal<signalSemantics>([&](std::unique_ptr<IEvent> event, const EventID& eventID)
				{
					DoQueueEvent(std::move(event), eventID, uuid, flags);
				}, signal, std::forward<Args>(arg)...);
			}

		public:
			// [WX] Bind free or static function
			template<class TEvent, class TEventArg, class = std::enable_if_t<std::is_base_of_v<wxEvent, TEvent>>>
			LocallyUniqueID BindWx(wxEventTypeTag<TEvent> eventTag, void(*func)(TEventArg&), FlagSet<BindEventFlag> flags = BindEventFlag::Direct)
			{
				return Bind(EventTag<IEvent>(eventTag), [func](IEvent& event)
				{
					DoCallWxEvent<TEventArg>(event, func);
				}, flags);
			}

			// [WX] Bind a generic callable
			template<class TEvent, class TCallable, class = std::enable_if_t<std::is_base_of_v<wxEvent, TEvent>>>
			LocallyUniqueID BindWx(wxEventTypeTag<TEvent> eventTag, TCallable&& callable, FlagSet<BindEventFlag> flags = BindEventFlag::Direct)
			{
				return Bind(EventTag<IEvent>(eventTag), [callable = std::forward<TCallable>(callable)](IEvent& event)
				{
					DoCallWxEvent<TEvent>(event, callable);
				}, flags);
			}

			// [WX] Bind a member function
			template<class TEvent, class TClass, class TEventArg, class TEventHandler, class = std::enable_if_t<std::is_base_of_v<wxEvent, TEvent>>>
			LocallyUniqueID BindWx(wxEventTypeTag<TEvent> eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler, FlagSet<BindEventFlag> flags = BindEventFlag::Direct)
			{
				return Bind(EventTag<IEvent>(eventTag), [method, handler](IEvent& event)
				{
					DoCallWxEvent<TEventArg>(event, [&](TEventArg& event)
					{
						std::invoke(method, handler, event);
					});
				}, flags);
			}

		public:
			bool ProcessEvent(IEvent& event, const EventID& eventID = {}, FlagSet<ProcessEventFlag> flags = {})
			{
				return DoProcessEvent(event, eventID, {}, flags);
			}

			template<class TEvent, class... Args>
			bool ProcessEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return DoProcessEvent(event, eventTag);
			}

			template<class TEvent, class... Args>
			bool ProcessEvent(FlagSet<ProcessEventFlag> flags, const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return DoProcessEvent(event, eventTag, {}, flags);
			}

			bool ProcessUniqueEvent(const UniversallyUniqueID& uuid, IEvent& event, const EventID& eventID = {}, FlagSet<ProcessEventFlag> flags = {})
			{
				if (uuid)
				{
					return DoProcessEvent(event, eventID, uuid, flags);
				}
				return false;
			}

			template<class TEvent, class... Args>
			bool ProcessUniqueEvent(const UniversallyUniqueID& uuid, const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				if (uuid)
				{
					TEvent event(std::forward<Args>(arg)...);
					return DoProcessEvent(event, eventTag, uuid);
				}
				return false;
			}

			template<class TEvent, class... Args>
			bool ProcessUniqueEvent(const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				if (uuid)
				{
					TEvent event(std::forward<Args>(arg)...);
					return DoProcessEvent(event, eventTag, uuid, flags);
				}
				return false;
			}

			// Construct and send the event using the event builder
			template<class TEvent, class... Args>
			decltype(auto) BuildProcessEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				return EventSystem::DirectEventBuilder<TEvent>(*this, TEvent(std::forward<Args>(arg)...), eventTag);
			}

		public:
			// Queue an event for later processing
			std::unique_ptr<IEvent> QueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, FlagSet<ProcessEventFlag> flags = {})
			{
				return DoQueueEvent(std::move(event), eventID, {}, flags);
			}

			template<class TEvent, class... Args>
			std::unique_ptr<TEvent> QueueEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				return Utility::StaticCastUniquePtr<TEvent>(DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag));
			}

			template<class TEvent, class... Args>
			std::unique_ptr<TEvent> QueueEvent(FlagSet<ProcessEventFlag> flags, const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				return Utility::StaticCastUniquePtr<TEvent>(DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag, {}, flags));
			}

			std::unique_ptr<IEvent> QueueUniqueEvent(const UniversallyUniqueID& uuid, std::unique_ptr<IEvent> event, const EventID& eventID = {}, FlagSet<ProcessEventFlag> flags = {})
			{
				if (uuid)
				{
					return DoQueueEvent(std::move(event), eventID, uuid, flags);
				}
				return nullptr;
			}

			template<class TEvent, class... Args>
			std::unique_ptr<TEvent> QueueUniqueEvent(const UniversallyUniqueID& uuid, const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				if (uuid)
				{
					return Utility::StaticCastUniquePtr<TEvent>(DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag, uuid));
				}
				return nullptr;
			}

			template<class TEvent, class... Args>
			std::unique_ptr<TEvent> QueueUniqueEvent(const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				if (uuid)
				{
					return Utility::StaticCastUniquePtr<TEvent>(DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag, uuid, flags));
				}
				return nullptr;
			}

			// Construct and queue event using the event builder
			template<class TEvent, class... Args>
			decltype(auto) BuildQueueEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				return EventSystem::QueuedEventBuilder<TEvent>(*this, std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag);
			}

		public:
			// Queue execution of a given callable to the next event loop iteration
			template<class TCallable, class... Args, class = std::enable_if_t<std::is_invocable_v<TCallable, Args...>>>
			std::unique_ptr<IEvent> CallAfter(TCallable&& callable, Args&&... arg)
			{
				return DoCallAfter({}, {}, std::forward<TCallable>(callable), std::forward<Args>(arg)...);
			}

			template<class TCallable, class... Args, class = std::enable_if_t<std::is_invocable_v<TCallable, Args...>>>
			std::unique_ptr<IEvent> CallAfter(FlagSet<ProcessEventFlag> flags, TCallable&& callable, Args&&... arg)
			{
				return DoCallAfter({}, flags, std::forward<TCallable>(callable), std::forward<Args>(arg)...);
			}

			// Queue execution of a given callable to the next event loop iteration replacing previously sent callable with the same ID
			template<class TCallable, class... Args, class = std::enable_if_t<std::is_invocable_v<TCallable, Args...>>>
			std::unique_ptr<IEvent> UniqueCallAfter(const UniversallyUniqueID& uuid, TCallable&& callable, Args&&... arg)
			{
				return DoCallAfter(uuid, std::forward<TCallable>(callable), std::forward<Args>(arg)...);
			}

			template<class TCallable, class... Args, class = std::enable_if_t<std::is_invocable_v<TCallable, Args...>>>
			std::unique_ptr<IEvent> UniqueCallAfter(const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, TCallable&& callable, Args&&... arg)
			{
				return DoCallAfter(uuid, flags, std::forward<TCallable>(callable), std::forward<Args>(arg)...);
			}
	};
}
