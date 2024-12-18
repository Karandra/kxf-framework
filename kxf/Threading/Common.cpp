#include "kxf-pch.h"
#include "Common.h"
#include <thread>
#include <wx/thread.h>
#include <Windows.h>

namespace kxf::Threading
{
	bool IsMainThread() noexcept
	{
		return wxThread::IsMain();
	}
	uint32_t GetCurrentThreadID() noexcept
	{
		return ::GetCurrentThreadId();
	}
	uint32_t GetHardwareConcurrency() noexcept
	{
		return std::thread::hardware_concurrency();
	}
}
