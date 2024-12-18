#pragma once
#include "ISystemThread.h"

namespace kxf
{
	class KXF_API RunningSystemThread: public ISystemThread
	{
		public:
			static RunningSystemThread GetCurrentThread();
			static RunningSystemThread OpenCurrentThread(FlagSet<SystemThreadAccess> access = SystemThreadAccess::Everything, bool inheritHandle = false);

		private:
			void* m_Handle = nullptr;

		public:
			RunningSystemThread() = default;
			RunningSystemThread(uint32_t tid, FlagSet<SystemThreadAccess> access, bool inheritHandle = false)
			{
				Open(tid, access, inheritHandle);
			}
			RunningSystemThread(const RunningSystemThread&) = delete;
			RunningSystemThread(RunningSystemThread&& other) noexcept
			{
				*this = std::move(other);
			}
			~RunningSystemThread()
			{
				Close();
			}

		public:
			// ISystemThread
			bool IsNull() const override
			{
				return m_Handle == nullptr;
			}
			bool IsCurrent() const override;
			bool Is64Bit() const override;
			uint32_t GetID() const override;
			SystemProcess GetOwningProcess() const override;

			SystemThreadPriority GetPriority() const override;
			bool SetPriority(SystemThreadPriority value) override;

			String GetDescription() const override;
			bool SetDescription(const String& description) override;

			bool IsRunning() const override;
			std::optional<uint32_t> GetExitCode() const override;
			bool Terminate(uint32_t exitCode) override;

			bool Suspend() override;
			bool Resume() override;

			size_t EnumWindows(std::function<CallbackCommand(SystemWindow)> func) const override;

			// RunningSystemThread
			bool Open(uint32_t tid, FlagSet<SystemThreadAccess> access, bool inheritHandle = false);
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

			bool PostMessage(uint32_t message, intptr_t wParam, intptr_t lParam);

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}

			RunningSystemThread& operator=(const RunningSystemThread&) = delete;
			RunningSystemThread& operator=(RunningSystemThread&& other) noexcept
			{
				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;

				return *this;
			}
	};
}
