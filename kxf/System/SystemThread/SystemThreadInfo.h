#pragma once
#include "ISystemThread.h"
#include "RunningSystemThread.h"
#include "../SystemProcess/SystemProcessInfo.h"

namespace kxf
{
	class KXF_API SystemThread: public ISystemThread
	{
		public:
			static SystemThread GetCurrentThread();

		protected:
			uint32_t m_TID = std::numeric_limits<uint32_t>::max();

		public:
			SystemThread() = default;
			SystemThread(uint32_t tid)
				:m_TID(tid)
			{
			}

		public:
			// ISystemThread
			bool IsNull() const override
			{
				return m_TID == std::numeric_limits<uint32_t>::max();
			}
			bool IsCurrent() const override;
			bool Is64Bit() const override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::QueryLimitedInformation).Is64Bit();
			}
			uint32_t GetID() const override
			{
				return m_TID;
			}
			SystemProcess GetOwningProcess() const override;

			SystemThreadPriority GetPriority() const override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::QueryLimitedInformation).GetPriority();
			}
			bool SetPriority(SystemThreadPriority priority) override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::SetLimitedInformation).SetPriority(priority);
			}

			String GetDescription() const override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::QueryLimitedInformation).GetDescription();
			}
			bool SetDescription(const String& description) override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::SetLimitedInformation).SetDescription(description);
			}

			bool IsRunning() const override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::QueryLimitedInformation).IsRunning();
			}
			std::optional<uint32_t> GetExitCode() const override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::QueryLimitedInformation).GetExitCode();
			}
			bool Terminate(uint32_t exitCode) override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::Terminate).Terminate(exitCode);
			}

			bool Suspend() override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::SuspendResume).Suspend();
			}
			bool Resume() override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::SuspendResume).Resume();
			}

			size_t EnumWindows(std::function<CallbackCommand(SystemWindow)> func) const override
			{
				return RunningSystemThread(m_TID, SystemThreadAccess::None).EnumWindows(std::move(func));
			}

			// SystemThread
			RunningSystemThread Open(FlagSet<SystemThreadAccess> access, bool inheritHandle = false) const
			{
				return RunningSystemThread(m_TID, access, inheritHandle);
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

			auto operator<=>(const SystemThread& other) const noexcept
			{
				return m_TID <=> other.m_TID;
			}
			bool operator==(const SystemThread& other) const noexcept
			{
				return m_TID == other.m_TID;
			}
	};
}

namespace kxf
{
	class KXF_API SystemThreadInfo: public ISystemThread
	{
		public:
			static SystemThreadInfo GetCurrentThread();

		private:
			SystemProcess m_OwningProcess;
			uint32_t m_TID = std::numeric_limits<uint32_t>::max();
			bool m_Is64Bit = false;
			SystemThreadPriority m_Priority;
			String m_Description;

		public:
			SystemThreadInfo() = default;
			SystemThreadInfo(uint32_t tid, bool is64Bit)
				:m_TID(tid), m_Is64Bit(is64Bit)
			{
			}
			SystemThreadInfo(const ISystemThread& info);

		public:
			// ISystemThread
			bool IsNull() const override
			{
				return m_TID == std::numeric_limits<uint32_t>::max();
			}
			bool IsCurrent() const override;
			bool Is64Bit() const override
			{
				return m_Is64Bit;
			}
			uint32_t GetID() const override
			{
				return m_TID;
			}
			SystemProcess GetOwningProcess() const override
			{
				return m_OwningProcess;
			}

			SystemThreadPriority GetPriority() const override
			{
				return m_Priority;
			}
			bool SetPriority(SystemThreadPriority priority) override
			{
				m_Priority = priority;
				return true;
			}

			String GetDescription() const override
			{
				return m_Description;
			}
			bool SetDescription(const String& description) override
			{
				m_Description = description;
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

			size_t EnumWindows(std::function<CallbackCommand(SystemWindow)> func) const override
			{
				return 0;
			}
	};
}

namespace std
{
	template<>
	struct hash<kxf::SystemThread> final
	{
		size_t operator()(const kxf::SystemThread& thread) const noexcept
		{
			std::hash<uint32_t> calc;
			return calc(thread.GetID());
		}
	};
}
