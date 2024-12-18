#pragma once
#include "../Common.h"
#include "kxf/EventSystem/Event.h"

namespace kxf
{
	class KXF_API ProcessEvent: public BasicEvent
	{
		public:
			kxf_EVENT_MEMBER(ProcessEvent, InputIdle);
			kxf_EVENT_MEMBER(ProcessEvent, Termination);

		private:
			uint32_t m_PID = 0;
			uint32_t m_ExitCode = std::numeric_limits<uint32_t>::max();

		public:
			ProcessEvent() = default;
			ProcessEvent(uint32_t pid, uint32_t exitCode)
				:m_PID(pid), m_ExitCode(exitCode)
			{
			}

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ProcessEvent>(std::move(*this));
			}

			uint32_t GetPID() const
			{
				return m_PID;
			}
			void SetPID(uint32_t pid)
			{
				m_PID = static_cast<int>(pid);
			}

			uint32_t GetExitCode() const
			{
				return m_ExitCode;
			}
			void SetExitCode(uint32_t pid)
			{
				m_ExitCode = static_cast<int>(pid);
			}
	};
}
