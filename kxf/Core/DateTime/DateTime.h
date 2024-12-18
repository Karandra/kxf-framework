#pragma once
#include "Common.h"
#include "TimeSpan.h"
#include "DateSpan.h"
#include "TimeZone.h"
#include "kxf/Serialization/BinarySerializer.h"

class wxDateTime;
struct _FILETIME;
struct _SYSTEMTIME;
namespace std
{
	struct tm;
}
namespace kxf
{
	class Locale;
}

namespace kxf
{
	class KXF_API DateTime final
	{
		public:
			static const int InvalidDay;
			static const int InvalidMonth;
			static const int InvalidYear;
			static const int InvalidWeekDay;

		public:
			static DateTime Now() noexcept;
			static DateTime Today() noexcept;

			static int GetCentury(int year) noexcept;
			static int ConvertYearToBC(int year) noexcept;

			static Country GetDefaultCountry() noexcept;
			static void SetDefaultCountry(Country country) noexcept;

			static bool IsDSTApplicable(int year = InvalidYear, Country country = Country::Default);
			static DateTime GetBeginDST(int year = InvalidYear, Country country = Country::Default);
			static DateTime GetEndDST(int year = InvalidYear, Country country = Country::Default) noexcept;

			static bool IsLeapYear(int year, Calendar calendar = Calendar::Gregorian) noexcept;
			static int GetNumberOfDays(int year, Calendar calendar = Calendar::Gregorian) noexcept;
			static int GetNumberOfDays(Month month, int year = InvalidYear, Calendar calendar = Calendar::Gregorian) noexcept;

			static String GetMonthName(Month month, const Locale& locale, FlagSet<UnitNameFlag> flags = UnitNameFlag::None);
			static String GetWeekDayName(WeekDay weekDay, const Locale& locale, FlagSet<UnitNameFlag> flags = UnitNameFlag::None);

		private:
			int64_t m_Value = std::numeric_limits<int64_t>::min();

		public:
			DateTime() noexcept = default;
			DateTime(const DateTime&) noexcept = default;
			DateTime(const wxDateTime& other) noexcept;

		public:
			bool IsValid() const noexcept
			{
				return m_Value != std::numeric_limits<int64_t>::min();
			}
			bool IsNegative() const noexcept
			{
				return m_Value < 0;
			}
			bool IsPositive() const noexcept
			{
				return m_Value > 0;
			}

			// Accessors and modifiers
			int64_t GetValue() const noexcept
			{
				return m_Value;
			}
			DateTime& SetValue(int64_t value) noexcept
			{
				m_Value = value;
				return *this;
			}

			int GetMillisecond(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetMillisecond(int value) noexcept;

			int GetSecond(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetSecond(int value) noexcept;

			int GetMinute(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetMinute(int value) noexcept;

			int GetHour(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetHour(int value) noexcept;

			int GetDay(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetDay(int value) noexcept;

			Month GetMonth(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetMonth(Month value) noexcept;

			int GetYear(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetYear(int value) noexcept;

			int GetCentury(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			int GetDayOfYear(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			WeekDay GetWeekDay(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			
			DateTime GetDateOnly() const noexcept;
			DateTime& ResetTime() noexcept;

			std::tm GetStdTm(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetStdTm(const std::tm& value, const TimeZoneOffset& tz = TimeZone::Local) noexcept;

			TimeSpan GetTimeSpan(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetTimeSpan(const TimeSpan& span) noexcept;

			DateSpan GetDateSpan(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetDateSpan(const DateSpan& span) noexcept;

			time_t GetUnixTime() const noexcept;
			DateTime& SetUnixTime(time_t value) noexcept;

			_SYSTEMTIME GetSystemTime(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetSystemTime(const _SYSTEMTIME& other, const TimeZoneOffset& tz = TimeZone::Local) noexcept;
			
			_FILETIME GetFileTime(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetFileTime(const _FILETIME& other, const TimeZoneOffset& tz = TimeZone::Local) noexcept;

			// Comparison
			bool IsSameDate(const DateTime& other) const noexcept;
			bool IsSameTime(const DateTime& other) const noexcept;
			bool IsEqualUpTo(const DateTime& other, const TimeSpan& span) const noexcept;

			bool IsEarlierThan(const DateTime& other) const noexcept;
			bool IsLaterThan(const DateTime& other) const noexcept;

			bool IsBetween(const DateTime& left, const DateTime& right) const noexcept;
			bool IsBetweenInclusive(const DateTime& left, const DateTime& right) const noexcept;

			auto operator<=>(const DateTime& other) const noexcept
			{
				return GetValue() <=> other.GetValue();
			}
			bool operator==(const DateTime& other) const noexcept
			{
				return GetValue() == other.GetValue();
			}

			// Arithmetics
			DateSpan GetDifference(const DateTime& other) const noexcept;

			DateTime operator+(const TimeSpan& other) const noexcept;
			DateTime operator-(const TimeSpan& other) const noexcept;
			DateTime operator+(const DateSpan& other) const noexcept;
			DateTime operator-(const DateSpan& other) const noexcept;
			TimeSpan operator-(const DateTime& other) const noexcept;

			DateTime& operator+=(const TimeSpan& other) noexcept;
			DateTime& operator-=(const TimeSpan& other) noexcept;
			DateTime& operator+=(const DateSpan& other) noexcept;
			DateTime& operator-=(const DateSpan& other) noexcept;

			// Formatting and parsing
			String Format(const String& format = {}, const TimeZoneOffset& tz = TimeZone::Local) const;
			String FormatDate(const Locale& locale, FlagSet<DateFormatFlag> flags = DateFormatFlag::None, const TimeZoneOffset& tz = TimeZone::Local) const;
			String FormatTime(const Locale& locale, FlagSet<TimeFormatFlag> flags = TimeFormatFlag::None, const TimeZoneOffset& tz = TimeZone::Local) const;

			String FormatISODate() const;
			String FormatISOTime() const;
			String FormatISOCombined(const UniChar& sep = 'T') const;

			bool ParseDate(const String& date);
			bool ParseTime(const String& time);
			bool ParseCombined(const String& dateTime);

			bool ParseFormat(const String& dateTime);
			bool ParseFormat(const String& dateTime, const String& format);
			bool ParseFormat(const String& dateTime, const String& format, const DateTime& dateDefault);

			bool ParseISODate(const String& date);
			bool ParseISOTime(const String& time);
			bool ParseISOCombined(const String& dateTime, const UniChar& sep = 'T');
			bool ParseRFC822Date(const String& dateTime);

			// Calendar calculations
			bool IsWorkDay(Country country) const noexcept;
			bool SetToWeekDay(WeekDay weekday, int n = 1, Month month = Month::None, int year = InvalidYear) noexcept;
			bool SetToLastWeekDay(WeekDay weekday, Month month = Month::None, int year = InvalidYear) noexcept;
			DateTime& SetToWeekDayInSameWeek(WeekDay weekday, WeekDayOption options = WeekDayOption::MondayFirst) noexcept;
			DateTime& SetToNextWeekDay(WeekDay weekday) noexcept;
			DateTime& SetToPrevWeekDay(WeekDay weekday) noexcept;
			DateTime& SetToLastMonthDay(Month month = Month::None, int year = InvalidYear) noexcept;
			DateTime& SetToYearDay(int day) noexcept;

			// Astronomical/Historical functions
			double GetRataDie() const noexcept;

			double GetJulianDayNumber() const noexcept;
			double GetModifiedJulianDayNumber() const noexcept;
			DateTime& SetJulianDayNumber(double jdn) noexcept;

			// Time Zone and DST support
			DateTime ToTimeZone(const TimeZoneOffset& tz, bool noDST = false) const noexcept;
			DateTime FromTimeZone(const TimeZoneOffset& tz, bool noDST = false) const noexcept;
			DateTime ToUTC(bool noDST = false) const noexcept;
			bool IsDST(Country country = Country::Default) const noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return IsValid();
			}
			bool operator!() const noexcept
			{
				return !IsValid();
			}

			DateTime& operator=(const DateTime&) noexcept = default;
			operator wxDateTime() const noexcept;
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<DateTime> final
	{
		uint64_t Serialize(IOutputStream& stream, const DateTime& value) const
		{
			return Serialization::WriteObject(stream, value.GetValue());
		}
		uint64_t Deserialize(IInputStream& stream, DateTime& value) const
		{
			int64_t buffer = DateTime().GetValue();
			auto read = Serialization::ReadObject(stream, buffer);
			value.SetValue(buffer);

			return read;
		}
	};
}

namespace std
{
	template<>
	struct hash<kxf::DateTime> final
	{
		size_t operator()(const kxf::DateTime& dateTime) const noexcept
		{
			return std::hash<int64_t>()(dateTime.GetValue());
		}
	};
}
