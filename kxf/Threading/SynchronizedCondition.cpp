#include "kxf-pch.h"
#include "SynchronizedCondition.h"
#include <Windows.h>

namespace kxf
{
	SynchronizedCondition::SynchronizedCondition() noexcept
	{
		m_Handle = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
	}
	SynchronizedCondition::~SynchronizedCondition()
	{
		if (m_Handle)
		{
			::CloseHandle(m_Handle);
		}
	}

	bool SynchronizedCondition::Signal() noexcept
	{
		return ::SetEvent(m_Handle);
	}
}
