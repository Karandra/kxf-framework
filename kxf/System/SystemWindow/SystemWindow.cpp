#include "kxf-pch.h"
#include "SystemWindow.h"
#include "../SystemThread/SystemThreadInfo.h"
#include "../SystemProcess/SystemProcessInfo.h"
#include "kxf/System/Private/Shell.h"
#include "kxf/System/NativeAPI.h"
#include "kxf/System/Win32Error.h"
#include "kxf/Utility/String.h"
#include "kxf/Utility/Drawing.h"
#include <Windows.h>
#include "kxf/System/UndefWindows.h"

namespace
{
	constexpr HWND ToHWND(void* handle) noexcept
	{
		return static_cast<HWND>(handle);
	}
}

namespace kxf
{
	SystemWindow SystemWindow::GetShellWindow()
	{
		return ::GetShellWindow();
	}
	SystemWindow SystemWindow::GetDesktopWindow()
	{
		return ::GetDesktopWindow();
	}
	SystemWindow SystemWindow::GetBroadcastWindow()
	{
		return HWND_BROADCAST;
	}
	SystemWindow SystemWindow::GetForegroundWindow()
	{
		return ::GetForegroundWindow();
	}
	SystemWindow SystemWindow::GetWindowFromPoint(const Point& position)
	{
		return ::WindowFromPoint({position.GetX(), position.GetY()});
	}
	SystemWindow SystemWindow::GetWindowFromPhysicalPoint(const Point& position)
	{
		return ::WindowFromPhysicalPoint({position.GetX(), position.GetY()});
	}

	uint32_t SystemWindow::GetID() const
	{
		return static_cast<uint32_t>(::GetWindowLongPtrW(ToHWND(m_Handle), GWLP_ID));
	}
	SystemThread SystemWindow::GetOwningThread() const
	{
		DWORD pid = 0;
		DWORD tid = ::GetWindowThreadProcessId(ToHWND(m_Handle), &pid);

		if (tid != 0)
		{
			return tid;
		}
		return {};
	}
	SystemProcess SystemWindow::GetOwningProcess() const
	{
		DWORD pid = 0;
		DWORD tid = ::GetWindowThreadProcessId(ToHWND(m_Handle), &pid);

		if (pid != 0)
		{
			return pid;
		}
		return {};
	}

	String SystemWindow::GetText() const
	{
		String result;
		constexpr size_t bufferLength = INT16_MAX / 8;

		if (GetOwningProcess().IsCurrent())
		{
			DWORD_PTR length = 0;

			Utility::StringBuffer buffer(result, bufferLength, true);
			::SendMessageTimeoutW(ToHWND(m_Handle), WM_GETTEXT, bufferLength, reinterpret_cast<LPARAM>(buffer.wc_str()), SMTO_ABORTIFHUNG, TimeSpan::Seconds(5).GetMilliseconds(), &length);
			result.resize(length);
		}
		else
		{
			size_t length = ::GetWindowTextW(ToHWND(m_Handle), Utility::StringBuffer(result, bufferLength, true), bufferLength);
			result.resize(length);
		}
		return result;
	}
	bool SystemWindow::SetText(const String& label)
	{
		if (GetOwningProcess().IsCurrent())
		{
			return ::SetWindowTextW(ToHWND(m_Handle), label.wc_str());
		}
		else
		{
			DWORD_PTR result = FALSE;
			::SendMessageTimeoutW(ToHWND(m_Handle), WM_SETTEXT, 0, label.wc_str().unsafe_intptr(), SMTO_ABORTIFHUNG, TimeSpan::Seconds(5).GetMilliseconds(), &result);

			return result;
		}
	}

	Rect SystemWindow::GetRect() const
	{
		RECT rect = {};
		if (::GetWindowRect(ToHWND(m_Handle), &rect))
		{
			return Utility::FromWindowsRect(rect);
		}
		return Rect::UnspecifiedRect();
	}
	Point SystemWindow::ScreenToClient(const Point& point) const
	{
		POINT p = {point.GetX(), point.GetY()};
		if (::ScreenToClient(ToHWND(m_Handle), &p))
		{
			return {p.x, p.y};
		}
		return Point::UnspecifiedPosition();
	}
	Point SystemWindow::ClientToScreen(const Point& point) const
	{
		POINT p = {point.GetX(), point.GetY()};
		if (::ClientToScreen(ToHWND(m_Handle), &p))
		{
			return {p.x, p.y};
		}
		return Point::UnspecifiedPosition();
	}
	Size SystemWindow::MapPoints(Rect& rect, const ISystemWindow& targetWindow) const
	{
		::SetLastError(ERROR_SUCCESS);

		auto r = Utility::ToWindowsRect(rect);
		int value = ::MapWindowPoints(ToHWND(m_Handle), ToHWND(targetWindow.IsNull() ? HWND_DESKTOP : targetWindow.GetHandle()), reinterpret_cast<POINT*>(&r), 2);

		if (::GetLastError() == ERROR_SUCCESS)
		{
			rect = Utility::FromWindowsRect(r);
			return {LOWORD(value), HIWORD(value)};
		}
		return Size::UnspecifiedSize();
	}

	std::optional<intptr_t> SystemWindow::GetValue(int index) const
	{
		::SetLastError(ERROR_SUCCESS);
		auto result = ::GetWindowLongPtrW(ToHWND(m_Handle), index);

		if (::GetLastError() == ERROR_SUCCESS)
		{
			return result;
		}
		return {};
	}
	std::optional<intptr_t> SystemWindow::SetValue(int index, intptr_t value)
	{
		::SetLastError(ERROR_SUCCESS);
		auto result = ::SetWindowLongPtrW(ToHWND(m_Handle), index, value);

		if (::GetLastError() == ERROR_SUCCESS)
		{
			return result;
		}
		return {};
	}

	bool SystemWindow::PostMessage(uint32_t message, intptr_t wParam, intptr_t lParam)
	{
		return ::PostMessageW(ToHWND(m_Handle), message, wParam, lParam);
	}
	std::optional<intptr_t> SystemWindow::SendMessage(uint32_t message, intptr_t wParam, intptr_t lParam, FlagSet<uint32_t> flags, TimeSpan timeout)
	{
		Win32Error::SetLastError(Win32Error::Success());

		if (timeout.IsPositive())
		{
			DWORD_PTR result = 0;
			auto status = ::SendMessageTimeoutW(ToHWND(m_Handle), message, wParam, lParam, flags.ToInt(), timeout.GetMilliseconds(), &result);
			auto lastError = Win32Error::GetLastError();

			if (status != 0 && lastError.IsSuccess())
			{
				return result;
			}
		}
		else
		{
			auto result = ::SendMessageW(ToHWND(m_Handle), message, wParam, lParam);
			if (Win32Error::GetLastError().IsSuccess())
			{
				return result;
			}
		}
		return {};
	}

	bool SystemWindow::Show(SHWindowCommand command, bool async)
	{
		if (async)
		{
			return ::ShowWindowAsync(ToHWND(m_Handle), *Shell::Private::MapSHWindowCommand(command));
		}
		else
		{
			return ::ShowWindow(ToHWND(m_Handle), *Shell::Private::MapSHWindowCommand(command));
		}
	}
	bool SystemWindow::Close()
	{
		if (m_Handle && m_Handle != HWND_BROADCAST)
		{
			if (::CloseWindow(ToHWND(m_Handle)))
			{
				m_Handle = nullptr;
				return true;
			}
		}
		return false;
	}
	bool SystemWindow::Destroy()
	{
		if (m_Handle && m_Handle != HWND_BROADCAST)
		{
			if (::DestroyWindow(ToHWND(m_Handle)))
			{
				m_Handle = nullptr;
				return true;
			}
		}
		return false;
	}

	RunningSystemProcess SystemWindow::OpenOwningProcess() const
	{
		if (NativeAPI::OleAcc::GetProcessHandleFromHwnd)
		{
			RunningSystemProcess process;
			process.AttachHandle(NativeAPI::OleAcc::GetProcessHandleFromHwnd(ToHWND(m_Handle)));

			return process;
		}
		return {};
	}
	bool SystemWindow::DoesExist() const
	{
		return m_Handle && ::IsWindow(ToHWND(m_Handle));
	}

	bool SystemWindow::IsBoradcastWindow() const
	{
		return m_Handle == HWND_BROADCAST;
	}
}
