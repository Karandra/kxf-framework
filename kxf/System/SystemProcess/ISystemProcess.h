#pragma once
#include "../Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/CallbackFunction.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class SystemThread;
	class SystemWindow;
}

namespace kxf
{
	class KXF_API ISystemProcess: public RTTI::Interface<ISystemProcess>
	{
		kxf_RTTI_DeclareIID(ISystemProcess, {0x3f3562cb, 0x5674, 0x4662, {0xa0, 0x45, 0x93, 0x28, 0x24, 0xfc, 0x33, 0x4c}});

		public:
			virtual ~ISystemProcess() = default;

		public:
			virtual bool IsNull() const = 0;
			virtual bool IsCurrent() const = 0;
			virtual bool Is64Bit() const = 0;
			virtual uint32_t GetID() const = 0;

			virtual SystemProcessPriority GetPriority() const = 0;
			virtual bool SetPriority(SystemProcessPriority priority) = 0;

			virtual bool IsRunning() const = 0;
			virtual std::optional<uint32_t> GetExitCode() const = 0;
			virtual bool Terminate(uint32_t exitCode) = 0;

			virtual bool Suspend() = 0;
			virtual bool Resume() = 0;

			virtual String GetCommandLine() const = 0;
			virtual FSPath GetExecutablePath() const = 0;
			virtual FSPath GetWorkingDirectory() const = 0;
			virtual String GetExecutableParameters() const = 0;
			virtual SHWindowCommand GetShowWindowCommand() const = 0;

			virtual size_t EnumEnvironemntVariables(std::function<CallbackCommand(const String&, const String&)> func) const = 0;
			virtual size_t EnumThreads(std::function<CallbackCommand(SystemThread)> func) const = 0;
			virtual size_t EnumWindows(std::function<CallbackCommand(SystemWindow)> func) const = 0;
	};
}
