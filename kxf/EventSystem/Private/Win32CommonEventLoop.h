#pragma once
#include "../Common.h"
#include "../CommonEventLoop.h"
#include "kxf/Core/DateTime/TimeSpan.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/Threading/SynchronizedCondition.h"
struct tagMSG;

// Required by wxWidgets' 'ClassRegistrar'
#include "kxf/Win32/WrapperUnicode.h"
kxf_MakeWinUnicodeCallWrapper(RegisterClass);

namespace kxf::EventSystem::Private
{
	struct Win32Message final
	{
		void* WindowHandle = nullptr;
		uint32_t Message = 0;
		uintptr_t wParam = 0;
		uintptr_t lParam = 0;
		TimeSpan Time;
		Point Point;
	};
}

namespace kxf::EventSystem::Private
{
	class KXF_API Win32CommonEventLoop: public CommonEventLoop
	{
		public:
			static Win32Message FromNativeMessage(const tagMSG& msg) noexcept;
			static tagMSG ToNativeMessage(const Win32Message& message) noexcept;

		private:
			SynchronizedCondition m_WakeUpCondition;

		protected:
			// Return true if wake up was requested and not handled yet, i.e. if 'm_WakeUpEvent' is signaled.
			bool IsWakeUpRequested();

			// A method to wait for the termination of the specified (by its native handle) thread or any input
			// message arriving (in GUI case). Return value is 'WAIT_OBJECT_0' if the thread terminated, 'WAIT_OBJECT_0 + 1'
			// if a message arrived with anything else indicating an error.
			uint32_t WaitForThread(void* threadHandle);

			// Get the next message from queue and return true or return false if we got 'WM_QUIT' or an error occurred.
			bool GetNextMessage(Win32Message& message);

			// Same as above but with a timeout.
			DispatchTimeout GetNextMessage(Win32Message& message, TimeSpan timeout);

		protected:
			// Process a single message: calls 'PreProcessMessage' before dispatching it
			virtual void ProcessMessage(Win32Message& message) = 0;

			// Preprocess a message, return true if processed (i.e. no further dispatching required).
			virtual bool PreProcessMessage(Win32Message& message) = 0;

		public:
			Win32CommonEventLoop(FlagSet<EventCategory> allowedToYield = EventCategory::Everything)
				:CommonEventLoop(allowedToYield)
			{
			}

		public:
			void WakeUp() override;
			bool Pending() override;
	};
}
