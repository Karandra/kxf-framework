#pragma once
#include "Common.h"
#include "TimeSpan.h"
#include "kxf/Utility/Numeric.h"
class wxDateSpan;

namespace kxf
{
	class Locale;
}

namespace kxf
{
	class KXF_API DateSpan final
	{
		public:
			static constexpr DateSpan Days(int days) noexcept
			{
				return DateSpan(0, 0, 0, days);
			}
			static constexpr DateSpan Weeks(int weeks) noexcept
			{
				return DateSpan(0, 0, weeks, 0);
			}
			static constexpr DateSpan Months(int months) noexcept
			{
				return DateSpan(0, months, 0, 0);
			}
			static constexpr DateSpan Years(int years) noexcept
			{
				return DateSpan(years, 0, 0, 0);
			}

		private:
			int m_Years = 0;
			int m_Months = 0;
			int m_Weeks = 0;
			int m_Days = 0;

		public:
			constexpr DateSpan() noexcept = default;
			constexpr DateSpan(const DateSpan&) noexcept = default;
			constexpr DateSpan(const TimeSpan& timeSpan) noexcept
				:m_Days(timeSpan.GetDays())
			{
			}
			constexpr DateSpan(int years, int months, int weeks, int days) noexcept
				:m_Years(years), m_Months(months), m_Weeks(weeks), m_Days(days)
			{
			}
			DateSpan(const wxDateSpan& dateSpan) noexcept;

		public:
			constexpr bool IsNull() const noexcept
			{
				return m_Days == 0 && m_Weeks == 0 && m_Months == 0 && m_Years == 0;
			}
			constexpr bool IsNegative() const noexcept
			{
				return m_Days < 0 && m_Weeks < 0 && m_Months < 0 && m_Years < 0;
			}
			constexpr bool IsPositive() const noexcept
			{
				return m_Days > 0 && m_Weeks > 0 && m_Months > 0 && m_Years > 0;
			}

			constexpr int GetDays() const noexcept
			{
				return m_Days;
			}
			constexpr DateSpan& SetDays(int days) noexcept
			{
				m_Days = days;
				return *this;
			}

			constexpr int GetWeeks() const noexcept
			{
				return m_Weeks;
			}
			constexpr DateSpan& SetWeeks(int weeks) noexcept
			{
				m_Weeks = weeks;
				return *this;
			}

			constexpr int GetMonths() const noexcept
			{
				return m_Months;
			}
			constexpr DateSpan& SetMonths(int months) noexcept
			{
				m_Months = months;
				return *this;
			}

			constexpr int GetYears() const noexcept
			{
				return m_Years;
			}
			constexpr DateSpan& SetYears(int years) noexcept
			{
				m_Years = years;
				return *this;
			}

			constexpr int GetTotalDays() const noexcept
			{
				return 7 * m_Weeks + m_Days;
			}
			constexpr int GetTotalMonths() const noexcept
			{
				return 12 * m_Years + m_Months;
			}

			constexpr DateSpan Abs() const noexcept
			{
				return DateSpan(Utility::Abs(m_Years), Utility::Abs(m_Months), Utility::Abs(m_Weeks), Utility::Abs(m_Days));
			}
			constexpr DateSpan Negate() const noexcept
			{
				return DateSpan(-m_Years, -m_Months, -m_Weeks, -m_Days);
			}

			// Compare two date spans, works with the absolute values
			constexpr bool IsLongerThan(const DateSpan& other) const noexcept
			{
				return Abs() > other.Abs();
			}
			constexpr bool IsShorterThan(const DateSpan& other) const noexcept
			{
				return Abs() < other.Abs();
			}

			String Format(const String& format) const;
			String FormatDate(const Locale& locale, DateFormatFlag flags = DateFormatFlag::None) const;

		public:
			constexpr DateSpan& operator=(const DateSpan&) noexcept = default;
			operator wxDateSpan() const noexcept;

			constexpr std::strong_ordering operator<=>(const DateSpan& other) const noexcept
			{
				if (auto cmp = m_Years <=> m_Years; cmp != 0)
				{
					return cmp;
				}
				if (auto cmp = m_Months <=> m_Months; cmp != 0)
				{
					return cmp;
				}
				return GetTotalDays() <=> other.GetTotalDays();
			}
			constexpr bool operator==(const DateSpan& other) const noexcept
			{
				return m_Years == other.m_Years && m_Months == other.m_Months && GetTotalDays() == other.GetTotalDays();
			}

			constexpr DateSpan& operator+=(const DateSpan& other) noexcept
			{
				m_Years += other.m_Years;
				m_Months += other.m_Months;
				m_Weeks += other.m_Weeks;
				m_Days += other.m_Days;

				return *this;
			}
			constexpr DateSpan& operator-=(const DateSpan& other) noexcept
			{
				m_Years -= other.m_Years;
				m_Months -= other.m_Months;
				m_Weeks -= other.m_Weeks;
				m_Days -= other.m_Days;

				return *this;
			}
			constexpr DateSpan& operator*=(int factor) noexcept
			{
				m_Years *= factor;
				m_Months *= factor;
				m_Weeks *= factor;
				m_Days *= factor;

				return *this;
			}

			constexpr DateSpan operator-() const noexcept
			{
				return Negate();
			}
			constexpr DateSpan operator+(const DateSpan& other) const noexcept
			{
				return DateSpan(m_Years + other.m_Years, m_Months + other.m_Months, m_Weeks + other.m_Weeks, m_Days + other.m_Days);
			}
			constexpr DateSpan operator-(const DateSpan& other) const noexcept
			{
				return DateSpan(m_Years - other.m_Years, m_Months - other.m_Months, m_Weeks - other.m_Weeks, m_Days - other.m_Days);
			}
			constexpr DateSpan operator*(int factor) const noexcept
			{
				return DateSpan(m_Years * factor, m_Months * factor, m_Weeks * factor, m_Days * factor);
			}
	};
}
