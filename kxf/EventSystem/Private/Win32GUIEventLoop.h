#pragma once
#include "../Common.h"
#include "Win32CommonEventLoop.h"
class wxWindow;

namespace kxf::EventSystem::Private
{
	class KXF_API Win32GUIEventLoop: public Win32CommonEventLoop
	{
		public:
			// Set the critical window: this is the window such that all the events except those
			// to this window (and its children) stop to be processed (typical examples: assert or
			// crash report dialog). Calling this function with nullptr argument restores the normal event handling.
			static void SetCriticalWindow(wxWindow* window);

			// Return true if there is no critical window or if this window is [a child of] the critical one.
			static bool AllowProcessing(wxWindow& window);

			// Check if the given window is a child of the current critical window (which must be non NULL)
			static bool IsChildOfCriticalWindow(wxWindow& window);

		private:
			// Array of messages used for temporary storage by 'YieldFor'
			std::vector<Win32Message> m_Messages;

		protected:
			void OnYieldFor(FlagSet<EventCategory> toProcess) override;
			void OnNextIteration() override;

			void ProcessMessage(Win32Message& message) override;
			bool PreProcessMessage(Win32Message& message) override;

		public:
			Win32GUIEventLoop(FlagSet<EventCategory> allowedToYield = EventCategory::Everything)
				:Win32CommonEventLoop(allowedToYield)
			{
			}

		public:
			bool Dispatch() override;
			DispatchTimeout Dispatch(TimeSpan timeout) override;
	};
}
