#include "kxf-pch.h"
#include "EvtHandler.h"
#include "IdleEvent.h"
#include "EvtHandlerAccessor.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/Application/Private/Utility.h"
#include "kxf/Utility/Container.h"

namespace
{
	using namespace kxf;

	template<class TEventTable, class TFunc>
	size_t SearchForBoundHandlers(TEventTable&& eventTable, const EventID& eventID, IEventExecutor& executor, TFunc&& onFound)
	{
		const EventSystem::EventItem dummyItem(eventID, executor);
		const IEventExecutor& nullExecutor = EventSystem::NullEventExecutor::Get();

		size_t count = 0;
		for (auto it = std::rbegin(eventTable); it != std::rend(eventTable); ++it)
		{
			// 1) If 'eventID' is 'IEvent::EvtAny' unbind *all* events.
			// 2) If 'eventID' matches our item ID and the supplied executor is a special null executor unbind all events with this ID.
			// 3) Unbind the event if both event ID and the executor matches (See 'EventItem::IsSameAs' for details).
			if ((eventID == IEvent::EvtAny) || (eventID == it->GetEventID() && &executor == &nullExecutor) || (it->IsSameAs(dummyItem)))
			{
				count++;
				if (!std::invoke(onFound, it))
				{
					break;
				}
			}
		}
		return count;
	}

	template<class TEventTable>
	size_t CountBoundHandlers(TEventTable&& eventTable, const EventID& eventID, IEventExecutor& executor)
	{
		return SearchForBoundHandlers(std::forward<TEventTable>(eventTable), eventID, executor, [](auto&&)
		{
			return true;
		});
	}

	bool IsInvalidFlagCombination(FlagSet<BindEventFlag> flags) noexcept
	{
		return flags.Contains(BindEventFlag::OneShot|BindEventFlag::AlwaysSkip) ||

			flags.Contains(BindEventFlag::Queued|BindEventFlag::Auto) ||
			flags.Contains(BindEventFlag::Direct|BindEventFlag::Queued) ||
			flags.Contains(BindEventFlag::Direct|BindEventFlag::Auto) ||
			flags.Contains(BindEventFlag::Direct|BindEventFlag::Queued|BindEventFlag::Auto);
	}
	bool ShouldQueueeEvent(FlagSet<BindEventFlag> flags, bool isMainThread) noexcept
	{
		return flags.Contains(BindEventFlag::Queued) || (flags.Contains(BindEventFlag::Auto) && !isMainThread);
	}
}

namespace kxf
{
	void EvtHandler::Move(EvtHandler&& other, bool destroy)
	{
		if (this == &other)
		{
			return;
		}
		if (destroy)
		{
			Destroy();
		}

		{
			WriteLockGuard lockThis(m_EventTableLock);
			WriteLockGuard lockOther(other.m_EventTableLock);

			m_EventTable = std::move(other.m_EventTable);
			m_EventBindSlot = std::exchange(other.m_EventBindSlot, 0);
		}
		{
			WriteLockGuard lockThis(m_PendingEventsLock);
			WriteLockGuard lockOther(other.m_PendingEventsLock);

			m_PendingEvents = std::move(other.m_PendingEvents);
		}
		m_PrevHandler.exchange(other.m_PrevHandler);
		m_NextHandler.exchange(other.m_NextHandler);
		m_IsEnabled.exchange(other.m_IsEnabled);
	}
	void EvtHandler::Destroy()
	{
		Unlink();

		// Remove us from the global list of the pending events if necessary
		if (auto app = ICoreApplication::GetInstance())
		{
			app->RemovePendingEventHandler(*this);
		}
		DiscardPendingEvents();

		WriteLockGuard lockGuard(m_EventTableLock);
		m_EventTable.clear();
	}

	void EvtHandler::PrepareEvent(IEvent& event, const EventID& eventID, const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, bool isAsync)
	{
		if (event.QueryInterface<IEventInternal>()->OnStartProcess(eventID, uuid, flags, isAsync))
		{
			// Set event source for indirect event calls
			if (isAsync && event.QueryInterface<IIndirectInvocationEvent>())
			{
				event.SetEventSource(QueryInterface<IEvtHandler>());
			}
		}
	}
	bool EvtHandler::FreeBindSlot(const LocallyUniqueID& bindSlot)
	{
		// If it's the last bind slot we have used, free it.
		if (m_EventBindSlot == bindSlot.ToInt())
		{
			--m_EventBindSlot;
			return true;
		}
		return false;
	}
	void EvtHandler::FreeAllBindSlots()
	{
		m_EventBindSlot = 0;
	}

	bool EvtHandler::TryApp(IEvent& event)
	{
		auto app = ICoreApplication::GetInstance();
		if (app && static_cast<IEvtHandler*>(this) != app)
		{
			// Special case: don't pass 'IIdleEvent::EvtIdle' to app, since it'll always
			// swallow it. Events of 'IIdleEvent::EvtIdle' are sent explicitly to the app
			//so it will be processed appropriately via 'EvtHandler::SearchEventTable'.
			if (event.GetEventID() != IdleEvent::EvtIdle && EventSystem::EvtHandlerAccessor(*app).DoProcessEvent(event))
			{
				return true;
			}
		}
		return false;
	}
	bool EvtHandler::TryChain(IEvent& event)
	{
		for (IEvtHandler* evtHandler = GetNextHandler(); evtHandler; evtHandler = evtHandler->GetNextHandler())
		{
			// We need to process this event at the level of this handler only
			// right now, the pre-/post-processing was either already done by
			// 'DoProcessEvent' from which we were called or will be done by it when
			// we return.
			//
			// However we must call 'DoProcessEvent' and not 'TryHereOnly' because the
			// existing code expects the overridden 'DoProcessEvent' in its custom event
			// handlers pushed on a widget to be called.
			//
			// So we must call 'DoProcessEvent' but it must not do what it usually
			// does. To resolve this paradox we set up a special parameter for 'DoProcessEvent'
			// to make it process only in specified handler.

			if (EventSystem::EvtHandlerAccessor(*evtHandler).DoProcessEvent(event, {}, {}, {}, evtHandler))
			{
				// Make sure "skipped" flag is not set as the event was really
				// processed in this case. Normally it shouldn't be set anyhow but
				// make sure just in case the user code does something strange.

				event.Skip(false);
				return true;
			}
		}
		return false;
	}
	bool EvtHandler::TryLocally(IEvent& event)
	{
		// Try the hooks which should be called before our own handlers and this
		// handler itself first. Notice that we should not call 'DoProcessEvent' on
		// this one as we're already called from it, which explains why we do it
		// here and not in 'TryChain'.
		return TryBeforeAndHere(event) || TryChain(event);
	}
	bool EvtHandler::TryHereOnly(IEvent& event)
	{
		// If the event handler is disabled it doesn't process any events at all
		if (IsEventProcessingEnabled())
		{
			// There is an implicit entry for indirect invocation in every event handler
			std::shared_ptr<IIndirectInvocationEvent> indirectInvoke;
			if (event.GetEventSource().get() == this && event.QueryInterface(indirectInvoke))
			{
				indirectInvoke->Execute();
				return true;
			}

			// Search for the bound event handler
			if (SearchEventTable(event))
			{
				return true;
			}
		}

		// We don't have a handler for this event
		return false;
	}
	bool EvtHandler::TryBeforeAndHere(IEvent& event)
	{
		// Another helper which simply calls pre-processing hook and then tries to handle the event at this handler level
		return TryBefore(event) || TryHereOnly(event);
	}

	bool EvtHandler::SearchEventTable(IEvent& event)
	{
		bool executed = false;
		size_t initialSize = 0;
		size_t nullCount = 0;
		LocallyUniqueID eventSlot;

		if (ReadLockGuard lockGuard(m_EventTableLock); !m_EventTable.empty())
		{
			initialSize = m_EventTable.size();

			const EventID eventID = event.GetEventID();
			for (auto it = m_EventTable.rbegin(); it != m_EventTable.rend(); ++it)
			{
				EventItem& eventItem = *it;
				if (eventItem.IsNull())
				{
					// This item must have been unbound at some time in the past, so skip it now
					// and really remove it from the event table, once we finish iterating.
					nullCount++;
					continue;
				}

				if (eventItem.IsSameEventID(eventID))
				{
					IEvtHandler* evtHandler = eventItem.GetExecutor()->GetTargetHandler();
					if (!evtHandler)
					{
						evtHandler = this;
					}

					// If the handler wants to be executed on the main thread, queue it, mark as queued (done in 'WasReQueued') and return.
					const auto eventFlags = eventItem.GetFlags();
					const bool isMainThread = wxThread::IsMain();
					if (ShouldQueueeEvent(eventFlags, isMainThread))
					{
						auto eventInternal = event.QueryInterface<IEventInternal>();
						if (!eventInternal->WasReQueued() && !eventInternal->IsAsync())
						{
							if (eventFlags.Contains(BindEventFlag::Blocking))
							{
								wxASSERT_MSG(!isMainThread, "Option 'BindEventFlag::Blocking' should not ever be used from main thread");

								// TODO: Do we need some kind of lock to ensure that our moved event won't get processed until after we call 'WaitProcessed' on it?
								auto movedEvent = event.Move();
								auto movedEventRef = movedEvent->QueryInterface<IEventInternal>();
								DoQueueEvent(std::move(movedEvent));

								// We need to leave the guard here so the main thread can enter this function. We're going to return right after anyway.
								lockGuard.Unlock();
								eventInternal->PutWaitResult(movedEventRef->WaitProcessed());
							}
							else
							{
								DoQueueEvent(event.Move());
							}

							// Stop execution of this event here, it'll be restarted from the beginning in 'ProcessPendingEvents'.
							return true;
						}
					}

					// Call the handler
					if (ExecuteEventHandler(event, eventItem, *evtHandler))
					{
						// If this is a one-shot event store its bind slot to unbind later
						const auto flags = eventItem.GetFlags();
						if (flags.Contains(BindEventFlag::OneShot))
						{
							eventSlot = eventItem.GetBindSlot();
						}

						// It's important to skip clearing of the unbound event entries
						// below because this object itself could have been deleted by
						// the event handler making 'm_EventTable' a dangling pointer
						// which can't be accessed any longer in the code below.
						//
						// In practice, it hopefully shouldn't be a problem to wait
						// until we get an event that we don't handle before clearing
						// because this should happen soon enough and even if it
						// doesn't the worst possible outcome is slightly increased
						// memory consumption while not skipping clearing can result in
						// hard to reproduce crashes (because it requires the disconnection
						// and deletion happen at the same time which is not always the case).
						//
						// Actually a user should use 'ICoreApplication::ScheduleForDestruction'
						// to destroy 'IEvtHandler' objects instead of just calling 'delete this'.
						// So assume we can never end up in a situation described above.
						executed = true;
						break;
					}
				}
			}
		}

		if (nullCount != 0 || eventSlot)
		{
			WriteLockGuard lockGuard(m_EventTableLock);
			
			// Unbind event if needed
			if (DoUnbind(eventSlot))
			{
				nullCount++;
			}

			// Purge the event table
			if (nullCount != 0)
			{
				Utility::Container::RemoveEachIf(m_EventTable, [](const EventItem& item)
				{
					return item.IsNull();
				});

				// Shrink vector only if we have deleted enough items to justify reallocation
				if (initialSize / 2 >= nullCount)
				{
					m_EventTable.shrink_to_fit();
				}
			}
		}
		return executed;
	}
	bool EvtHandler::ExecuteEventHandler(IEvent& event, EventItem& eventItem, IEvtHandler& evtHandler)
	{
		// Reset skip instruction
		event.Skip(false);

		// Call the handler
		eventItem.GetExecutor()->Execute(evtHandler, event);

		// Skip the event if we're required to always skip it
		if (eventItem.GetFlags().Contains(BindEventFlag::AlwaysSkip))
		{
			event.Skip();
		}

		// Return true if we processed this event and event handler itself didn't skipped it
		return !event.IsSkipped();
	}
	void EvtHandler::ConsumeException(IEvent& event)
	{
		auto app = ICoreApplication::GetInstance();
		try
		{
			if (!app || !app->OnMainLoopException())
			{
				// If 'OnMainLoopException' returns false, we're supposed to exit
				// the program and for this we need to exit the main loop, not the
				// possibly nested one we're running right now.
				if (app)
				{
					app->ExitMainLoop();
				}
				else
				{
					// We must not continue running after an exception, unless  explicitly requested,
					// so if we can't ensure this in any other way, do it brutally like this.
					std::terminate();
				}
			}
			// Continue running current event loop
		}
		catch (...)
		{
			// 'OnMainLoopException' threw, possibly rethrowing the same exception again. 
			// We have to deal with it here because we can't allow the exception to escape
			// from the handling code, this will result in a crash at best and in something
			// even more weird at worst (like exceptions completely disappearing into the void
			// under some 64-bit versions of Windows).
			if (app)
			{
				IEventLoop* eventLoop = app->GetActiveEventLoop();
				if (eventLoop && eventLoop->IsYielding())
				{
					eventLoop->Exit();
				}
			}

			// Give the application one last possibility to store the exception for rethrowing it later, when we get back to our code.
			bool exceptionStored = false;
			try
			{
				if (app)
				{
					exceptionStored = app->StoreCurrentException();
				}
			}
			catch (...)
			{
				// 'StoreCurrentException' really shouldn't throw, but if it did, take it as an indication that it didn't store it.
				exceptionStored = false;
			}

			// If it didn't take it, just abort, at least like this we behave consistently everywhere.
			if (!exceptionStored)
			{
				try
				{
					if (app)
					{
						app->OnUnhandledException();
					}
				}
				catch (...)
				{
					// And 'OnUnhandledException' absolutely shouldn't throw, but we still must account for the possibility that it
					// did. At least show some information about the exception in this case.
					Application::Private::OnUnhandledException();
				}
				std::terminate();
			}
		}
	}
	
	std::unique_ptr<IEvent> EvtHandler::DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID, const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags)
	{
		auto app = ICoreApplication::GetInstance();
		if (app && event)
		{
			PrepareEvent(*event, eventID, uuid, flags, true);

			if (WriteLockGuard lock(m_PendingEventsLock); true)
			{
				// Add this event to our list of pending events
				if (uuid)
				{
					// If this event has a unique ID, search for the last posted event with the same ID and,
					// if it'll be found, replace it with our new event, otherwise just add it at the end as usual.
					auto it = std::find_if(m_PendingEvents.rbegin(), m_PendingEvents.rend(), [&](const auto& item)
					{
						return item->GetUniqueID() == uuid;
					});
					if (it != m_PendingEvents.rend())
					{
						*it = std::move(event);
					}
					else
					{
						m_PendingEvents.emplace_back(std::move(event));
					}
				}
				else
				{
					m_PendingEvents.emplace_back(std::move(event));
				}

				// Add this event handler to the list of event handlers that have pending events
				app->AddPendingEventHandler(*this);

				// Only release 'm_PendingEventsLock' now because otherwise there is a race condition: we could process
				// the event just added to 'm_PendingEvents' in our 'ProcessPendingEvents' before we had time to append
				// this pointer to application's list; thus breaking the invariant that a handler should be in the list
				// if and only if it has any pending events to process.
			}

			// Inform the system that new pending events are somewhere, and that these should be processed in idle time.
			app->WakeUp();
		}

		return nullptr;
	}
	bool EvtHandler::DoProcessEvent(IEvent& event, const EventID& eventID, const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, IEvtHandler* onlyIn)
	{
		// Short-circuit for 'ProcessEventFlag::Locally' option
		if (!onlyIn && flags.Contains(ProcessEventFlag::Locally))
		{
			auto BeginLocally = [&]()
			{
				PrepareEvent(event, eventID, uuid, flags, false);
				return TryLocally(event);
			};
			if (flags.Contains(ProcessEventFlag::HandleExceptions))
			{
				try
				{
					return BeginLocally();
				}
				catch (...)
				{
					ConsumeException(event);
				}
			}
			else
			{
				return BeginLocally();
			}
			return false;
		}
		
		// Main entry point for event processing
		auto BeginProcessEvent = [&]()
		{
			PrepareEvent(event, eventID, uuid, flags, false);

			// The very first thing we do is to allow any registered filters to hook
			// into event processing in order to globally pre-process all events.
			//
			// Note that we should only do it if we're the first event handler called
			// to avoid calling 'FilterEvent' multiple times as the event goes through
			// the event handler chain and possibly upwards the window hierarchy.
			if (!event.QueryInterface<IEventInternal>()->WasProcessed())
			{
				using Result = IEventFilter::Result;
				if (auto app = ICoreApplication::GetInstance())
				{
					const Result result = app->FilterEvent(event);
					if (result != Result::Skip)
					{
						return result != Result::Ignore;
					}
					// Proceed normally
				}
			}

			// Short circuit the event processing logic if we're requested to process
			// this event in this handler only, see 'TryChain' for more details.
			if (this == onlyIn)
			{
				return TryBeforeAndHere(event);
			}

			// Try to process the event in this handler itself
			if (TryLocally(event))
			{
				// It is possible that 'TryChain' called from 'TryLocally' returned true but
				// the event was not really processed: this happens if a custom handler ignores
				// the request to process the event in this handler only and in this case we should
				// skip the post processing done in 'TryAfter' but still return the correct value
				// ourselves to indicate whether we did or did not find a handler for this event.
				return !event.IsSkipped();
			}

			// If we still didn't find a handler, propagate the event upwards the window chain and/or
			// to the application object.
			if (TryAfter(event))
			{
				return true;
			}

			// No handler found anywhere, bail out.
			return false;
		};
		if (flags.Contains(ProcessEventFlag::HandleExceptions))
		{
			try
			{
				return BeginProcessEvent();
			}
			catch (...)
			{
				ConsumeException(event);
			}
		}
		else
		{
			return BeginProcessEvent();
		}
		return false;
	}

	bool EvtHandler::TryBefore(IEvent& event)
	{
		return false;
	}
	bool EvtHandler::TryAfter(IEvent& event)
	{
		// We only want to pass the window to the application object once even if
		// there are several chained handlers. Ensure that this is what happens by
		// only calling 'DoTryApp' if there is no next handler (which would do it).
		//
		// Notice that, unlike simply calling TryAfter() on the last handler in the
		// chain only from ProcessEvent(), this also works with wxWindow object in
		// the middle of the chain: its overridden 'TryAfter' will still be called
		// and propagate the event upwards the window hierarchy even if it's not
		// the last one in the chain (which, admittedly, shouldn't happen often).
		if (IEvtHandler* next = GetNextHandler())
		{
			return EventSystem::EvtHandlerAccessor(*next).TryAfter(event);
		}

		// If this event is going to be processed in another handler next, don't pass it
		// to application instance now, it will be done from 'TryAfter' of this other handler.
		if (event.QueryInterface<IEventInternal>()->WillBeProcessedAgain())
		{
			return false;
		}
		return TryApp(event);
	}

	LocallyUniqueID EvtHandler::DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<BindEventFlag> flags)
	{
		if (executor && eventID && eventID != IEvent::EvtAny && eventID != IIndirectInvocationEvent::EvtIndirectInvocation)
		{
			// Following combinations makes no sense
			if (IsInvalidFlagCombination(flags))
			{
				wxFAIL_MSG("Invalid combination of 'BindEventFlag' values");
				return {};
			}

			// If none are present, assume direct, it's not a hard error to pass 'BindEventFlag::None' here.
			flags.Add(BindEventFlag::Direct, !flags.Contains(BindEventFlag::Direct) && !flags.Contains(BindEventFlag::Queued) && !flags.Contains(BindEventFlag::Auto));

			EventItem eventItem(eventID, std::move(executor));

			// Save flags but remove 'BindEventFlag::Unique' as it's only makes sense inside this function
			eventItem.SetFlags(flags.Clone().Remove(BindEventFlag::Unique));

			if (OnDynamicBind(eventItem) && eventItem)
			{
				WriteLockGuard lock(m_EventTableLock);

				// If 'BindEventFlag::Unique' is present, refuse to bind this handler if the same handler is already bound.
				if (flags.Contains(BindEventFlag::Unique) && CountBoundHandlers(m_EventTable, eventID, *eventItem.GetExecutor()) != 0)
				{
					return {};
				}

				// Test for the bind slots count, we can't allow this to overflow.
				const size_t nextBindSlot = m_EventBindSlot + 1;
				if (nextBindSlot == std::numeric_limits<size_t>::max())
				{
					throw std::runtime_error("[EvtHandler] exceeded maximum number of bind slots (std::numeric_limits<size_t>::max())");
				}

				m_EventBindSlot = nextBindSlot;
				m_EventTable.emplace_back(std::move(eventItem)).SetBindSlot(nextBindSlot);
				return nextBindSlot;
			}
		}
		return {};
	}
	bool EvtHandler::DoUnbind(const EventID& eventID, IEventExecutor& executor)
	{
		if (eventID)
		{
			if (WriteLockGuard lock(m_EventTableLock); !m_EventTable.empty())
			{
				size_t unbindCount = 0;
				SearchForBoundHandlers(m_EventTable, eventID, executor, [&](auto&& itemIt)
				{
					// Clear the handler if derived class allows that
					if (OnDynamicUnbind(*itemIt))
					{
						// We can't delete the entry from the vector if we're currently iterating over it.
						// As we don't know whether we're or not, just null it for now and we will really
						// erase it when we do finish iterating over it the next time.
						FreeBindSlot(itemIt->GetBindSlot());
						*itemIt = {};

						unbindCount++;
					}
					return true;
				});

				// If we were asked to unbind all items (or we've deleted them all) we can free all bind slots as well
				if (eventID == IEvent::EvtAny || unbindCount == m_EventTable.size())
				{
					FreeAllBindSlots();
				}
				return unbindCount != 0;
			}
		}
		return false;
	}
	bool EvtHandler::DoUnbind(const LocallyUniqueID& bindSlot)
	{
		if (bindSlot)
		{
			if (WriteLockGuard lock(m_EventTableLock); !m_EventTable.empty())
			{
				size_t unbindCount = 0;
				auto it = std::find_if(m_EventTable.rbegin(), m_EventTable.rend(), [&](const EventItem& item)
				{
					return item.GetBindSlot() == bindSlot;
				});
				if (it != m_EventTable.rend() && OnDynamicUnbind(*it))
				{
					// See the comment in the 'DoUnbind(EventID, IEventExecutor&)' overload above for why
					// we just null the event item and not erase it.
					FreeBindSlot(bindSlot);
					*it = {};

					unbindCount++;
					return true;
				}

				if (unbindCount == m_EventTable.size())
				{
					FreeAllBindSlots();
				}
				return unbindCount != 0;
			}
		}
		return false;
	}

	bool EvtHandler::ProcessPendingEvents()
	{
		// We need an event loop which manages the list of event handlers with pending events. Cannot proceed without it!
		if (auto app = ICoreApplication::GetInstance())
		{
			// We need to process only a single pending event in this call because each call to 'DoProcessEvent'
			// could result in the destruction of this same event handler (see the comment at the end of this function).
			std::unique_ptr<IEvent> pendingEvent;

			// This method is only called by an application if this handler does have pending events
			if (WriteLockGuard lock(m_PendingEventsLock); !m_PendingEvents.empty())
			{
				// Always get the first event
				auto pendingIt = m_PendingEvents.begin();

				// If we're inside 'Yield' call, process events selectively instead
				IEventLoop* eventLoop = app->GetActiveEventLoop();
				if (eventLoop && eventLoop->IsYielding())
				{
					// Find the first event which can be processed now
					auto it = m_PendingEvents.begin();
					for (; it != m_PendingEvents.end(); ++it)
					{
						if (eventLoop->IsEventAllowedInsideYield((*it)->GetEventCategory()))
						{
							pendingIt = std::move(it);
							break;
						}
					}

					if (it == m_PendingEvents.end())
					{
						// All our events are *not* processable now, signal this.
						app->DelayPendingEventHandler(*this);

						// See the comment at the beginning of 'IEventLoop' header for the logic behind YieldFor() and behind DelayPendingEventHandler().
						return false;
					}
				}

				// It's important we remove event from the list before processing it, else a nested event loop,
				// for example from a modal dialog, might process the same event again.
				pendingEvent = std::move(*pendingIt);
				m_PendingEvents.erase(pendingIt);

				if (m_PendingEvents.empty())
				{
					// If there are no more pending events left, we don't need to stay in this list.
					app->RemovePendingEventHandler(*this);
				}
			}
			
			if (pendingEvent)
			{
				auto pendingEventInternal = pendingEvent->QueryInterface<IEventInternal>();

				// Careful: this object could have been deleted by the event handler executed by the 'DoProcessEvent' call below,
				// so we can't access any fields of this object anymore.
				DoProcessEvent(*pendingEvent, {}, {}, pendingEventInternal->GetProcessFlags());

				// Signal to the waiting thread that we have processed this event and relinquish the event object to it.
				// Doesn't do anything if the event isn't waitable.
				pendingEventInternal->SignalProcessed(std::move(pendingEvent));

				// Should we return the result of 'DoProcessEvent' here? Probably not.
				return true;
			}
		}
		return false;
	}
	size_t EvtHandler::DiscardPendingEvents()
	{
		WriteLockGuard lock(m_PendingEventsLock);

		const size_t count = m_PendingEvents.size();
		m_PendingEvents.clear();
		return count;
	}

	void EvtHandler::Unlink()
	{
		// This event handler must take itself out of the chain of handlers
		IEvtHandler* prev = m_PrevHandler;
		IEvtHandler* next = m_NextHandler;
		if (prev)
		{
			prev->SetNextHandler(next);
		}
		if (next)
		{
			next->SetPrevHandler(prev);
		}

		m_PrevHandler = nullptr;
		m_NextHandler = nullptr;
	}
	bool EvtHandler::IsUnlinked() const
	{
		return m_PrevHandler == nullptr && m_NextHandler == nullptr;
	}
}
