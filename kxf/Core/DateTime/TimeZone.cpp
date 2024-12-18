#include "kxf-pch.h"
#include "TimeZone.h"
#include "Private/Mapping.h"
#include <wx/datetime.h>

namespace kxf
{
	void TimeZoneOffset::FromTimeZone(TimeZone tz) noexcept
	{
		m_Offset = Private::MapTimeZone(wxDateTime::TimeZone(static_cast<wxDateTime::TZ>(tz)));
	}

	bool TimeZoneOffset::IsLocal() const noexcept
	{
		return m_Offset.GetMilliseconds() == -1;
	}
	TimeSpan TimeZoneOffset::GetOffset() const noexcept
	{
		auto wxOffset = Private::MapTimeZone(m_Offset, IsLocal());
		return TimeSpan::Seconds(wxOffset.GetOffset());
	}
}
