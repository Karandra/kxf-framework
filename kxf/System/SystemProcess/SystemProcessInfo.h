#pragma once
#include "ISystemProcess.h"
#include "RunningSystemProcess.h"
#include "kxf/EventSystem/EvtHandlerDelegate.h"
#include <unordered_map>

namespace kxf
{
	class KXF_API SystemProcess: public ISystemProcess
	{
		public:
			static SystemProcess GetCurrentProcess();

		protected:
			uint32_t m_PID = std::numeric_limits<uint32_t>::max();

		public:
			SystemProcess() = default;
			SystemProcess(uint32_t pid)
				:m_PID(pid)
			{
			}

		public:
			// ISystemProcess
			bool IsNull() const override
			{
				return m_PID == std::numeric_limits<uint32_t>::max();
			}
			bool IsCurrent() const override;
			bool Is64Bit() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).Is64Bit();
			}
			uint32_t GetID() const override
			{
				return m_PID;
			}

			SystemProcessPriority GetPriority() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).GetPriority();
			}
			bool SetPriority(SystemProcessPriority priority) override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::SetInformation).SetPriority(priority);
			}

			bool IsRunning() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).IsRunning();
			}
			std::optional<uint32_t> GetExitCode() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).GetExitCode();
			}
			bool Terminate(uint32_t exitCode) override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::Terminate).Terminate(exitCode);
			}

			bool Suspend() override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::SuspendResume).Suspend();
			}
			bool Resume() override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::SuspendResume).Resume();
			}

			String GetCommandLine() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryInformation|SystemProcessAccess::VMRead).GetCommandLine();
			}
			FSPath GetExecutablePath() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).GetExecutablePath();
			}
			FSPath GetWorkingDirectory() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).GetWorkingDirectory();
			}
			String GetExecutableParameters() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryInformation|SystemProcessAccess::VMRead).GetExecutableParameters();
			}
			SHWindowCommand GetShowWindowCommand() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).GetShowWindowCommand();
			}

			size_t EnumEnvironemntVariables(std::function<CallbackCommand(const String&, const String&)> func) const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::None).EnumEnvironemntVariables(std::move(func));
			}
			size_t EnumThreads(std::function<CallbackCommand(SystemThread)> func) const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::None).EnumThreads(std::move(func));
			}
			size_t EnumWindows(std::function<CallbackCommand(SystemWindow)> func) const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::None).EnumWindows(std::move(func));
			}

			// SystemProcess
			RunningSystemProcess Open(SystemProcessAccess access, bool inheritHandle = false) const
			{
				return RunningSystemProcess(m_PID, access, inheritHandle);
			}
			bool SafeTerminate(uint32_t exitCode)
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation|SystemProcessAccess::DuplicateHandle).SafeTerminate(exitCode);
			}

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}

			auto operator<=>(const SystemProcess& other) const noexcept
			{
				return m_PID <=> other.m_PID;
			}
			bool operator==(const SystemProcess& other) const noexcept
			{
				return m_PID == other.m_PID;
			}
	};
}

namespace kxf
{
	class KXF_API SystemProcessInfo: public ISystemProcess
	{
		public:
			static SystemProcessInfo GetCurrentProcess();

		private:
			std::unordered_map<String, String> m_Environment;
			SystemProcessPriority m_Priority = SystemProcessPriority::None;
			SHWindowCommand m_ShowWindowCommand = SHWindowCommand::None;
			FSPath m_ExecutablePath;
			FSPath m_WorkingDirectory;
			String m_Parameters;
			uint32_t m_PID = std::numeric_limits<uint32_t>::max();
			bool m_Is64Bit = false;

		public:
			SystemProcessInfo() = default;
			SystemProcessInfo(uint32_t pid, bool is64Bit)
				:m_PID(pid), m_Is64Bit(is64Bit)
			{
			}
			SystemProcessInfo(const ISystemProcess& info);

		public:
			// ISystemProcess
			bool IsNull() const override
			{
				return m_PID == std::numeric_limits<uint32_t>::max();
			}
			bool IsCurrent() const override;
			bool Is64Bit() const override
			{
				return m_Is64Bit;
			}
			uint32_t GetID() const override
			{
				return m_PID;
			}

			SystemProcessPriority GetPriority() const override
			{
				return m_Priority;
			}
			bool SetPriority(SystemProcessPriority priority) override
			{
				m_Priority = priority;
				return true;
			}

			bool IsRunning() const override
			{
				return false;
			}
			std::optional<uint32_t> GetExitCode() const override
			{
				return {};
			}
			bool Terminate(uint32_t exitCode) override
			{
				return false;
			}

			bool Suspend() override
			{
				return false;
			}
			bool Resume() override
			{
				return false;
			}

			String GetCommandLine() const override;
			FSPath GetExecutablePath() const override
			{
				return m_ExecutablePath;
			}
			FSPath GetWorkingDirectory() const override
			{
				return m_WorkingDirectory;
			}
			String GetExecutableParameters() const override
			{
				return m_Parameters;
			}
			SHWindowCommand GetShowWindowCommand() const override
			{
				return m_ShowWindowCommand;
			}

			size_t EnumEnvironemntVariables(std::function<CallbackCommand(const String&, const String&)> func) const override
			{
				size_t count = 0;
				for (const auto& [name, value]: m_Environment)
				{
					count++;
					if (std::invoke(func, name, value) == CallbackCommand::Terminate)
					{
						break;
					}
				}
				return count;
			}
			size_t EnumThreads(std::function<CallbackCommand(SystemThread)> func) const override
			{
				return 0;
			}
			size_t EnumWindows(std::function<CallbackCommand(SystemWindow)> func) const override
			{
				return 0;
			}

			// SystemProcessInfo
			void SetExecutablePath(FSPath path)
			{
				m_ExecutablePath = std::move(path);
			}
			void SetWorkingDirectory(FSPath directory)
			{
				m_WorkingDirectory = std::move(directory);
			}
			void SetExecutableParameters(String parameters)
			{
				m_Parameters = std::move(parameters);
			}
			bool SetShowWindowCommand(SHWindowCommand value)
			{
				m_ShowWindowCommand = value;
				return true;
			}

			void SetEnvironmentVariable(const String& name, const String& value)
			{
				m_Environment.insert_or_assign(name, value);
			}
			void ClearEnvironmentVariables()
			{
				m_Environment.clear();
			}

			std::unique_ptr<ISystemProcess> Spawn(EvtHandlerDelegate evtHandler = {}, FlagSet<CreateSystemProcessFlag> flags = {});
	};
}

namespace std
{
	template<>
	struct hash<kxf::SystemProcess> final
	{
		size_t operator()(const kxf::SystemProcess& process) const noexcept
		{
			std::hash<uint32_t> calc;
			return calc(process.GetID());
		}
	};
}
