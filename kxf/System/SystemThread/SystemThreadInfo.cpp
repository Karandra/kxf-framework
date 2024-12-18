#include "kxf-pch.h"
#include "SystemThreadInfo.h"
#include "kxf/System/SystemProcess.h"
#include <Windows.h>
#include "kxf/System/UndefWindows.h"

namespace kxf
{
	SystemThread SystemThread::GetCurrentThread()
	{
		return ::GetCurrentThreadId();
	}

	bool SystemThread::IsCurrent() const
	{
		return m_TID == ::GetCurrentThreadId();
	}
	SystemProcess SystemThread::GetOwningProcess() const
	{
		return RunningSystemThread(m_TID, SystemThreadAccess::QueryLimitedInformation).GetOwningProcess();
	}
}

namespace kxf
{
	SystemThreadInfo SystemThreadInfo::GetCurrentThread()
	{
		return RunningSystemThread::GetCurrentThread();
	}

	SystemThreadInfo::SystemThreadInfo(const ISystemThread& info)
		:m_OwningProcess(info.GetOwningProcess()),
		m_TID(info.GetID()),
		m_Is64Bit(info.Is64Bit()),
		m_Priority(info.GetPriority()),
		m_Description(info.GetDescription())
	{
	}

	bool SystemThreadInfo::IsCurrent() const
	{
		return m_TID == ::GetCurrentThreadId();
	}
}
