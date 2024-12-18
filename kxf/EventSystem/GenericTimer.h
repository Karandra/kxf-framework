#pragma once
#include "Common.h"
#include "ITimer.h"
#include "TimerEvent.h"

namespace kxf::Private
{
	class GenericTimerImpl;
}

namespace kxf
{
	class KXF_API GenericTimer: public RTTI::DynamicImplementation<GenericTimer, ITimer>
	{
		friend class Private::GenericTimerImpl;

		private:
			std::unique_ptr<Private::GenericTimerImpl> m_Timer;
			IEvtHandler* m_EvtHandler = nullptr;

		protected:
			void OnNotify() override;

		public:
			GenericTimer();
			GenericTimer(const GenericTimer&) = delete;
			GenericTimer(GenericTimer&&);
			GenericTimer(IEvtHandler* evtHandler, int id = -1);
			~GenericTimer();

		public:
			int GetID() const noexcept override;
			bool IsRunning() const noexcept override;
			TimeSpan GetInterval() const noexcept override;

			IEvtHandler* GetEvtHandler() const noexcept override;
			void SetEvtHandler(IEvtHandler& evtHandler, int id = -1) noexcept override;

			void Start(TimeSpan interval, FlagSet<TimerFlag> flags = {}) noexcept override;
			void Stop() noexcept override;

		public:
			GenericTimer& operator=(const GenericTimer&) = delete;
			GenericTimer& operator=(GenericTimer&&);
	};
}
