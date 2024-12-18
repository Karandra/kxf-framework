#include "kxf-pch.h"
#include "CommonEventLoop.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/Utility/ScopeGuard.h"

namespace kxf
{
	bool CommonEventLoop::DispatchEvents()
	{
		// Process pending framework events first as they correspond to low-level events
		// which happened before, i.e. typically pending events were queued by a previous
		// call to 'Dispatch' and if we didn't process them now the next call to it might
		// enqueue them again (as happens with e.g. socket events which would be generated
		// as long as there is input available on socket and this input is only removed from
		// it when pending event handlers are executed).

		auto app = ICoreApplication::GetInstance();
		if (app)
		{
			app->ProcessPendingEventHandlers();

			// One of the pending event handlers could have decided to exit the loop so check
			// for the flag before trying to dispatch more events (which could block indefinitely
			// if no more are coming).
			if (m_ShouldExit)
			{
				return false;
			}
		}

		// Rethrow any exceptions which could have been produced by the handlers ran by 'Dispatch'.
		Utility::ScopeGuard atExit = [&]()
		{
			if (app)
			{
				app->RethrowStoredException();
			}
		};
		return Dispatch();
	}

	int CommonEventLoop::OnRun()
	{
		// Enters a loop calling 'OnNextIteration', 'Pending' and 'Dispatch'
		// and terminating when '[Schedule]Exit' is called.
		//
		// We must ensure that 'OnExit' is called even if an exception is thrown
		// from inside 'ProcessEvents' but we must call it from 'Exit' in normal
		// situations because it is supposed to be called synchronously, 'wxModalEventLoop'
		// depends on this (so we can't just use ON_BLOCK_EXIT or something similar here)

		auto app = ICoreApplication::GetInstance();
		while (true)
		{
			try
			{
				// This is the event loop itself
				while (true)
				{
					// Give them the possibility to do whatever they want
					OnNextIteration();

					// Generate and process idle events for as long as we don't have anything
					// else to do, but stop doing this if 'Exit' is called by one of the idle handlers
					while (!m_ShouldExit && !Pending() && DispatchIdle())
					{
					}

					// if 'Exit' was called, don't dispatch any more events here.
					if (m_ShouldExit)
					{
						break;
					}

					// A message came or no more idle processing to do, dispatch  all the pending events
					// and call 'Dispatch' to wait for the next message.
					if (!DispatchEvents() || m_ShouldExit)
					{
						break;
					}
				}

				// Process any still pending events
				while (true)
				{
					bool hasMoreEvents = false;

					// We always dispatch events pending at the framework level: it may be important to do it
					// before the loop exits and e.g. the modal  dialog possibly referenced by these events
					// handlers is destroyed. It also shouldn't result in the problems described below for
					// the native events and while there is still a risk of never existing the loop due to an
					// endless  stream of events generated from the user-defined event handlers, we consider
					// that well-behaved programs shouldn't do this, and if they do, it's better to keep running
					// the loop than crashing after leaving it.
					if (app && app->ProcessPendingEventHandlers())
					{
						hasMoreEvents = true;
					}

					// For the underlying toolkit events, we only handle them when exiting the outermost event
					// loop but not when exiting nested loops. This is required at least under Windows where,
					// in case of a nested modal event loop, the modality has already been undone as Exit() had
					// been already called, so all UI elements  are re-enabled and if we dispatched events from
					// them here, we could end up reentering the same event handler that had shown the modal dialog
					// in the first place and showing the dialog second time before its first instance was destroyed,
					// resulting in a lot of fun.
					//
					// Also, unlike framework events above, it should be fine to dispatch the native events from the outer
					// event loop, as any events generated from outside the dialog itself (necessarily, as the dialog
					// is already hidden and about to be destroyed) shouldn't reference the dialog. Which is one of the
					// reasons we still dispatch them in the outermost event loop, to ensure they're still processed.
					// Another reason is that if we do have an endless stream of native events, e.g. because we have a
					// timer with a too short interval, it's arguably better to keep handling them instead of exiting.
					if (IEventLoop::GetEventLoopCount() == 1 && Pending())
					{
						Dispatch();
						hasMoreEvents = true;
					}

					if (!hasMoreEvents)
					{
						break;
					}
				}

				// Exit the outer loop as well
				break;
			}
			catch (...)
			{
				try
				{
					if (!app || !app->OnMainLoopException())
					{
						OnExit();
						break;
					}
					// Or else continue running the event loop
				}
				catch (...)
				{
					// 'OnMainLoopException' thrown something, possibly the same exception again, but we still need 'OnExit' to be called.
					OnExit();
					throw;
				}
			}
		}
		return m_ExitCode;
	}
	void CommonEventLoop::OnYieldFor(FlagSet<EventCategory> toProcess)
	{
		// Normally yielding dispatches not only the pending native events, but  also the events pending
		// in the framework itself and idle events.
		//
		// Notice however that we must not do it if we're asked to process only the events of specific
		// kind, as pending events could be of any kind at all (ideal would be to have a filtering
		// version of 'ProcessPendingEventHandlers' too) and idle events are typically unexpected when yielding
		// for the specific event kinds only.
		if (toProcess == EventCategory::Everything)
		{
			if (auto app = ICoreApplication::GetInstance())
			{
				app->ProcessPendingEventHandlers();
			}

			// We call it just once, even if it returns true, because we don't want to get stuck inside
			// 'Yield' forever if the application does some constant background processing in its idle
			// handler, we do need to get back to the main loop soon.
			DispatchIdle();
		}
	}

	int CommonEventLoop::Run()
	{
		// Event loops aren't recursive, you need to create another loop.
		if (!m_IsInsideRun)
		{
			// 'Dispatch' and 'DispatchIdle' below may throw so the code here should be exception-safe,
			// hence we must use local objects for all actions we should undo.
			EventLoopActivator activate(*this);

			// We might be called again, after a previous call to 'ScheduleExit', so reset this flag.
			m_ShouldExit = false;

			// Set this variable to true for the duration of this function
			m_IsInsideRun = true;
			Utility::ScopeGuard onExit = [&]()
			{
				m_IsInsideRun = false;
				UpdateWxLoop();
			};
			UpdateWxLoop();

			// Finally run the event loop
			return OnRun();
		}
		return -1;
	}
	void CommonEventLoop::Exit(int exitCode)
	{
		// TODO: Think about merging this function with 'ScheduleExit'.
		// It doesn't look like it makes any sense to have such separation.
		ScheduleExit(exitCode);
	}
	void CommonEventLoop::ScheduleExit(int exitCode)
	{
		if (m_IsInsideRun)
		{
			m_ExitCode = exitCode;
			m_ShouldExit = true;
			UpdateWxLoop();

			OnExit();

			// All we have to do to exit from the loop is to (maybe) wake it up so that
			// it can notice that 'Exit' had been called.
			//
			// in particular, do *not* use here calls such as 'PostQuitMessage' (under Windows)
			// which terminate the current event loop here because we're not sure that it is
			// going to be processed by the correct event loop: it would be possible that another
			// one is started and terminated by mistake if we do this.
			WakeUp();
		}
	}
	bool CommonEventLoop::DispatchIdle()
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			return app->DispatchIdle();
		}
		return false;
	}
	
	bool CommonEventLoop::Yield(FlagSet<EventYieldFlag> flags)
	{
		if (flags.Contains(EventYieldFlag::OnlyIfRequired) && IsYielding())
		{
			return false;
		}
		return YieldFor(EventCategory::Everything);
	}
	bool CommonEventLoop::YieldFor(FlagSet<EventCategory> toProcess)
	{
		// Don't ever dispatch events from non-main threads
		if (wxThread::IsMain())
		{
			// Set the value and don't forget to reset it before returning
			Utility::ScopeGuard onExit = [&, yieldLevelOld = m_YieldLevel, oldAllowedToYield = m_AllowedToYield]()
			{
				m_YieldLevel = yieldLevelOld;
				m_AllowedToYield = oldAllowedToYield;

				UpdateWxLoop();
			};
			m_YieldLevel++;
			m_AllowedToYield = toProcess;
			UpdateWxLoop();

			// Disable log flushing from here because a call to 'Yield' shouldn't  normally result in message boxes popping up
			// and ensure the logs will be flashed again when we exit.
			wxLog::Suspend();
			Utility::ScopeGuard resumeLog = [&]()
			{
				wxLog::Resume();
			};

			OnYieldFor(toProcess);

			// If any handlers called from inside 'OnYieldFor' threw exceptions, they may have been stored
			// for later rethrow as it's unsafe to let them escape from inside 'DoYield' itself, as it
			// calls native functions through which the exceptions can't propagate. But now that we're
			// back to our own code, we may rethrow them.
			if (auto app = ICoreApplication::GetInstance())
			{
				app->RethrowStoredException();
			}
			return true;
		}
		return false;
	}
}
