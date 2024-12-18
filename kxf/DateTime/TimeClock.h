#pragma once
#include "Common.h"
#include "TimeSpan.h"
#include "ITimeClock.h"

namespace kxf
{
	class KXF_API SystemClock final: public RTTI::DynamicImplementation<SystemClock, ITimeClock>
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KXF_API SteadyClock final: public RTTI::DynamicImplementation<SteadyClock, ITimeClock>
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KXF_API HighResolutionClock final: public RTTI::DynamicImplementation<HighResolutionClock, ITimeClock>
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KXF_API FileClock final: public RTTI::DynamicImplementation<FileClock, ITimeClock>
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KXF_API UTCClock final: public RTTI::DynamicImplementation<UTCClock, ITimeClock>
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KXF_API GPSClock final: public RTTI::DynamicImplementation<GPSClock, ITimeClock>
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KXF_API AtomicTimeClock final: public RTTI::DynamicImplementation<AtomicTimeClock, ITimeClock>
	{
		public:
			TimeSpan Now() const noexcept override;
	};
}
