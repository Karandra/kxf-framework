#pragma once
#include "Common.h"
#include "SystemProcess/ISystemProcess.h"
#include "SystemProcess/ISystemProcessStdIO.h"
#include "SystemProcess/SystemProcessInfo.h"
#include "SystemProcess/ProcessEvent.h"
#include "kxf/EventSystem/EvtHandlerDelegate.h"

namespace kxf::System
{
	KXF_API size_t EnumRunningProcesses(std::function<CallbackCommand(SystemProcess)> func);

	KXF_API std::unique_ptr<ISystemProcess> CreateProcess(const ISystemProcess& info, EvtHandlerDelegate evtHandler = {}, FlagSet<CreateSystemProcessFlag> flags = {});
}
