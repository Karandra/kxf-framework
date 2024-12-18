#pragma once
#include "Common.h"
#include "TimeSpan.h"
#include "kxf/EventSystem/IEvtHandler.h"

namespace kxf
{
	enum class TimerFlag: uint32_t
	{
		None = 0,

		Continuous = FlagSetValue<TimerFlag>(0)
	};
}

namespace kxf
{
	class KXF_API ITimer: public RTTI::Interface<ITimer>
	{
		kxf_RTTI_DeclareIID(ITimer, {0xbbe3593a, 0xd2ac, 0x4ba1, {0xa5, 0xa9, 0x90, 0x64, 0xa3, 0xa2, 0x3e, 0x79}});

		protected:
			virtual void OnNotify() = 0;

		public:
			~ITimer() = default;

		public:
			virtual int GetID() const = 0;
			virtual bool IsRunning() const = 0;
			virtual TimeSpan GetInterval() const = 0;

			virtual std::shared_ptr<IEvtHandler> GetEvtHandler() const = 0;
			virtual void SetEvtHandler(std::shared_ptr<IEvtHandler> evtHandler, int id = -1) = 0;

			virtual void Start(TimeSpan interval, FlagSet<TimerFlag> flags = {}) = 0;
			virtual void Stop() = 0;
	};
}
