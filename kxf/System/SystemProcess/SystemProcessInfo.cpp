#include "kxf-pch.h"
#include "SystemProcessInfo.h"
#include "kxf/System/SystemProcess.h"
#include "kxf/System/SystemInformation.h"
#include <Windows.h>
#include "kxf/System/UndefWindows.h"

namespace kxf
{
	SystemProcess SystemProcess::GetCurrentProcess()
	{
		return ::GetCurrentProcessId();
	}

	bool SystemProcess::IsCurrent() const
	{
		return m_PID == ::GetCurrentProcessId();
	}
}

namespace kxf
{
	SystemProcessInfo SystemProcessInfo::GetCurrentProcess()
	{
		return RunningSystemProcess::GetCurrentProcess();
	}

	SystemProcessInfo::SystemProcessInfo(const ISystemProcess& info)
		:m_PID(info.GetID()),
		m_Is64Bit(info.Is64Bit()),
		m_ExecutablePath(info.GetExecutablePath()),
		m_Parameters(info.GetExecutableParameters()),
		m_WorkingDirectory(info.GetWorkingDirectory()),
		m_ShowWindowCommand(info.GetShowWindowCommand()),
		m_Priority(info.GetPriority())
	{
		info.EnumEnvironemntVariables([&](const String& name, const String& value)
		{
			m_Environment.insert_or_assign(name, value);
			return CallbackCommand::Continue;
		});
	}

	bool SystemProcessInfo::IsCurrent() const
	{
		return m_PID == ::GetCurrentProcessId();
	}
	String SystemProcessInfo::GetCommandLine() const
	{
		return Format("\"{}\" {}", m_ExecutablePath.GetFullPath(), m_Parameters);
	}
	std::unique_ptr<ISystemProcess> SystemProcessInfo::Spawn(EvtHandlerDelegate evtHandler, FlagSet<CreateSystemProcessFlag> flags)
	{
		return System::CreateProcess(*this, std::move(evtHandler), flags);
	}
}
