#include "kxf-pch.h"
#include "RunningSystemThread.h"
#include "kxf/System/SystemProcess.h"
#include "kxf/System/SystemWindow.h"
#include "kxf/System/Private/System.h"
#include "kxf/System/NativeAPI.h"
#include "kxf/System/HResult.h"

#include <Windows.h>
#include <PsAPI.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf
{
	RunningSystemThread RunningSystemThread::GetCurrentThread()
	{
		RunningSystemThread thread;
		thread.AttachHandle(::GetCurrentThread());

		return thread;
	}
	RunningSystemThread RunningSystemThread::OpenCurrentThread(FlagSet<SystemThreadAccess> access, bool inheritHandle)
	{
		return RunningSystemThread(::GetCurrentThreadId(), access, inheritHandle);
	}

	// ISystemProcess
	bool RunningSystemThread::IsCurrent() const
	{
		return GetID() == ::GetCurrentThreadId();
	}
	bool RunningSystemThread::Is64Bit() const
	{
		return GetOwningProcess().Is64Bit();
	}
	uint32_t RunningSystemThread::GetID() const
	{
		return ::GetThreadId(m_Handle);
	}
	SystemProcess RunningSystemThread::GetOwningProcess() const
	{
		return ::GetProcessIdOfThread(m_Handle);
	}

	SystemThreadPriority RunningSystemThread::GetPriority() const
	{
		return ::GetThreadPriority(m_Handle);
	}
	bool RunningSystemThread::SetPriority(SystemThreadPriority priority)
	{
		return ::SetThreadPriority(m_Handle, *priority);
	}

	String RunningSystemThread::GetDescription() const
	{
		if (NativeAPI::Kernel32::GetThreadDescription)
		{
			wchar_t* ptr = nullptr;
			Utility::ScopeGuard atExit = [&]()
			{
				::LocalFree(ptr);
			};

			if (HResult(NativeAPI::Kernel32::GetThreadDescription(m_Handle, &ptr)).IsSuccess())
			{
				return ptr;
			}
		}
		return {};
	}
	bool RunningSystemThread::SetDescription(const String& description)
	{
		if (NativeAPI::Kernel32::SetThreadDescription)
		{
			return HResult(NativeAPI::Kernel32::SetThreadDescription(m_Handle, description.wc_str())).IsSuccess();
		}
		return false;
	}

	bool RunningSystemThread::IsRunning() const
	{
		DWORD exitCode = 0;
		return ::GetExitCodeThread(m_Handle, &exitCode) && exitCode == STILL_ACTIVE;
	}
	std::optional<uint32_t> RunningSystemThread::GetExitCode() const
	{
		DWORD exitCode = STILL_ACTIVE;
		if (::GetExitCodeThread(m_Handle, &exitCode) && exitCode != STILL_ACTIVE)
		{
			return exitCode;
		}
		return {};
	}
	bool RunningSystemThread::Terminate(uint32_t exitCode)
	{
		return ::TerminateThread(m_Handle, exitCode);
	}

	bool RunningSystemThread::Suspend()
	{
		return ::SuspendThread(m_Handle) != std::numeric_limits<DWORD>::max();
	}
	bool RunningSystemThread::Resume()
	{
		return ::ResumeThread(m_Handle) != std::numeric_limits<DWORD>::max();
	}

	size_t RunningSystemThread::EnumWindows(std::function<CallbackCommand(SystemWindow)> func) const
	{
		if (!IsNull())
		{
			return System::Private::EnumWindows([&](void* hwnd, uint32_t pid, uint32_t tid)
			{
				return std::invoke(func, hwnd);
			}, GetOwningProcess().GetID(), GetID());
		}
		return 0;
	}

	// RunningSystemThread
	bool RunningSystemThread::Open(uint32_t tid, FlagSet<SystemThreadAccess> access, bool inheritHandle)
	{
		if (!m_Handle)
		{
			m_Handle = ::OpenThread(*System::Private::MapSystemThreadAccess(access), inheritHandle, tid);
			return m_Handle != nullptr;
		}
		return false;
	}
	void RunningSystemThread::Close()
	{
		if (m_Handle)
		{
			::CloseHandle(m_Handle);
			m_Handle = nullptr;
		}
	}

	bool RunningSystemThread::PostMessage(uint32_t message, intptr_t wParam, intptr_t lParam)
	{
		return ::PostThreadMessageW(GetID(), message, wParam, lParam);
	}
}
