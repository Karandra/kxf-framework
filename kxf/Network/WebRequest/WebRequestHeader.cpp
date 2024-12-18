#include "kxf-pch.h"
#include "WebRequestHeader.h"

namespace kxf
{
	WebRequestHeader WebRequestHeader::UserAgent(String value)
	{
		return {"User-Agent", std::move(value)};
	}
	WebRequestHeader WebRequestHeader::Referer(String value)
	{
		return {"Referer", std::move(value)};
	}
	WebRequestHeader WebRequestHeader::Cookie(String value)
	{
		return {"Cookie", std::move(value)};
	}

	bool WebRequestHeader::AddValue(const String& value, WebRequestHeaderFlag separator)
	{
		auto Coalesce = [](String& result, const String& value, XChar c)
		{
			if (!result.IsEmpty())
			{
				XChar separator[] = {c, ' ', 0};
				result += separator;
			}
			result += value;
		};

		switch (separator)
		{
			case WebRequestHeaderFlag::CoalesceComma:
			{
				Coalesce(m_Value, value, ',');
				return true;
			}
			case WebRequestHeaderFlag::CoalesceSemicolon:
			{
				Coalesce(m_Value, value, ';');
				return true;
			}
		};
		return false;
	}
	String WebRequestHeader::Format() const
	{
		if (!IsNull())
		{
			if (!m_Value.IsEmpty())
			{
				return kxf::Format("{}: {}", m_Name, m_Value);
			}
			else
			{
				String result = m_Name;
				result += ':';

				return result;
			}
		}
		return {};
	}

	bool WebRequestHeader::operator==(const WebRequestHeader& other) const noexcept
	{
		return this == &other || (m_Name == other.m_Name && m_Value == other.m_Value);
	}
	bool WebRequestHeader::operator!=(const WebRequestHeader& other) const noexcept
	{
		return this != &other || m_Name != other.m_Name || m_Value != other.m_Value;
	}
}

namespace std
{
	size_t std::hash<kxf::WebRequestHeader>::operator()(const kxf::WebRequestHeader& header) const noexcept
	{
		if (header)
		{
			std::hash<kxf::String> hash;
			return hash(header.m_Name) ^ hash(header.m_Value);
		}
		return 0;
	}
}
