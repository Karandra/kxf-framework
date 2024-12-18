#include "kxf-pch.h"
#include "SystemProcess.h"
#include "SystemProcess/CreateProcessExecutor.h"

#include <PsAPI.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::System
{
	size_t EnumRunningProcesses(std::function<CallbackCommand(SystemProcess)> func)
	{
		DWORD buffer[4096] = {};
		DWORD validBufferSize = 0;

		if (EnumProcesses(buffer, std::size(buffer) * sizeof(DWORD), &validBufferSize))
		{
			const size_t totalCount = validBufferSize / sizeof(DWORD);

			size_t count = 0;
			for (size_t i = 0; i < totalCount; i++)
			{
				count++;
				if (std::invoke(func, buffer[i]) == CallbackCommand::Terminate)
				{
					break;
				}
			}
			return count;
		}
		return 0;
	}

	std::unique_ptr<ISystemProcess> CreateProcess(const ISystemProcess& info, EvtHandlerDelegate evtHandler, FlagSet<CreateSystemProcessFlag> flags)
	{
		IEvtHandler* evtHandlerRef = evtHandler.Get();
		auto executor = std::make_unique<CreateProcessExecutor>(std::move(evtHandler), flags);

		if (flags & CreateSystemProcessFlag::Async)
		{
			if (evtHandlerRef && executor->CreateProcess(info) && executor->Run() == wxTHREAD_NO_ERROR)
			{
				return executor;
			}
		}
		else if (executor->CreateProcess(info))
		{
			executor->RunHere();
			return executor;
		}
		return nullptr;
	}
}
