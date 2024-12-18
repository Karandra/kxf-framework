#pragma once
#include "kxf/EventSystem/Event.h"

namespace kxf
{
	class KXF_API ThreadEvent: public BasicEvent
	{
		private:
			template<class T>
			constexpr static bool IsValidThreadExitCode() noexcept
			{
				return std::is_pointer_v<T> || std::is_integral_v<T> || std::is_enum_v<T>;
			}

		public:
			kxf_EVENT_MEMBER(ThreadEvent, Execute);
			kxf_EVENT_MEMBER(ThreadEvent, Started);
			kxf_EVENT_MEMBER(ThreadEvent, Finished);

		private:
			void* m_ExitCode = nullptr;

		public:
			ThreadEvent() = default;

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ThreadEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				// This is important to avoid that calling 'IEventLoop::YieldFor' thread events gets processed when this is unwanted.
				return EventCategory::Thread;
			}

		public:
			template<class T = void*> requires(IsValidThreadExitCode<T>())
			T GetExitCode() const
			{
				return static_cast<T>(m_ExitCode);
			}
			
			template<class T> requires(IsValidThreadExitCode<T>())
			void SetExitCode(T code)
			{
				m_ExitCode = static_cast<void*>(code);
			}
	};
}
