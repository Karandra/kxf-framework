#pragma once
#include "../Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/DateTime.h"

namespace kxf
{
	class Version;
}

namespace kxf::Private::Version
{
	struct DefaultFormat final
	{
		public:
			static constexpr size_t ItemCount = 8;
			using Array = std::array<DefaultFormat, ItemCount>;

		public:
			XChar m_String[ItemCount + 1] = {};
			int m_Numeric = -1;

		public:
			DefaultFormat() = default;
			DefaultFormat(int number, const XChar* str = nullptr, size_t count = 0)
				:m_Numeric(number)
			{
				SetString(str, count);
			}

		public:
			bool HasNumeric() const
			{
				return m_Numeric >= 0;
			}
			bool HasString() const
			{
				return m_String[0] != '\0';
			}
			void SetString(const XChar* str, size_t count = 0)
			{
				if (!str)
				{
					str = kxfS("");
					count = 0;
				}
				std::char_traits<XChar>::copy(m_String, str, std::min(ItemCount, count));
			}
	};
}

namespace kxf::Private::Version
{
	std::strong_ordering Compare(const kxf::Version& left, const kxf::Version& right);

	bool Parse(const String& source, DefaultFormat::Array& items, size_t& componentCount);
	bool Parse(const String& source, DateTime& dateTime);

	String Format(const DefaultFormat::Array& items, size_t itemCount);
	String Format(const DateTime& dateTime);

	inline bool HasTimePart(const DateTime& dateTime)
	{
		return dateTime.GetHour() != 0 || dateTime.GetMinute() != 0 || dateTime.GetSecond() != 0 || dateTime.GetMillisecond() != 0;
	}
}
