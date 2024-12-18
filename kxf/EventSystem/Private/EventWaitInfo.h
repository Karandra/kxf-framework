#pragma once
#include "../Common.h"
#include "kxf/Utility/Common.h"
#include <atomic>
#include <condition_variable>

namespace kxf
{
	class IEvent;
}

namespace kxf::EventSystem::Private
{
	class KXF_API EventWaitInfo final
	{
		private:
			struct WaitInfo
			{
				std::mutex Mutex;
				std::condition_variable Condition;
				std::atomic<bool> Flag = false;
				std::unique_ptr<IEvent> Self;
			};
			std::unique_ptr<WaitInfo> m_WaitInfo;
			std::unique_ptr<IEvent> m_WaitResult;

		public:
			EventWaitInfo() = default;
			EventWaitInfo(const EventWaitInfo&) noexcept = default;
			EventWaitInfo(EventWaitInfo&&) noexcept = default;

		public:
			std::unique_ptr<IEvent> WaitProcessed()
			{
				if (!m_WaitInfo)
				{
					m_WaitInfo = std::make_unique<WaitInfo>();
					m_WaitInfo->Flag = false;

					std::unique_lock lock(m_WaitInfo->Mutex);
					m_WaitInfo->Condition.wait(lock, [&]()
					{
						return m_WaitInfo->Flag == true;
					});

					return std::move(m_WaitInfo->Self);
				}
				return nullptr;
			}
			void SignalProcessed(std::unique_ptr<IEvent> event) noexcept
			{
				m_WaitInfo->Self = std::move(event);
				m_WaitInfo->Flag = true;

				m_WaitInfo->Condition.notify_one();
			}

			void PutWaitResult(std::unique_ptr<IEvent> event) noexcept
			{
				m_WaitResult = std::move(event);
			}
			std::unique_ptr<IEvent> GetWaitResult() noexcept
			{
				return std::move(m_WaitResult);
			}

			bool HasWaitInfo() const noexcept
			{
				return m_WaitInfo != nullptr;
			}

		public:
			EventWaitInfo& operator=(const EventWaitInfo&) noexcept = default;
			EventWaitInfo& operator=(EventWaitInfo&& other) noexcept = default;
	};
}
