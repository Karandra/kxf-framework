#pragma once
#include "Common.h"
#include "TimeSpan.h"

namespace kxf
{
	enum class TimeZone
	{
		// The time in the current time zone
		Local,

		// Zones from UTC (Universal Time Coordinated): they're guaranteed to be
		// consequent numbers, so writing something like `UTC0 + offset' is
		// safe if abs(offset) <= 12

		// Underscore stands for minus
		UTC_12, UTC_11, UTC_10, UTC_9, UTC_8, UTC_7,
		UTC_6, UTC_5, UTC_4, UTC_3, UTC_2, UTC_1,

		UTC0,
		UTC = UTC0,

		UTC1, UTC2, UTC3, UTC4, UTC5, UTC6,
		UTC7, UTC8, UTC9, UTC10, UTC11, UTC12, UTC13,
		// Note that UTC12 and UTC_12 are not the same, there is a difference of exactly one day between them

		// Symbolic names for some time zones
		// TODO: Add more symbolic timezone names here

		// Europe
		EU_WET = UTC0,                         // Western Europe Time
		EU_WEST = UTC1,                        // Western Europe Summer Time
		EU_CET = UTC1,                         // Central Europe Time
		EU_CEST = UTC2,                        // Central Europe Summer Time
		EU_EET = UTC2,                         // Eastern Europe Time
		EU_EEST = UTC3,                        // Eastern Europe Summer Time
		EU_MSK = UTC3,                         // Moscow Time
		EU_MSD = UTC4,                         // Moscow Summer Time

		// North America
		NA_AST = UTC_4,                        // Atlantic Standard Time
		NA_ADT = UTC_3,                        // Atlantic Daylight Time
		NA_EST = UTC_5,                        // Eastern Standard Time
		NA_EDT = UTC_4,                        // Eastern Daylight Saving Time
		NA_CST = UTC_6,                        // Central Standard Time
		NA_CDT = UTC_5,                        // Central Daylight Saving Time
		NA_MST = UTC_7,                        // Mountain Standard Time
		NA_MDT = UTC_6,                        // Mountain Daylight Saving Time
		NA_PST = UTC_8,                        // Pacific Standard Time
		NA_PDT = UTC_7,                        // Pacific Daylight Saving Time
		NA_HST = UTC_10,                       // Hawaiian Standard Time
		NA_AKST = UTC_9,                       // Alaska Standard Time
		NA_AKDT = UTC_8,                       // Alaska Daylight Saving Time

		// Australia
		AU_WST = UTC8,							// Western Standard Time
		AU_CST = UTC13 + 1,						// Central Standard Time (+9.5)
		AU_EST = UTC10,							// Eastern Standard Time
		AU_ESST = UTC11,						// Eastern Summer Time

		// New Zealand
		NZ_NZST = UTC12,                       // Standard Time
		NZ_NZDT = UTC13,                       // Daylight Saving Time
	};
}

namespace kxf
{
	class KXF_API TimeZoneOffset final
	{
		private:
			TimeSpan m_Offset;

		private:
			void FromTimeZone(TimeZone tz) noexcept;

		public:
			TimeZoneOffset(const TimeSpan& offset = {}) noexcept
				:m_Offset(offset)
			{
			}
			TimeZoneOffset(TimeZone tz) noexcept
			{
				FromTimeZone(tz);
			}

		public:
			bool IsLocal() const noexcept;
			TimeSpan GetOffset() const noexcept;

		public:
			auto operator<=>(const TimeZoneOffset&) const noexcept = default;
	};
}
