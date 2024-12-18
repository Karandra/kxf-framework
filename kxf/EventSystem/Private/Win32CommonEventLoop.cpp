#include "kxf-pch.h"
#include "Win32CommonEventLoop.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/Log/ScopedLogger.h"
#include <Windows.h>

namespace kxf::EventSystem::Private
{
	Win32Message Win32CommonEventLoop::FromNativeMessage(const tagMSG& msg) noexcept
	{
		Win32Message message;
		message.WindowHandle = msg.hwnd;
		message.Message = msg.message;
		message.lParam = msg.lParam;
		message.wParam = msg.wParam;
		message.Time = TimeSpan::Milliseconds(msg.time);
		message.Point = {msg.pt.x, msg.pt.y};

		return message;
	}
	tagMSG Win32CommonEventLoop::ToNativeMessage(const Win32Message& message) noexcept
	{
		MSG msg = {};
		msg.hwnd = reinterpret_cast<HWND>(message.WindowHandle);
		msg.message = message.Message;
		msg.lParam = message.lParam;
		msg.wParam = message.wParam;
		msg.time = message.Time.GetMilliseconds();
		msg.pt = {message.Point.GetX(), message.Point.GetY()};

		return msg;
	}

	bool Win32CommonEventLoop::IsWakeUpRequested()
	{
		return ::WaitForSingleObject(m_WakeUpCondition.GetHandle(), 0) == WAIT_OBJECT_0;
	}
	uint32_t Win32CommonEventLoop::WaitForThread(void* threadHandle)
	{
		// The order is important here, the code using this function assumes that 'WAIT_OBJECT_0'
		// indicates the thread termination and anything else - the availability of an input event.
		// So the thread handle must come first.
		// Wait for any objects, not all of them. Without timeout and return as soon as there are any events (QS_*).

		const HANDLE handles[2] = {threadHandle, m_WakeUpCondition.GetHandle()};
		return ::MsgWaitForMultipleObjects(std::size(handles), handles, FALSE, INFINITE, QS_ALLINPUT|QS_ALLPOSTMESSAGE);
	}

	bool Win32CommonEventLoop::GetNextMessage(Win32Message& message)
	{
		return GetNextMessage(message, TimeSpan::Milliseconds(INFINITE)) == DispatchTimeout::Success;
	}
	IEventLoop::DispatchTimeout Win32CommonEventLoop::GetNextMessage(Win32Message& message, TimeSpan timeout)
	{
		// 'MsgWaitForMultipleObjects' won't notice any input which was already examined (e.g. using 'PeekMessageW')
		// but not yet removed from the queue so we need to remove any immediate messages manually.

		auto DoPeekMessage = [&]()
		{
			MSG msg = {};
			const bool result = ::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE);
			message = FromNativeMessage(msg);
			return result;
		};

		while (!DoPeekMessage())
		{
			const HANDLE handle = m_WakeUpCondition.GetHandle();
			const uint32_t waitStatus = ::MsgWaitForMultipleObjects(1, &handle, FALSE, timeout.GetMilliseconds(), QS_ALLINPUT|QS_ALLPOSTMESSAGE);

			switch (waitStatus)
			{
				case WAIT_TIMEOUT:
				{
					return DispatchTimeout::Expired;
				}
				case WAIT_OBJECT_0:
				{
					// We were woken up by a background thread, which means there is no actual input message available,
					// but we should still return to the event loop, so pretend there was 'WM_NULL' in the queue.
					message = {};

					return DispatchTimeout::Success;
				}
				case WAIT_OBJECT_0 + 1:
				{
					// Some message is supposed to be available, but spurious wake-ups are also possible,
					// so just return to the loop: either we'll get the message or start waiting again.
					break;
				}
				default:
				{
					Log::Debug("unexpected 'MsgWaitForMultipleObjects' return value '{}'", waitStatus);
					break;
				}
			};
		}

		if (message.Message != WM_QUIT)
		{
			return DispatchTimeout::Success;
		}
		return DispatchTimeout::ShouldExit;
	}

	void Win32CommonEventLoop::WakeUp()
	{
		m_WakeUpCondition.Signal();
	}
	bool Win32CommonEventLoop::Pending()
	{
		MSG msg;
		return ::PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
	}
}
