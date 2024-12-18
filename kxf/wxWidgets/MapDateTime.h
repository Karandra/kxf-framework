#pragma once
#include "kxf/DateTime/Common.h"
#include "kxf/DateTime/TimeSpan.h"
#include "kxf/DateTime/TimeZone.h"
#include <wx/datetime.h>

namespace kxf::wxWidgets
{
	constexpr wxDateTime::Month MapMonth(Month month) noexcept
	{
		if (month == Month::None)
		{
			return wxDateTime::Inv_Month;
		}
		else
		{
			return static_cast<wxDateTime::Month>(month);
		}
	}
	constexpr Month MapMonth(wxDateTime::Month month) noexcept
	{
		if (month == wxDateTime::Inv_Month)
		{
			return Month::None;
		}
		else
		{
			return static_cast<Month>(month);
		}
	}

	constexpr wxDateTime::WeekDay MapWeekDay(WeekDay weekDay) noexcept
	{
		switch (weekDay)
		{
			case WeekDay::Monday:
			{
				return wxDateTime::Mon;
			}
			case WeekDay::Tuesday:
			{
				return wxDateTime::Tue;
			}
			case WeekDay::Wednesday:
			{
				return wxDateTime::Wed;
			}
			case WeekDay::Thursday:
			{
				return wxDateTime::Thu;
			}
			case WeekDay::Friday:
			{
				return wxDateTime::Fri;
			}
			case WeekDay::Sunday:
			{
				return wxDateTime::Sun;
			}
			case WeekDay::Saturday:
			{
				return wxDateTime::Sat;
			}
		};
		return wxDateTime::Inv_WeekDay;
	}
	constexpr WeekDay MapWeekDay(wxDateTime::WeekDay weekDay) noexcept
	{
		switch (weekDay)
		{
			case wxDateTime::Mon:
			{
				return WeekDay::Monday;
			}
			case wxDateTime::Tue:
			{
				return WeekDay::Tuesday;
			}
			case wxDateTime::Wed:
			{
				return WeekDay::Wednesday;
			}
			case wxDateTime::Thu:
			{
				return WeekDay::Thursday;
			}
			case wxDateTime::Fri:
			{
				return WeekDay::Friday;
			}
			case wxDateTime::Sun:
			{
				return WeekDay::Sunday;
			}
			case wxDateTime::Sat:
			{
				return WeekDay::Saturday;
			}
		};
		return WeekDay::None;
	}
	constexpr wxDateTime::WeekFlags MapWeekDayFlag(WeekDayOption option) noexcept
	{
		switch (option)
		{
			case WeekDayOption::MondayFirst:
			{
				return wxDateTime::Monday_First;
			}
			case WeekDayOption::SundayFirst:
			{
				return wxDateTime::Sunday_First;
			}
		};
		return wxDateTime::Default_First;
	}
	
	constexpr wxDateTime::Country MapCountry(Country country) noexcept
	{
		switch (country)
		{
			case Country::Default:
			{
				return wxDateTime::Country_Default;
			}
			case Country::France:
			{
				return wxDateTime::France;
			}
			case Country::Germany:
			{
				return wxDateTime::Germany;
			}
			case Country::Russia:
			{
				return wxDateTime::Russia;
			}
			case Country::USA:
			{
				return wxDateTime::USA;
			}
			case Country::UK:
			{
				return wxDateTime::UK;
			}
		};
		return wxDateTime::Country_Unknown;
	}
	constexpr Country MapCountry(wxDateTime::Country country) noexcept
	{
		switch (country)
		{
			case wxDateTime::Country_Default:
			{
				return Country::Default;
			}
			case wxDateTime::France:
			{
				return Country::France;
			}
			case wxDateTime::Germany:
			{
				return Country::Germany;
			}
			case wxDateTime::Russia:
			{
				return Country::Russia;
			}
			case wxDateTime::USA:
			{
				return Country::USA;
			}
			case wxDateTime::UK:
			{
				return Country::UK;
			}
		};
		return Country::None;
	}

	constexpr wxDateTime::Calendar MapCalendar(Calendar calendar) noexcept
	{
		switch (calendar)
		{
			case Calendar::Gregorian:
			{
				return wxDateTime::Gregorian;
			}
			case Calendar::Julian:
			{
				return wxDateTime::Julian;
			}
		};
		return wxDateTime::Gregorian;
	}
	constexpr wxDateTime::NameFlags MapWeekDayName(FlagSet<UnitNameFlag> flags) noexcept
	{
		if (flags & UnitNameFlag::Abbreviated)
		{
			return wxDateTime::Name_Abbr;
		}
		return wxDateTime::Name_Full;
	}

	inline TimeSpan MapTimeZone(const wxDateTime::TimeZone& tz) noexcept
	{
		if (tz.IsLocal())
		{
			return TimeSpan::Milliseconds(-1);
		}
		else
		{
			return TimeSpan::Seconds(tz.GetOffset());
		}
	}
	inline wxDateTime::TimeZone MapTimeZone(const TimeSpan& offset, bool isLocal) noexcept
	{
		if (isLocal)
		{
			return -1;
		}
		else
		{
			return offset.GetSeconds();
		}
	}
	inline wxDateTime::TimeZone MapTimeZone(const TimeZoneOffset& tz) noexcept
	{
		return MapTimeZone(tz.GetOffset(), tz.IsLocal());
	}
}
