#pragma once
#include "Common.h"
#include "kxf/Serialization/BinarySerializer.h"
class wxTimeSpan;

namespace kxf
{
	class Locale;
	class ITimeClock;
}

namespace kxf::Private
{
	constexpr int MillisecondsInSecond = 1000;
	constexpr int MillisecondsInMinute = MillisecondsInSecond * 60;
	constexpr int MillisecondsInHour = MillisecondsInMinute * 60;
	constexpr int MillisecondsInDay = MillisecondsInHour * 24;
	constexpr int MillisecondsInWeek = MillisecondsInDay * 7;
}

namespace kxf
{
	class KXF_API TimeSpan final
	{
		public:
			static constexpr TimeSpan Milliseconds(int64_t milliseconds) noexcept
			{
				return milliseconds;
			}
			static constexpr TimeSpan Seconds(int64_t seconds) noexcept
			{
				return seconds * Private::MillisecondsInSecond;
			}
			static constexpr TimeSpan Minutes(int64_t minutes) noexcept
			{
				return minutes * Private::MillisecondsInMinute;
			}
			static constexpr TimeSpan Hours(int64_t hours) noexcept
			{
				return hours * Private::MillisecondsInHour;
			}
			static constexpr TimeSpan Days(int64_t days) noexcept
			{
				return days * Private::MillisecondsInDay;
			}
			static constexpr TimeSpan Weeks(int64_t weeks) noexcept
			{
				return weeks * Private::MillisecondsInWeek;
			}

			static TimeSpan Now() noexcept;
			static TimeSpan Now(const ITimeClock& clock) noexcept;

		private:
			int64_t m_Value = 0;

		private:
			constexpr TimeSpan(int64_t value) noexcept
				:m_Value(value)
			{
			}

		public:
			constexpr TimeSpan() noexcept = default;
			constexpr TimeSpan(const TimeSpan&) noexcept = default;
			TimeSpan(const wxTimeSpan& other) noexcept;

		public:
			constexpr bool IsNull() const noexcept
			{
				return m_Value == 0;
			}
			constexpr bool IsNegative() const noexcept
			{
				return m_Value < 0;
			}
			constexpr bool IsPositive() const noexcept
			{
				return m_Value > 0;
			}

			constexpr int64_t GetValue() const noexcept
			{
				return m_Value;
			}
			constexpr TimeSpan& SetValue(int64_t value) noexcept
			{
				m_Value = value;
				return *this;
			}

			constexpr int64_t GetWeeks() const noexcept
			{
				return m_Value / Private::MillisecondsInWeek;
			}
			constexpr int64_t GetDays() const noexcept
			{
				return m_Value / Private::MillisecondsInDay;
			}
			constexpr int64_t GetHours() const noexcept
			{
				return m_Value / Private::MillisecondsInHour;
			}
			constexpr int64_t GetMinutes() const noexcept
			{
				return m_Value / Private::MillisecondsInMinute;
			}
			constexpr int64_t GetSeconds() const noexcept
			{
				return m_Value / Private::MillisecondsInSecond;
			}
			constexpr int64_t GetMilliseconds() const noexcept
			{
				return m_Value;
			}

			constexpr TimeSpan Abs() const noexcept
			{
				return TimeSpan(m_Value < 0 ? -m_Value : m_Value);
			}
			constexpr TimeSpan Negate() const noexcept
			{
				return TimeSpan(-m_Value);
			}

			// Compare two time spans, works with the absolute values
			constexpr bool IsLongerThan(const TimeSpan& other) const noexcept
			{
				return Abs() > other.Abs();
			}
			constexpr bool IsShorterThan(const TimeSpan& other) const noexcept
			{
				return Abs() < other.Abs();
			}

			String Format(const String& format) const;
			String FormatTime(const Locale& locale, FlagSet<TimeFormatFlag> flags = {}) const;

		public:
			constexpr TimeSpan& operator=(const TimeSpan&) noexcept = default;
			operator wxTimeSpan() const noexcept;

			constexpr auto operator<=>(const TimeSpan&) const noexcept = default;
			constexpr bool operator==(const TimeSpan&) const noexcept = default;

			constexpr TimeSpan& operator+=(const TimeSpan& other) noexcept
			{
				m_Value += other.m_Value;
				return *this;
			}
			constexpr TimeSpan& operator-=(const TimeSpan& other) noexcept
			{
				m_Value -= other.m_Value;
				return *this;
			}
			constexpr TimeSpan& operator*=(int multiplier) noexcept
			{
				m_Value *= multiplier;
				return *this;
			}

			constexpr TimeSpan operator-() const noexcept
			{
				return Negate();
			}
			constexpr TimeSpan operator+(const TimeSpan& other) const noexcept
			{
				return m_Value + other.m_Value;
			}
			constexpr TimeSpan operator-(const TimeSpan& other) const noexcept
			{
				return m_Value - other.m_Value;
			}
			constexpr TimeSpan operator*(int multiplier) const noexcept
			{
				return m_Value * multiplier;
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<TimeSpan> final
	{
		uint64_t Serialize(IOutputStream& stream, const TimeSpan& value) const
		{
			return Serialization::WriteObject(stream, value.GetValue());
		}
		uint64_t Deserialize(IInputStream& stream, TimeSpan& value) const
		{
			int64_t buffer = 0;
			auto read = Serialization::ReadObject(stream, buffer);
			value.SetValue(buffer);

			return read;
		}
	};
}

namespace std
{
	template<>
	struct hash<kxf::TimeSpan> final
	{
		size_t operator()(const kxf::TimeSpan& timeSpan) const noexcept
		{
			return std::hash<int64_t>()(timeSpan.GetValue());
		}
	};
}
