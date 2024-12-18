#include "kxf-pch.h"
#include "DateTime.h"
#include "TimeSpan.h"
#include "kxf/Localization/Locale.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/wxWidgets/MapDateTime.h"

#include <ctime>
#include <Windows.h>
#include <wx/datetime.h>
#include "kxf/Win32/UndefMacros.h"

namespace
{
	constexpr int64_t g_InvalidValue = std::numeric_limits<int64_t>::min();

	wxDateTime AsWxDateTime(int64_t value) noexcept
	{
		return wxDateTime(wxLongLong(value));
	}

	template<class TFunc>
	decltype(auto) ModifyAsWxDateTime(int64_t& value, TFunc&& func) noexcept(std::is_nothrow_invocable_v<TFunc, wxDateTime&>)
	{
		wxDateTime dt = AsWxDateTime(value);
		kxf::Utility::ScopeGuard atExit = [&]()
		{
			value = dt.GetValue().GetValue();
		};

		return std::invoke(func, dt);
	}
}

namespace kxf
{
	const int DateTime::InvalidDay = -1;
	const int DateTime::InvalidMonth = wxDateTime::Inv_Month;
	const int DateTime::InvalidYear = wxDateTime::Inv_Year;
	const int DateTime::InvalidWeekDay = wxDateTime::Inv_WeekDay;

	DateTime DateTime::Now() noexcept
	{
		return wxDateTime::UNow();
	}
	DateTime DateTime::Today() noexcept
	{
		return wxDateTime::UNow().ResetTime();
	}

	int DateTime::GetCentury(int year) noexcept
	{
		return wxDateTime::GetCentury(year);
	}
	int DateTime::ConvertYearToBC(int year) noexcept
	{
		return wxDateTime::ConvertYearToBC(year);
	}

	Country DateTime::GetDefaultCountry() noexcept
	{
		return wxWidgets::MapCountry(wxDateTime::GetCountry());
	}
	void DateTime::SetDefaultCountry(Country country) noexcept
	{
		return wxDateTime::SetCountry(wxWidgets::MapCountry(country));
	}

	bool DateTime::IsDSTApplicable(int year, Country country)
	{
		return wxDateTime::IsDSTApplicable(year, wxWidgets::MapCountry(country));
	}
	DateTime DateTime::GetBeginDST(int year, Country country)
	{
		return wxDateTime::GetBeginDST(year, wxWidgets::MapCountry(country));
	}
	DateTime DateTime::GetEndDST(int year, Country country) noexcept
	{
		return wxDateTime::GetEndDST(year, wxWidgets::MapCountry(country));
	}

	bool DateTime::IsLeapYear(int year, Calendar calendar) noexcept
	{
		return wxDateTime::IsLeapYear(year, wxWidgets::MapCalendar(calendar));
	}
	int DateTime::GetNumberOfDays(int year, Calendar calendar) noexcept
	{
		return wxDateTime::GetNumberOfDays(year, wxWidgets::MapCalendar(calendar));
	}
	int DateTime::GetNumberOfDays(Month month, int year, Calendar calendar) noexcept
	{
		return wxDateTime::GetNumberOfDays(wxWidgets::MapMonth(month), year, wxWidgets::MapCalendar(calendar));
	}

	String DateTime::GetMonthName(Month month, const Locale& locale, FlagSet<UnitNameFlag> flags)
	{
		return locale.GetMonthName(month, flags).value_or(NullString);
	}
	String DateTime::GetWeekDayName(WeekDay weekDay, const Locale& locale, FlagSet<UnitNameFlag> flags)
	{
		return locale.GetWeekDayName(weekDay, flags).value_or(NullString);
	}

	DateTime::DateTime(const wxDateTime& other) noexcept
		:m_Value(other.GetValue().GetValue())
	{
	}

	// Accessors and modifiers
	int DateTime::GetMillisecond(const TimeZoneOffset& tz) const noexcept
	{
		return AsWxDateTime(m_Value).GetMillisecond(wxWidgets::MapTimeZone(tz));
	}
	DateTime& DateTime::SetMillisecond(int value) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetMillisecond(value);
		});
		return *this;
	}

	int DateTime::GetSecond(const TimeZoneOffset& tz) const noexcept
	{
		return AsWxDateTime(m_Value).GetSecond(wxWidgets::MapTimeZone(tz));
	}
	DateTime& DateTime::SetSecond(int value) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetSecond(value);
		});
		return *this;
	}

	int DateTime::GetMinute(const TimeZoneOffset& tz) const noexcept
	{
		return AsWxDateTime(m_Value).GetMinute(wxWidgets::MapTimeZone(tz));
	}
	DateTime& DateTime::SetMinute(int value) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetMinute(value);
		});
		return *this;
	}

	int DateTime::GetHour(const TimeZoneOffset& tz) const noexcept
	{
		return AsWxDateTime(m_Value).GetHour(wxWidgets::MapTimeZone(tz));
	}
	DateTime& DateTime::SetHour(int value) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetHour(value);
		});
		return *this;
	}

	int DateTime::GetDay(const TimeZoneOffset& tz) const noexcept
	{
		return AsWxDateTime(m_Value).GetDay(wxWidgets::MapTimeZone(tz));
	}
	DateTime& DateTime::SetDay(int value) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetDay(value);
		});
		return *this;
	}

	Month DateTime::GetMonth(const TimeZoneOffset& tz) const noexcept
	{
		return wxWidgets::MapMonth(AsWxDateTime(m_Value).GetMonth(wxWidgets::MapTimeZone(tz)));
	}
	DateTime& DateTime::SetMonth(Month value) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetMonth(wxWidgets::MapMonth(value));
		});
		return *this;
	}

	int DateTime::GetYear(const TimeZoneOffset& tz) const noexcept
	{
		return AsWxDateTime(m_Value).GetYear(wxWidgets::MapTimeZone(tz));
	}
	DateTime& DateTime::SetYear(int value) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetYear(value);
		});
		return *this;
	}

	int DateTime::GetCentury(const TimeZoneOffset& tz) const noexcept
	{
		return AsWxDateTime(m_Value).GetCentury(wxWidgets::MapTimeZone(tz));
	}
	int DateTime::GetDayOfYear(const TimeZoneOffset& tz) const noexcept
	{
		return AsWxDateTime(m_Value).GetDayOfYear(wxWidgets::MapTimeZone(tz));
	}
	WeekDay DateTime::GetWeekDay(const TimeZoneOffset& tz) const noexcept
	{
		return wxWidgets::MapWeekDay(AsWxDateTime(m_Value).GetWeekDay(wxWidgets::MapTimeZone(tz)));
	}

	DateTime DateTime::GetDateOnly() const noexcept
	{
		return AsWxDateTime(m_Value).GetDateOnly();
	}
	DateTime& DateTime::ResetTime() noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.ResetTime();
		});
		return *this;
	}

	std::tm DateTime::GetStdTm(const TimeZoneOffset& tz) const noexcept
	{
		std::tm tm;
		tm.tm_year = GetYear(tz);
		tm.tm_mon = static_cast<int>(GetMonth(tz));
		tm.tm_hour = GetHour(tz);
		tm.tm_min = GetMinute(tz);
		tm.tm_sec = GetSecond(tz);

		tm.tm_mday = GetDay(tz);
		tm.tm_wday = static_cast<int>(wxWidgets::MapWeekDay(GetWeekDay(tz)));
		tm.tm_yday = GetDayOfYear(tz);
		tm.tm_isdst = AsWxDateTime(m_Value).IsDST();

		return tm;
	}
	DateTime& DateTime::SetStdTm(const std::tm& value, const TimeZoneOffset& tz) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.Set(value);
			if (!tz.IsLocal())
			{
				dt = dt.FromTimezone(wxWidgets::MapTimeZone(tz));
			}
		});
		return *this;
	}

	TimeSpan DateTime::GetTimeSpan(const TimeZoneOffset& tz) const noexcept
	{
		return TimeSpan::Seconds(GetUnixTime());
	}
	DateTime& DateTime::SetTimeSpan(const TimeSpan& span) noexcept
	{
		SetUnixTime(span.GetSeconds());
		return *this;
	}

	DateSpan DateTime::GetDateSpan(const TimeZoneOffset& tz) const noexcept
	{
		if (IsValid())
		{
			int years = GetYear(tz);
			if (years == wxDateTime::Inv_Year)
			{
				years = 0;
			}

			int months = AsWxDateTime(m_Value).GetMonth(wxWidgets::MapTimeZone(tz));
			if (months == wxDateTime::Inv_Month)
			{
				months = 0;
			}

			return DateSpan(years, months, 0, AsWxDateTime(m_Value).GetDay(wxWidgets::MapTimeZone(tz)));
		}
		return {};
	}
	DateTime& DateTime::SetDateSpan(const DateSpan& span) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.ResetTime();
			dt.SetYear(span.GetYears());
			dt.SetMonth(static_cast<wxDateTime::Month>(span.GetMonths()));
			dt.SetDay(span.GetDays());
		});
		return *this;
	}

	time_t DateTime::GetUnixTime() const noexcept
	{
		return AsWxDateTime(m_Value).GetTicks();
	}
	kxf::DateTime& DateTime::SetUnixTime(time_t value) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.Set(value);
		});
		return *this;
	}

	_SYSTEMTIME DateTime::GetSystemTime(const TimeZoneOffset& tz) const noexcept
	{
		if (IsValid())
		{
			auto Get = [&tz](const DateTime& dateTime)
			{
				SYSTEMTIME systemTime = {};
				AsWxDateTime(dateTime.m_Value).GetAsMSWSysTime(&systemTime);

				// Set the day of week because 'wxDateTime::GetAsMSWSysTime' doesn't set it for some reason
				systemTime.wDayOfWeek = static_cast<int>(wxWidgets::MapWeekDay(dateTime.GetWeekDay(tz)));
				return systemTime;
			};

			if (tz.IsLocal())
			{
				return Get(*this);
			}
			else
			{
				return Get(ToTimeZone(tz));
			}
		}
		return {};
	}
	DateTime& DateTime::SetSystemTime(const _SYSTEMTIME& other, const TimeZoneOffset& tz) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			dt.SetFromMSWSysTime(other);
			if (!tz.IsLocal())
			{
				dt = dt.FromTimezone(wxWidgets::MapTimeZone(tz));
			}
		});
		return *this;
	}

	_FILETIME DateTime::GetFileTime(const TimeZoneOffset& tz) const noexcept
	{
		SYSTEMTIME systemTime = GetSystemTime(tz);
		if (FILETIME fileTIme = {}; ::SystemTimeToFileTime(&systemTime, &fileTIme))
		{
			return fileTIme;
		}
		return {};
	}
	DateTime& DateTime::SetFileTime(const _FILETIME& other, const TimeZoneOffset& tz) noexcept
	{
		SYSTEMTIME systemTime = {};
		if (::FileTimeToSystemTime(&other, &systemTime))
		{
			SetSystemTime(systemTime, tz);
		}
		else
		{
			m_Value = g_InvalidValue;
		}
		return *this;
	}

	// Comparison
	bool DateTime::IsSameDate(const DateTime& other) const noexcept
	{
		return AsWxDateTime(m_Value).IsSameDate(other);
	}
	bool DateTime::IsSameTime(const DateTime& other) const noexcept
	{
		return AsWxDateTime(m_Value).IsSameTime(other);
	}
	bool DateTime::IsEqualUpTo(const DateTime& other, const TimeSpan& span) const noexcept
	{
		return AsWxDateTime(m_Value).IsEqualUpTo(other, span);
	}

	bool DateTime::IsEarlierThan(const DateTime& other) const noexcept
	{
		return AsWxDateTime(m_Value).IsEarlierThan(other);
	}
	bool DateTime::IsLaterThan(const DateTime& other) const noexcept
	{
		return AsWxDateTime(m_Value).IsLaterThan(other);
	}

	bool DateTime::IsBetween(const DateTime& left, const DateTime& right) const noexcept
	{
		return AsWxDateTime(m_Value).IsStrictlyBetween(left, right);
	}
	bool DateTime::IsBetweenInclusive(const DateTime& left, const DateTime& right) const noexcept
	{
		return AsWxDateTime(m_Value).IsBetween(left, right);
	}

	// Arithmetics
	DateSpan DateTime::GetDifference(const DateTime& other) const noexcept
	{
		return AsWxDateTime(m_Value).DiffAsDateSpan(other);
	}

	DateTime DateTime::operator+(const TimeSpan& other) const noexcept
	{
		return AsWxDateTime(m_Value) + other;
	}
	DateTime DateTime::operator-(const TimeSpan& other) const noexcept
	{
		return AsWxDateTime(m_Value) - other;
	}
	DateTime DateTime::operator-(const DateSpan& other) const noexcept
	{
		return AsWxDateTime(m_Value) - other;
	}
	TimeSpan DateTime::operator-(const DateTime& other) const noexcept
	{
		return AsWxDateTime(m_Value) - other;
	}
	DateTime DateTime::operator+(const DateSpan& other) const noexcept
	{
		return AsWxDateTime(m_Value) + other;
	}

	DateTime& DateTime::operator+=(const TimeSpan& other) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt += other;
		});
		return *this;
	}
	DateTime& DateTime::operator-=(const TimeSpan& other) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt -= other;
		});
		return *this;
	}
	DateTime& DateTime::operator+=(const DateSpan& other) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt += other;
		});
		return *this;
	}
	DateTime& DateTime::operator-=(const DateSpan& other) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt -= other;
		});
		return *this;
	}

	// Formatting and parsing
	String DateTime::Format(const String& format, const TimeZoneOffset& tz) const
	{
		if (format.IsEmpty())
		{
			return AsWxDateTime(m_Value).Format(wxDefaultDateTimeFormat, wxWidgets::MapTimeZone(tz));
		}
		else
		{
			return AsWxDateTime(m_Value).Format(format, wxWidgets::MapTimeZone(tz));
		}
	}
	String DateTime::FormatDate(const Locale& locale, FlagSet<DateFormatFlag> flags, const TimeZoneOffset& tz) const
	{
		if (IsValid())
		{
			FlagSet<DWORD> nativeFlags = DATE_AUTOLAYOUT;
			nativeFlags.Add(DATE_LONGDATE, flags & DateFormatFlag::Long);
			nativeFlags.Add(DATE_YEARMONTH, flags & DateFormatFlag::YearMonth);
			nativeFlags.Add(DATE_MONTHDAY, flags & DateFormatFlag::MonthDay);

			wchar_t formatted[1024] = {};
			SYSTEMTIME localTime = GetSystemTime(tz);
			if (::GetDateFormatEx(locale.m_LocaleName, *nativeFlags, &localTime, nullptr, formatted, std::size(formatted), nullptr) != 0)
			{
				return formatted;
			}
		}
		return {};
	}
	String DateTime::FormatTime(const Locale& locale, FlagSet<TimeFormatFlag> flags, const TimeZoneOffset& tz) const
	{
		if (IsValid())
		{
			FlagSet<DWORD> nativeFlags;
			nativeFlags.Add(TIME_NOSECONDS, flags & TimeFormatFlag::NoSeconds);
			nativeFlags.Add(TIME_NOTIMEMARKER, flags & TimeFormatFlag::NoTimeMarker);
			nativeFlags.Add(TIME_NOMINUTESORSECONDS, flags & TimeFormatFlag::NoMinutes);
			nativeFlags.Add(TIME_FORCE24HOURFORMAT, flags & TimeFormatFlag::Force24Hour);

			wchar_t formatted[1024] = {};
			SYSTEMTIME localTime = GetSystemTime(tz);
			if (::GetTimeFormatEx(locale.m_LocaleName, *nativeFlags, &localTime, nullptr, formatted, std::size(formatted)) != 0)
			{
				return formatted;
			}
		}
		return {};
	}

	String DateTime::FormatISODate() const
	{
		return AsWxDateTime(m_Value).FormatISODate();
	}
	String DateTime::FormatISOTime() const
	{
		return AsWxDateTime(m_Value).FormatISOTime();
	}
	String DateTime::FormatISOCombined(const UniChar& sep) const
	{
		return AsWxDateTime(m_Value).FormatISOCombined(sep.ToASCII().value_or('T'));
	}

	bool DateTime::ParseDate(const String& date)
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			wxString::const_iterator it;
			return dt.ParseDate(date, &it);
		});
	}
	bool DateTime::ParseTime(const String& time)
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			wxString::const_iterator it;
			return dt.ParseTime(time, &it);
		});
	}
	bool DateTime::ParseCombined(const String& dateTime)
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			wxString::const_iterator it;
			return dt.ParseDateTime(dateTime, &it);
		});
	}

	bool DateTime::ParseFormat(const String& dateTime)
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			wxString::const_iterator it;
			return dt.ParseFormat(dateTime, &it);
		});
	}
	bool DateTime::ParseFormat(const String& dateTime, const String& format)
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			wxString::const_iterator it;
			return dt.ParseFormat(dateTime, format, &it);
		});
	}
	bool DateTime::ParseFormat(const String& dateTime, const String& format, const DateTime& dateDefault)
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			wxString::const_iterator it;
			return dt.ParseFormat(dateTime, format, dateDefault, &it);
		});
	}

	bool DateTime::ParseISODate(const String& date)
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			return dt.ParseISODate(date);
		});
	}
	bool DateTime::ParseISOTime(const String& time)
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			return dt.ParseISOTime(time);
		});
	}
	bool DateTime::ParseISOCombined(const String& dateTime, const UniChar& sep)
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			return dt.ParseISOCombined(dateTime, sep.ToASCII().value_or('T'));
		});
	}
	bool DateTime::ParseRFC822Date(const String& dateTime)
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt)
		{
			wxString::const_iterator end;
			return dt.ParseRfc822Date(dateTime, &end);
		});
	}

	// Calendar calculations
	bool DateTime::IsWorkDay(Country country) const noexcept
	{
		return AsWxDateTime(m_Value).IsWorkDay(wxWidgets::MapCountry(country));
	}
	bool DateTime::SetToWeekDay(WeekDay weekday, int n, Month month, int year) noexcept
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			return dt.SetToWeekDay(wxWidgets::MapWeekDay(weekday), n, wxWidgets::MapMonth(month), year);
		});
	}
	bool DateTime::SetToLastWeekDay(WeekDay weekday, Month month, int year) noexcept
	{
		return ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			return dt.SetToLastWeekDay(wxWidgets::MapWeekDay(weekday), wxWidgets::MapMonth(month), year);
		});
	}
	DateTime& DateTime::SetToWeekDayInSameWeek(WeekDay weekday, WeekDayOption options) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetToWeekDayInSameWeek(wxWidgets::MapWeekDay(weekday), wxWidgets::MapWeekDayFlag(options));
		});
		return *this;
	}
	DateTime& DateTime::SetToNextWeekDay(WeekDay weekday) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetToNextWeekDay(wxWidgets::MapWeekDay(weekday));
		});
		
		return *this;
	}
	DateTime& DateTime::SetToPrevWeekDay(WeekDay weekday) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetToPrevWeekDay(wxWidgets::MapWeekDay(weekday));
		});
		return *this;
	}
	kxf::DateTime& DateTime::SetToLastMonthDay(Month month, int year) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetToLastMonthDay(wxWidgets::MapMonth(month), year);
		});
		return *this;
	}
	DateTime& DateTime::SetToYearDay(int day) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.SetToYearDay(day);
		});
		return *this;
	}

	// Astronomical/Historical functions
	double DateTime::GetRataDie() const noexcept
	{
		return AsWxDateTime(m_Value).GetRataDie();
	}

	double DateTime::GetJulianDayNumber() const noexcept
	{
		return AsWxDateTime(m_Value).GetJulianDayNumber();
	}
	double DateTime::GetModifiedJulianDayNumber() const noexcept
	{
		return AsWxDateTime(m_Value).GetModifiedJulianDayNumber();
	}
	DateTime& DateTime::SetJulianDayNumber(double jdn) noexcept
	{
		ModifyAsWxDateTime(m_Value, [&](wxDateTime& dt) noexcept
		{
			dt.Set(jdn);
		});
		return *this;
	}

	// Time Zone and DST support
	DateTime DateTime::ToTimeZone(const TimeZoneOffset& tz, bool noDST) const noexcept
	{
		return AsWxDateTime(m_Value).ToTimezone(wxWidgets::MapTimeZone(tz), noDST);
	}
	DateTime DateTime::FromTimeZone(const TimeZoneOffset& tz, bool noDST) const noexcept
	{
		return AsWxDateTime(m_Value).FromTimezone(wxWidgets::MapTimeZone(tz), noDST);
	}
	DateTime DateTime::ToUTC(bool noDST) const noexcept
	{
		return AsWxDateTime(m_Value).ToUTC(noDST);
	}
	bool DateTime::IsDST(Country country) const noexcept
	{
		return AsWxDateTime(m_Value).IsDST(wxWidgets::MapCountry(country));
	}

	DateTime::operator wxDateTime() const noexcept
	{
		return AsWxDateTime(m_Value);
	}
}
