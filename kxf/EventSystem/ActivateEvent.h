#pragma once
#include "Common.h"
#include "BasicEvent.h"

namespace kxf
{
	enum class ActivateEventReason
	{
		Unknown = -1,

		Mouse
	};
}

namespace kxf
{
	class KXF_API ActivateEvent: public BasicEvent
	{
		public:
			kxf_EVENT_MEMBER(ActivateEvent, Activate);

		private:
			ActivateEventReason m_Reason = ActivateEventReason::Unknown;
			bool m_IsActive = false;

		public:
			ActivateEvent() noexcept = default;
			ActivateEvent(bool isActive, ActivateEventReason reason) noexcept
				:m_IsActive(isActive), m_Reason(reason)
			{
			}

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ActivateEvent>(std::move(*this));
			}

			ActivateEventReason GetReason() const noexcept
			{
				return m_Reason;
			}
			bool IsActive() const noexcept
			{
				return m_IsActive;
			}
	};
}
