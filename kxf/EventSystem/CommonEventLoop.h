#pragma once
#include "Common.h"
#include "IEventLoop.h"
#include "kxf/wxWidgets/EventLoopWrapper.h"

namespace kxf
{
	class KXF_API CommonEventLoop: public RTTI::Implementation<CommonEventLoop, IEventLoop>
	{
		private:
			mutable wxWidgets::EventLoopWrapperWx m_WxEventLoop;
			size_t m_YieldLevel = 0;
			FlagSet<EventCategory> m_AllowedToYield;

			int m_ExitCode = -1;
			bool m_IsInsideRun = false;
			bool m_ShouldExit = false;

		private:
			bool DispatchEvents();

		protected:
			int GetExitCode() const noexcept
			{
				return m_ExitCode;
			}
			bool IsInsideRun() const
			{
				return m_IsInsideRun;
			}
			bool ShouldExit() const
			{
				return m_ShouldExit;
			}

			void OnYieldFor(FlagSet<EventCategory> toProcess) override = 0;
			int OnRun() override;

		public:
			CommonEventLoop(FlagSet<EventCategory> allowedToYield = EventCategory::Everything)
				:m_WxEventLoop(*this, allowedToYield), m_AllowedToYield(allowedToYield)
			{
			}

		public:
			void UpdateWxLoop()
			{
				m_WxEventLoop.Update(m_AllowedToYield, m_ShouldExit, m_YieldLevel, m_IsInsideRun);
			}

			// IEventLoop
			wxEventLoopBase& GetWxLoop() override
			{
				return m_WxEventLoop;
			}

			int Run() override;
			void Exit(int exitCode = 0) override;
			void ScheduleExit(int exitCode = 0) override;

			bool DispatchIdle() override;

			bool IsYielding() const override
			{
				return m_YieldLevel != 0;
			}
			bool IsEventAllowedInsideYield(FlagSet<EventCategory> eventCategory) const override
			{
				return m_AllowedToYield.Contains(eventCategory);
			}

			bool Yield(FlagSet<EventYieldFlag> flags) override;
			bool YieldFor(FlagSet<EventCategory> toProcess) override;
	};
}
