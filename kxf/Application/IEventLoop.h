#pragma once
#include "Common.h"
#include "kxf/Core/DateTime/TimeSpan.h"

/*
	NOTE ABOUT IEventLoop::YieldFor LOGIC

	The 'IEventLoop::YieldFor' function helps to avoid re-entrancy problems
	and problems caused by out-of-order event processing
	[WX: (see "wxYield-like problems" and "wxProgressDialog+threading BUG" wx-dev threads)].

	The logic behind 'IEventLoop::YieldFor' is simple: it analyzes the queue of the native events
	generated by the underlying GUI toolkit and picks out and processes only those matching the given mask.

	It's important to note that 'IEventLoop::YieldFor' is used to selectively process the events
	generated by the NATIVE toolkit. Events synthesized by wxWidgets code or by user code are instead selectively processed
	thanks to the logic built into 'IEvtHandler::ProcessPendingEvents'. In fact, when 'IEvtHandler::ProcessPendingEvents' gets
	called from inside an 'IEventLoop::YieldFor' call, 'EventLoopBase::IsEventAllowedInsideYield' is
	used to decide if the pending events for that event handler can be processed. If all the pending events associated with that
	event handler result as "not processable", the event handler "delays" itself calling 'ICoreApplication::DelayPendingEventHandler`
	(so it's moved: m_PendingEvtHandlers => m_DelayedPendingEvtHandlers). Last, 'ICoreApplication::ProcessPendingEvents'
	before exiting moves the delayed event handlers back into the list of handlers with pending events (m_DelayedPendingEvtHandlers =>
	m_PendingEvtHandlers) so that a later call to 'ProcessPendingEventHandlers' (possibly outside the 'YieldFor' call) will process
	all pending events as usual.
*/

namespace kxf
{
	enum class EventYieldFlag: uint32_t
	{
		None = 0,

		OnlyIfRequired = 1 << 0
	};
	kxf_FlagSet_Declare(EventYieldFlag);
}
namespace kxf::wxWidgets
{
	class EventLoopWrapperWx;
}

namespace kxf
{
	class KXF_API IEventLoop: public RTTI::Interface<IEventLoop>
	{
		kxf_RTTI_DeclareIID(IEventLoop, {0x7926acb6, 0x7016, 0x478c, {0x9d, 0xa3, 0x5f, 0x3e, 0xc1, 0xa1, 0xcc, 0xbb}});

		friend class ICoreApplication;
		friend class wxWidgets::EventLoopWrapperWx;

		public:
			enum class DispatchTimeout
			{
				Success = 1,
				ShouldExit = 0,
				Expired = -1
			};

		public:
			static size_t GetEventLoopCount() noexcept;

		protected:
			virtual void OnEnter();
			virtual void OnExit();
			virtual int OnRun() = 0;

			virtual void OnYieldFor(FlagSet<EventCategory> toProcess) = 0;
			virtual void OnNextIteration()
			{
			}

		public:
			IEventLoop();
			virtual ~IEventLoop();

		public:
			bool IsMain() const;
			bool IsRunning() const;
			virtual void* GetHandle() const = 0;

			virtual int Run() = 0;
			virtual void Exit(int exitCode = 0) = 0;
			virtual void ScheduleExit(int exitCode = 0) = 0;

			virtual void WakeUp() = 0;
			virtual bool Pending() = 0;
			virtual bool Dispatch() = 0;
			virtual DispatchTimeout Dispatch(TimeSpan timeout) = 0;
			virtual bool DispatchIdle() = 0;

			virtual bool IsYielding() const = 0;
			virtual bool IsEventAllowedInsideYield(FlagSet<EventCategory> eventCategory) const = 0;

			virtual bool Yield(FlagSet<EventYieldFlag> flags) = 0;
			virtual bool YieldFor(FlagSet<EventCategory> toProcess) = 0;
	};
}

namespace kxf
{
	class EventLoopActivator final
	{
		private:
			IEventLoop* m_PreviousLoop = nullptr;

		public:
			EventLoopActivator(IEventLoop& eventLoop) noexcept;
			EventLoopActivator(const EventLoopActivator&) = delete;
			~EventLoopActivator() noexcept;

		public:
			EventLoopActivator& operator=(const EventLoopActivator&) = delete;
	};
}
