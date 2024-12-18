#include "kxf-pch.h"
#include "TimeSpan.h"
#include "TimeClock.h"
#include <wx/datetime.h>

namespace kxf
{
	TimeSpan TimeSpan::Now() noexcept
	{
		return SteadyClock().Now();
	}
	TimeSpan TimeSpan::Now(const ITimeClock& clock) noexcept
	{
		return clock.Now();
	}

	TimeSpan::TimeSpan(const wxTimeSpan& other) noexcept
		:m_Value(other.GetValue().GetValue())
	{
	}

	String TimeSpan::Format(const String& format) const
	{
		if (format.IsEmpty())
		{
			return wxTimeSpan::Milliseconds(m_Value).Format(wxDefaultTimeSpanFormat);
		}
		else
		{
			return wxTimeSpan::Milliseconds(m_Value).Format(format);
		}
	}
	String TimeSpan::FormatTime(const Locale& locale, FlagSet<TimeFormatFlag> flags) const
	{
		if (flags & TimeFormatFlag::NoMinutes)
		{
			return Format(kxfS("%H"));
		}
		else if (flags & TimeFormatFlag::NoSeconds)
		{
			return Format(kxfS("%H:%M"));
		}
		else
		{
			return Format(kxfS("%H:%M:%S"));
		}
	}

	TimeSpan::operator wxTimeSpan() const noexcept
	{
		return wxTimeSpan(wxLongLong(m_Value));
	}
}
