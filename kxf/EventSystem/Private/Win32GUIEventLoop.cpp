#include "kxf-pch.h"
#include "Win32GUIEventLoop.h"
#include "kxf/Application/ICoreApplication.h"

Kx_MakeWinUnicodeCallWrapper(SendMessage);
#include "wx/msw/private.h"
#include "wx/evtloop.h"
#include <Windows.h>

namespace
{
	// Only the main thread can process Windows messages
	bool g_HadGUILock = true;
	std::vector<kxf::EventSystem::Private::Win32Message> g_SavedMessages;

	wxWindow* g_CriticalWindow = nullptr;
}

namespace kxf::EventSystem::Private
{
	void Win32GUIEventLoop::SetCriticalWindow(wxWindow* window)
	{
		g_CriticalWindow = window;
	}
	bool Win32GUIEventLoop::AllowProcessing(wxWindow& window)
	{
		return g_CriticalWindow == nullptr || IsChildOfCriticalWindow(window);
	}
	bool Win32GUIEventLoop::IsChildOfCriticalWindow(wxWindow& window)
	{
		if (g_CriticalWindow)
		{
			for (wxWindow* thisWindow = &window ; thisWindow; thisWindow = thisWindow->GetParent())
			{
				if (thisWindow == g_CriticalWindow)
				{
					return true;
				}
			}
		}
		return false;
	}

	void Win32GUIEventLoop::OnYieldFor(FlagSet<EventCategory> toProcess)
	{
		// We don't want to process 'WM_QUIT' from here - it should be processed in the main event loop in order to stop it.
		MSG nativeMessage = {};
		size_t countOfPaintEvents = 0;
		while (::PeekMessageW(&nativeMessage, nullptr, 0, 0, PM_NOREMOVE) && nativeMessage.message != WM_QUIT)
		{
			wxMutexGuiLeaveOrEnter();

			if (nativeMessage.message == WM_PAINT)
			{
				// 'WM_PAINT' messages are categorized as 'EventCategory::UI'
				if (!toProcess.Contains(EventCategory::UI))
				{
					// This 'nativeMessage' is not going to be dispatched however 'WM_PAINT' is special:
					// until there are damaged windows, Windows will keep sending it forever!
					if (countOfPaintEvents > 10)
					{
						// We got 10 WM_PAINT consecutive messages, we must have reached the tail of the
						// message queue: we're now getting *only* 'WM_PAINT' events and this will continue
						// forever (since we don't dispatch them because of the user-specified 'toProcess' mask),
						// so break out of this loop!
						break;
					}
					else
					{
						countOfPaintEvents++;
					}
				}
				// Else we're going to dispatch it below, so we don't need to take any special action.
			}
			else
			{
				// Reset the counter of consecutive 'WM_PAINT' messages received
				countOfPaintEvents = 0;
			}

			// Choose a wxEventCategory for this Windows message
			bool processNow = false;
			switch (nativeMessage.message)
			{
				case WM_NCMOUSEMOVE:

				case WM_NCLBUTTONDOWN:
				case WM_NCLBUTTONUP:
				case WM_NCLBUTTONDBLCLK:
				case WM_NCRBUTTONDOWN:
				case WM_NCRBUTTONUP:
				case WM_NCRBUTTONDBLCLK:
				case WM_NCMBUTTONDOWN:
				case WM_NCMBUTTONUP:
				case WM_NCMBUTTONDBLCLK:

				case WM_KEYDOWN:
				case WM_KEYUP:
				case WM_CHAR:
				case WM_DEADCHAR:
				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
				case WM_SYSCHAR:
				case WM_SYSDEADCHAR:
				case WM_UNICHAR:
				case WM_HOTKEY:
				case WM_IME_STARTCOMPOSITION:
				case WM_IME_ENDCOMPOSITION:
				case WM_IME_COMPOSITION:
				case WM_COMMAND:
				case WM_SYSCOMMAND:

				case WM_IME_SETCONTEXT:
				case WM_IME_NOTIFY:
				case WM_IME_CONTROL:
				case WM_IME_COMPOSITIONFULL:
				case WM_IME_SELECT:
				case WM_IME_CHAR:
				case WM_IME_KEYDOWN:
				case WM_IME_KEYUP:

				case WM_MOUSEHOVER:
				case WM_MOUSELEAVE:
				case WM_NCMOUSELEAVE:

				case WM_CUT:
				case WM_COPY:
				case WM_PASTE:
				case WM_CLEAR:
				case WM_UNDO:

				case WM_MOUSEMOVE:
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
				case WM_RBUTTONDBLCLK:
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
				case WM_MBUTTONDBLCLK:
				case WM_MOUSEWHEEL:
				{
					processNow = toProcess.Contains(EventCategory::UserInput);
					break;
				}
				case WM_TIMER:
				{
					toProcess.Contains(EventCategory::Timer);
					break;
				}
				default:
				{
					if (nativeMessage.message < WM_USER)
					{
						// [0; WM_USER) is the range of message IDs reserved for use by the system.
						// There are too many of these types of messages to handle them in this switch.
						processNow = toProcess.Contains(EventCategory::UI);
					}
					else
					{
						// Process all the unknown messages. We must do it because failure to process
						// some of them can be fatal, e.g. if we don't dispatch 'WM_APP + 2' then embedded
						// IE ActiveX controls don't work any more, see [WX: #14027]. And there may be more
						// examples like this, so dispatch all unknown messages immediately to be safe.
						processNow = true;
					}
					break;
				}
			};

			// Should we process this event now?
			if (processNow)
			{
				if (!wxApp::GetInstance()->Dispatch())
				{
					break;
				}
			}
			else
			{
				// Remove the message and store it
				::GetMessageW(&nativeMessage, nullptr, 0, 0);
				m_Messages.emplace_back(FromNativeMessage(nativeMessage));
			}
		}
		CommonEventLoop::YieldFor(toProcess);

		// Put back unprocessed events in the queue
		const uint32_t currentThread = GetCurrentThreadId();
		for (const Win32Message& message: m_Messages)
		{
			::PostThreadMessageW(currentThread, message.Message, message.wParam, message.lParam);
		}
		m_Messages.clear();
	}
	void Win32GUIEventLoop::OnNextIteration()
	{
		wxMutexGuiLeaveOrEnter();
	}
	
	void Win32GUIEventLoop::ProcessMessage(Win32Message& message)
	{
		// Give us the chance to pre-process the message first
		if (!PreProcessMessage(message))
		{
			// If it wasn't done, dispatch it to the corresponding window

			MSG msg = ToNativeMessage(message);
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
	}
	bool Win32GUIEventLoop::PreProcessMessage(Win32Message& message)
	{
		HWND windowHandle = reinterpret_cast<HWND>(message.WindowHandle);
		wxWindow* thisWindow = wxGetWindowFromHWND(windowHandle);
		wxWindow* window = nullptr;
		MSG msg = ToNativeMessage(message);

		// This might happen if we're in a modeless dialog, or if a control has children which themselves
		// were not created by the framework (ActiveX control children).
		if (!thisWindow)
		{
			while (windowHandle && (::GetWindowLongPtrW(windowHandle, GWL_STYLE) & WS_CHILD))
			{
				windowHandle = ::GetParent(windowHandle);

				// If the control has a framework parent, break and give the parent a chance to process the window message
				thisWindow = wxGetWindowFromHWND(windowHandle);
				if (thisWindow)
				{
					break;
				}
			}

			if (!thisWindow)
			{
				// This may happen if the event occurred in a standard modeless dialog (find/replace dialog)
				// then call 'IsDialogMessage' to make TAB navigation in it work.
				//
				// NOTE: IsDialogMessage() just eats all the messages (i.e. returns true for them) if we call
				// it for the control itself.
				return windowHandle && ::IsDialogMessageW(windowHandle, &msg) != 0;
			}
		}

		if (!AllowProcessing(*thisWindow))
		{
			// Not a child of critical window, so we eat the event but take care to stop an endless stream
			// of 'WM_PAINT' messages which would have resulted if we didn't validate the invalidated part of the window.
			if (message.Message == WM_PAINT)
			{
				::ValidateRect(windowHandle, nullptr);
			}
			return true;
		}

		// We must relay 'WM_MOUSEMOVE' events to the tooltip ctrl if we want it to popup the tooltip bubbles
		if (message.Message == WM_MOUSEMOVE)
		{
			// We should do it if one of window children has an associated tooltip (and not just if the window has a tooltip itself).
			if (thisWindow->HasToolTips())
			{
				wxToolTip::RelayEvent(&msg);
			}
		}

		// Allow the window to prevent certain messages from being translated/processed (this is currently used
		// by 'wxTextCtrl' to always grab Ctrl-C/V/X, even if they are also accelerators in some parent).
		if (!thisWindow->MSWShouldPreProcessMessage(&msg))
		{
			return false;
		}

		// Try translations first: the accelerators override everything
		for (window = thisWindow; window; window = window->GetParent())
		{
			if (window->MSWTranslateMessage(&msg))
			{
				return true;
			}

			// Stop at top navigation domain, i.e. typically a top level window
			if (window->IsTopNavigationDomain(wxWindow::Navigation_Accel))
			{
				break;
			}
		}

		// Now try the other hooks (keyboard navigation is handled here)
		for (window = thisWindow; window; window = window->GetParent())
		{
			if (window->MSWProcessMessage(&msg))
			{
				return true;
			}

			// Also stop at first top level window here, just as above because if we don't do this,
			// pressing ESC on a modal dialog shown as child of a modal dialog with 'wxID_CANCEL' will
			// cause the parent dialog to be closed, for example.
			if (window->IsTopNavigationDomain(wxWindow::Navigation_Accel))
			{
				break;
			}
		}

		// No special preprocessing for this message, dispatch it normally.
		return false;
	}

	bool Win32GUIEventLoop::Dispatch()
	{
		Win32Message message;
		if (!GetNextMessage(message))
		{
			return false;
		}

		// If a secondary thread owning the mutex is doing GUI calls, save all messages for later
		// processing - we can't process them right now because it will lead to recursive library
		// calls (and we're not reentrant).
		if (!wxGuiOwnedByMainThread())
		{
			g_HadGUILock = false;

			// Leave out 'WM_COMMAND' messages: too dangerous, sometimes the message will be processed twice.
			if (!wxIsWaitingForThread() || message.Message != WM_COMMAND)
			{
				g_SavedMessages.emplace_back(std::move(message));
			}
			return true;
		}
		else
		{
			// Have we just regained the GUI lock? If so, post all of the saved messages.
			// FIXME: Of course, it's not *exactly* the same as processing the messages normally, expect some things to break...
			if (!g_HadGUILock)
			{
				g_HadGUILock = true;

				for (auto it = g_SavedMessages.begin(); it != g_SavedMessages.end(); it = g_SavedMessages.begin())
				{
					Win32Message savedMessage = std::move(*it);
					g_SavedMessages.erase(it);

					ProcessMessage(savedMessage);
				}
			}
		}
		ProcessMessage(message);

		return true;
	}
	IEventLoop::DispatchTimeout Win32GUIEventLoop::Dispatch(TimeSpan timeout)
	{
		Win32Message message;

		const DispatchTimeout result = GetNextMessage(message, timeout);
		if (result == DispatchTimeout::Success)
		{
			ProcessMessage(message);
			return DispatchTimeout::Success;
		}
		return result;
	}
}
