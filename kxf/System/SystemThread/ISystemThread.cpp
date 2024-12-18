#include "kxf-pch.h"
#include "ISystemThread.h"
#include <Windows.h>

#include <PsAPI.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf
{
	SystemThreadPriority SystemThreadPriority::Idle() noexcept
	{
		return THREAD_PRIORITY_IDLE;
	}
	SystemThreadPriority SystemThreadPriority::Lowest() noexcept
	{
		return THREAD_PRIORITY_LOWEST;
	}
	SystemThreadPriority SystemThreadPriority::BelowNormal() noexcept
	{
		return THREAD_PRIORITY_BELOW_NORMAL;
	}
	SystemThreadPriority SystemThreadPriority::Normal() noexcept
	{
		return THREAD_PRIORITY_NORMAL;
	}
	SystemThreadPriority SystemThreadPriority::AboveNormal() noexcept
	{
		return THREAD_PRIORITY_ABOVE_NORMAL;
	}
	SystemThreadPriority SystemThreadPriority::Highest() noexcept
	{
		return THREAD_PRIORITY_HIGHEST;
	}
	SystemThreadPriority SystemThreadPriority::TimeCritical() noexcept
	{
		return THREAD_PRIORITY_TIME_CRITICAL;
	}

	bool SystemThreadPriority::IsStandardValue() const noexcept
	{
		switch (m_Value)
		{
			case THREAD_PRIORITY_IDLE:
			case THREAD_PRIORITY_LOWEST:
			case THREAD_PRIORITY_BELOW_NORMAL:
			case THREAD_PRIORITY_NORMAL:
			case THREAD_PRIORITY_ABOVE_NORMAL:
			case THREAD_PRIORITY_HIGHEST:
			case THREAD_PRIORITY_TIME_CRITICAL:
			{
				return true;
			}
		};
		return false;
	}
}
