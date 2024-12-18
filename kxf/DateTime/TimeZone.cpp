#include "kxf-pch.h"
#include "TimeZone.h"
#include "kxf/wxWidgets/MapDateTime.h"
#include <wx/datetime.h>

namespace kxf
{
	void TimeZoneOffset::FromTimeZone(TimeZone tz) noexcept
	{
		m_Offset = wxWidgets::MapTimeZone(wxDateTime::TimeZone(static_cast<wxDateTime::TZ>(tz)));
	}

	bool TimeZoneOffset::IsLocal() const noexcept
	{
		return m_Offset.GetMilliseconds() == -1;
	}
	TimeSpan TimeZoneOffset::GetOffset() const noexcept
	{
		auto wxOffset = wxWidgets::MapTimeZone(m_Offset, IsLocal());
		return TimeSpan::Seconds(wxOffset.GetOffset());
	}
}
