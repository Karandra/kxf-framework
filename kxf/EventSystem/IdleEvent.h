#pragma once
#include "Common.h"
#include "BasicEvent.h"

namespace kxf
{
	enum class IdleEventMode
	{
		ProcessAll = wxIDLE_PROCESS_ALL,
		ProcessSpecific = wxIDLE_PROCESS_SPECIFIED
	};
}

namespace kxf
{
	class KXF_API IdleEvent: public BasicEvent
	{
		public:
			kxf_EVENT_MEMBER(IdleEvent, Idle);

		public:
			static IdleEventMode GetMode() noexcept
			{
				return static_cast<IdleEventMode>(wxIdleEvent::GetMode());
			}
			static void SetMode(IdleEventMode mode) noexcept
			{
				wxIdleEvent::SetMode(static_cast<wxIdleMode>(mode));
			}

		private:
			bool m_RequestedMore = false;

		public:
			IdleEvent() = default;

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<IdleEvent>(std::move(*this));
			}

			bool IsMoreRequested() const noexcept
			{
				return m_RequestedMore;
			}
			void RequestMore(bool needMore = true) noexcept
			{
				m_RequestedMore = needMore;
			}
	};
}
