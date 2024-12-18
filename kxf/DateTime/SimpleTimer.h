#pragma once
#include "Common.h"
#include "ITimer.h"
#include "TimerEvent.h"

namespace kxf::Private
{
	class SimpleTimerImpl;
}

namespace kxf
{
	class KXF_API SimpleTimer: public RTTI::DynamicImplementation<SimpleTimer, ITimer>
	{
		friend class Private::SimpleTimerImpl;

		private:
			std::unique_ptr<Private::SimpleTimerImpl> m_Timer;
			std::shared_ptr<IEvtHandler> m_EvtHandler;

		protected:
			void OnNotify() override;

		public:
			SimpleTimer();
			SimpleTimer(const SimpleTimer&) = delete;
			SimpleTimer(SimpleTimer&&);
			SimpleTimer(std::shared_ptr<IEvtHandler> evtHandler, int id = -1);
			~SimpleTimer();

		public:
			int GetID() const noexcept override;
			bool IsRunning() const noexcept override;
			TimeSpan GetInterval() const noexcept override;

			std::shared_ptr<IEvtHandler> GetEvtHandler() const noexcept override;
			void SetEvtHandler(std::shared_ptr<IEvtHandler> evtHandler, int id = -1) noexcept override;

			void Start(TimeSpan interval, FlagSet<TimerFlag> flags = {}) noexcept override;
			void Stop() noexcept override;

		public:
			SimpleTimer& operator=(const SimpleTimer&) = delete;
			SimpleTimer& operator=(SimpleTimer&&);
	};
}
