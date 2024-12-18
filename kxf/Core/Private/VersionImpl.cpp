#include "kxf-pch.h"
#include "VersionImpl.h"
#include "../Version.h"

namespace
{
	template<kxf::VersionType type, class T>
	auto&& GetItem(T&& items)
	{
		using namespace kxf;

		return std::get<ToInt(type)>(items);
	}
}

namespace kxf::Private::Version
{
	std::strong_ordering Compare(const kxf::Version& left, const kxf::Version& right)
	{
		if (left.GetType() == right.GetType())
		{
			switch (left.GetType())
			{
				case VersionType::Default:
				{
					const auto& itemsLeft = GetItem<VersionType::Default>(left.m_Value);
					const auto& itemsRight = GetItem<VersionType::Default>(right.m_Value);

					for (size_t i = 0; i < std::max(left.GetComponentCount(), right.GetComponentCount()); i++)
					{
						if (auto cmp = itemsLeft[i].m_Numeric <=> itemsRight[i].m_Numeric; cmp != 0)
						{
							return cmp;
						}
					}
					return std::strong_ordering::equal;
				}
				case VersionType::DateTime:
				{
					return left.ToDateTime() <=> right.ToDateTime();
				}
			};
		}
		return left.GetType() <=> right.GetType();
	}

	bool Parse(const String& source, DefaultFormat::Array& items, size_t& componentCount)
	{
		items.fill({});
		componentCount = 0;

		if (!source.IsEmpty())
		{
			// TODO: Rewrite this
			size_t pos = 0;
			while (pos != String::npos && componentCount < DefaultFormat::ItemCount)
			{
				XChar* endPtr = nullptr;
				size_t endIndex = (endPtr - source.wc_str()) / sizeof(XChar);

				items[componentCount].m_Numeric = std::wcstol(source.wc_str() + pos + (pos == 0 ? 0 : 1), &endPtr, 10);
				if (endPtr && endIndex != (size_t)pos && *endPtr != '.')
				{
					const XChar* nextDotPtr = wcschr(endPtr, L'.');
					items[componentCount].SetString(endPtr, (nextDotPtr ? nextDotPtr : source.wc_str() + source.length()) - endPtr);
				}
				componentCount++;

				pos = source.Find('.', {}, pos + 1);
			}

			// Check
			for (size_t i = 0; i < componentCount; i++)
			{
				if (!items[i].HasNumeric())
				{
					componentCount = 0;
					return false;
				}
			}

			// Remove trailing zeros, but leave versions like '1.0' untouched.
			if (componentCount > 2)
			{
				for (size_t i = componentCount - 1; i > 1; i--)
				{
					if (items[i].m_Numeric == 0 && !items[i].HasString())
					{
						--componentCount;
					}
					else
					{
						break;
					}
				}
			}
			return true;
		}

		componentCount = 0;
		return false;
	}
	bool Parse(const String& source, DateTime& dateTime)
	{
		return dateTime.ParseISOCombined(source) || dateTime.ParseISOCombined(source, ' ') || dateTime.ParseISODate(source) || dateTime.ParseRFC822Date(source);
	}

	String Format(const DefaultFormat::Array& items, size_t itemCount)
	{
		if (!items.empty() && itemCount != 0)
		{
			String result;
			result.reserve(itemCount * 2);

			for (size_t i = 0; i < itemCount; i++)
			{
				const int num = items[i].m_Numeric;
				const XChar* string = items[i].m_String;

				// Don't print '0' for first element if it has non-empty string part
				if (i == 0 && num == 0 && *string)
				{
					result += string;
				}
				else
				{
					result += ToString(num);
					if (*string)
					{
						result += string;
					}
				}

				// Add dot if this is not the last part
				if (i + 1 != itemCount)
				{
					result += '.';
				}
			}

			// Print single component versions as 'Nabc.0' instead of just 'Nabc'.
			if (itemCount == 1)
			{
				result += ".0";
			}
			return result;
		}
		return {};
	}
	String Format(const DateTime& dateTime)
	{
		if (dateTime.IsValid())
		{
			return HasTimePart(dateTime) ? dateTime.FormatISOCombined() : dateTime.FormatISODate();
		}
		return {};
	}
}
