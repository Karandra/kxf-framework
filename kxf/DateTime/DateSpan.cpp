#include "kxf-pch.h"
#include "DateSpan.h"
#include "DateTime.h"
#include <wx/datetime.h>

namespace kxf
{
	DateSpan::DateSpan(const wxDateSpan& dateSpan) noexcept
		:m_Years(dateSpan.GetYears()), m_Months(dateSpan.GetMonths()), m_Weeks(dateSpan.GetWeeks()), m_Days(dateSpan.GetDays())
	{
	}

	String DateSpan::Format(const String& format) const
	{
		return DateTime().SetDateSpan(*this).Format(format);
	}
	String DateSpan::FormatDate(const Locale& locale, DateFormatFlag flags) const
	{
		return DateTime().SetDateSpan(*this).FormatDate(locale, flags);
	}

	DateSpan::operator wxDateSpan() const noexcept
	{
		return wxDateSpan(m_Years, m_Months, m_Weeks, m_Days);
	}
}
