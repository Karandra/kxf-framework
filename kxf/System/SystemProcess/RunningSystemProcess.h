#pragma once
#include "ISystemProcess.h"

namespace kxf
{
	class KXF_API RunningSystemProcess: public ISystemProcess
	{
		public:
			static RunningSystemProcess GetCurrentProcess();
			static RunningSystemProcess OpenCurrentProcess(FlagSet<SystemProcessAccess> access = SystemProcessAccess::Everything, bool inheritHandle = false);

		private:
			void* m_Handle = nullptr;

		public:
			RunningSystemProcess() = default;
			RunningSystemProcess(uint32_t pid, FlagSet<SystemProcessAccess> access, bool inheritHandle = false)
			{
				Open(pid, access, inheritHandle);
			}
			RunningSystemProcess(const RunningSystemProcess&) = delete;
			RunningSystemProcess(RunningSystemProcess&& other) noexcept
			{
				*this = std::move(other);
			}
			~RunningSystemProcess()
			{
				Close();
			}

		public:
			// ISystemProcess
			bool IsNull() const override
			{
				return m_Handle == nullptr;
			}
			bool IsCurrent() const override;
			bool Is64Bit() const override;
			uint32_t GetID() const override;

			SystemProcessPriority GetPriority() const override;
			bool SetPriority(SystemProcessPriority priority) override;

			bool IsRunning() const override;
			std::optional<uint32_t> GetExitCode() const override;
			bool Terminate(uint32_t exitCode) override;

			bool Suspend() override;
			bool Resume() override;

			String GetCommandLine() const override;
			FSPath GetExecutablePath() const override;
			FSPath GetWorkingDirectory() const override;
			String GetExecutableParameters() const override;
			SHWindowCommand GetShowWindowCommand() const override;

			size_t EnumEnvironemntVariables(std::function<CallbackCommand(const String&, const String&)> func) const override;
			size_t EnumThreads(std::function<CallbackCommand(SystemThread)> func) const override;
			size_t EnumWindows(std::function<CallbackCommand(SystemWindow)> func) const override;

			// RunningSystemProcess
			bool Open(uint32_t pid, FlagSet<SystemProcessAccess> access, bool inheritHandle = false);
			void Close();

			void* GetHandle() const
			{
				return m_Handle;
			}
			bool AttachHandle(void* handle)
			{
				if (!m_Handle)
				{
					m_Handle = handle;
					return true;
				}
				return false;
			}
			void* DetachHandle() noexcept
			{
				void* handle = m_Handle;
				m_Handle = nullptr;
				return handle;
			}

			SystemThread GetMainThread() const;
			bool SafeTerminate(uint32_t exitCode);

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}

			RunningSystemProcess& operator=(const RunningSystemProcess&) = delete;
			RunningSystemProcess& operator=(RunningSystemProcess&& other) noexcept
			{
				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;

				return *this;
			}
	};
}
