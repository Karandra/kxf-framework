#include "kxf-pch.h"
#include "Win32ConsoleEventLoop.h"
#include <Windows.h>

namespace kxf::EventSystem::Private
{
	void Win32ConsoleEventLoop::ProcessMessage(Win32Message& message)
	{
		// Give us the chance to pre-process the message first
		if (!PreProcessMessage(message))
		{
			// If it wasn't done, dispatch it to the corresponding window

			MSG msg = ToNativeMessage(message);
			::DispatchMessageW(&msg);
		}
	}

	bool Win32ConsoleEventLoop::Dispatch()
	{
		Win32Message message;
		if (GetNextMessage(message))
		{
			ProcessMessage(message);
			return !ShouldExit();
		}
		return false;
	}
	IEventLoop::DispatchTimeout Win32ConsoleEventLoop::Dispatch(TimeSpan timeout)
	{
		Win32Message message;

		const DispatchTimeout result = GetNextMessage(message, timeout);
		if (result == DispatchTimeout::Success)
		{
			ProcessMessage(message);
			return ShouldExit() ? DispatchTimeout::ShouldExit : DispatchTimeout::Success;
		}
		return result;
	}
}
