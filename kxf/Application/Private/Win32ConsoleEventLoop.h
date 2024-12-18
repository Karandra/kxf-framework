#pragma once
#include "../Common.h"
#include "Win32CommonEventLoop.h"

namespace kxf::EventSystem::Private
{
	class KXF_API Win32ConsoleEventLoop: public Win32CommonEventLoop
	{
		protected:
			void OnYieldFor(FlagSet<EventCategory> toProcess) override
			{
				Win32CommonEventLoop::OnYieldFor(toProcess);
			}
			
			bool PreProcessMessage(Win32Message& message) override
			{
				return false;
			}
			void ProcessMessage(Win32Message& message) override;

		public:
			Win32ConsoleEventLoop(FlagSet<EventCategory> allowedToYield = EventCategory::Everything)
				:Win32CommonEventLoop(allowedToYield)
			{
			}

		public:
			bool Dispatch() override;
			DispatchTimeout Dispatch(TimeSpan timeout) override;
	};
}
