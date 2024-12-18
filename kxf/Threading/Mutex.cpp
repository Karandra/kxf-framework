#include "kxf-pch.h"
#include "Mutex.h"
#include "kxf/System/Win32Error.h"
#include "kxf/System/Private/System.h"
#include <Windows.h>

namespace kxf
{
	bool Mutex::DoCreate(const String& name, bool acquireInitially, KernelObjectNamespace ns) noexcept
	{
		if (!name.IsEmpty())
		{
			String fullName = System::Private::GetKernelObjectName(name, ns);
			m_Handle = ::CreateMutexW(nullptr, acquireInitially, fullName.wc_str());
		}
		else
		{
			m_Handle = ::CreateMutexW(nullptr, acquireInitially, nullptr);
		}
		return m_Handle != nullptr;
	}
	bool Mutex::DoOpen(const String& name, KernelObjectNamespace ns) noexcept
	{
		String fullName = System::Private::GetKernelObjectName(name, ns);
		m_Handle = ::OpenMutexW(SYNCHRONIZE, FALSE, fullName.wc_str());

		return m_Handle != nullptr;
	}

	bool Mutex::Acquire(const TimeSpan& timeout) noexcept
	{
		return ::WaitForSingleObject(m_Handle, timeout.IsPositive() ? timeout.GetMilliseconds() : INFINITE) == WAIT_OBJECT_0;
	}
	bool Mutex::Release() noexcept
	{
		return ::ReleaseMutex(m_Handle);
	}
	void Mutex::Destroy() noexcept
	{
		if (m_Handle)
		{
			::CloseHandle(m_Handle);
			m_Handle = nullptr;
		}
	}
}
