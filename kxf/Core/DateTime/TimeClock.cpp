#include "kxf-pch.h"
#include "TimeClock.h"
#include <chrono>

namespace
{
	template<class TClock>
	kxf::TimeSpan ChronoClockTimeNow() noexcept
	{
		using namespace std::chrono;
		using namespace kxf;

		return TimeSpan::Milliseconds(duration_cast<milliseconds>(TClock::now().time_since_epoch()).count());
	}
}

namespace kxf
{
	TimeSpan SystemClock::Now() const noexcept
	{
		return ChronoClockTimeNow<std::chrono::system_clock>();
	}

	TimeSpan SteadyClock::Now() const noexcept
	{
		return ChronoClockTimeNow<std::chrono::steady_clock>();
	}

	TimeSpan HighResolutionClock::Now() const noexcept
	{
		return ChronoClockTimeNow<std::chrono::high_resolution_clock>();
	}

	TimeSpan FileClock::Now() const noexcept
	{
		return ChronoClockTimeNow<std::chrono::file_clock>();
	}

	TimeSpan UTCClock::Now() const noexcept
	{
		return ChronoClockTimeNow<std::chrono::utc_clock>();
	}

	TimeSpan GPSClock::Now() const noexcept
	{
		return ChronoClockTimeNow<std::chrono::gps_clock>();
	}

	TimeSpan AtomicTimeClock::Now() const noexcept
	{
		return ChronoClockTimeNow<std::chrono::tai_clock>();
	}
}
