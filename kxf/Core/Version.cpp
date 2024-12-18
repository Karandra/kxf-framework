#include "kxf-pch.h"
#include "Version.h"
#include <wx/versioninfo.h>

namespace
{
	template<kxf::VersionType type, class T>
	auto&& GetItem(T&& items)
	{
		using namespace kxf;

		return std::get<static_cast<size_t>(type)>(items);
	}
}

namespace kxf
{
	bool Version::Parse(const String& source, VersionType type)
	{
		switch (type)
		{
			case VersionType::Default:
			{
				return Private::Version::Parse(source, AssignDefault(), m_ComponentCount);
			}
			case VersionType::DateTime:
			{
				return Private::Version::Parse(source, AssignDateTime());
			}
		};
		return false;
	}
	VersionType Version::ParseUnknown(const String& source)
	{
		// Order is important here
		if (Private::Version::Parse(source, AssignDateTime()))
		{
			return VersionType::DateTime;
		}
		else if (Private::Version::Parse(source, AssignDefault(), m_ComponentCount))
		{
			return VersionType::Default;
		}
		return VersionType::None;
	}

	Version::Version(const wxVersionInfo& versionInfo)
		:m_Type(VersionType::Default), m_ComponentCount(3)
	{
		auto& version = AssignDefault();
		version[0].m_Numeric = versionInfo.GetMajor();
		version[1].m_Numeric = versionInfo.GetMinor();
		version[2].m_Numeric = versionInfo.GetMicro();
	}
	Version::Version(std::initializer_list<int> items)
	{
		auto& version = AssignDefault();

		size_t i = 0;
		for (int item: items)
		{
			if (item < 0)
			{
				break;
			}
			if (i + 1 >= Private::Version::DefaultFormat::ItemCount)
			{
				break;
			}

			version[i].m_Numeric = item;
			i++;
		}

		m_ComponentCount = i;
		if (m_ComponentCount != 0)
		{
			m_Type = VersionType::Default;
		}
	}

	size_t Version::GetComponentCount() const
	{
		switch (m_Type)
		{
			case VersionType::Default:
			{
				return m_ComponentCount;
			}
			case VersionType::DateTime:
			{
				return Private::Version::HasTimePart(GetItem<VersionType::DateTime>(m_Value)) ? 2 : 1;
			}
		};
		return 0;
	}
	String Version::ToString() const
	{
		switch (m_Type)
		{
			case VersionType::Default:
			{
				return Private::Version::Format(GetItem<VersionType::Default>(m_Value), m_ComponentCount);
			}
			case VersionType::DateTime:
			{
				return Private::Version::Format(GetItem<VersionType::DateTime>(m_Value));
			}
		};
		return {};
	}
	wxVersionInfo Version::ToWxVersionInfo(const String& name, const String& description, const String& copyright) const
	{
		const auto& items = GetItem<VersionType::Default>(m_Value);

		auto GetPart = [&](size_t index)
		{
			int value = items[index].m_Numeric;
			return value >= 0 ? value : 0;
		};
		return wxVersionInfo(name, GetPart(0), GetPart(1), GetPart(2), description, copyright);
	}
	DateTime Version::ToDateTime() const
	{
		if (m_Type == VersionType::DateTime)
		{
			return GetItem<VersionType::DateTime>(m_Value);
		}
		return {};
	}
	uint64_t Version::ToInteger() const
	{
		switch (m_Type)
		{
			case VersionType::DateTime:
			{
				return GetItem<VersionType::DateTime>(m_Value).GetValue();
			}
			case VersionType::Default:
			{
				uint64_t result = 0;

				auto& value = GetItem<VersionType::Default>(m_Value);
				for (size_t i = 0; i < m_ComponentCount; i++)
				{
					auto& item = value[i];
					if (item.HasNumeric())
					{
						auto magintude = static_cast<uint64_t>(std::pow(10, m_ComponentCount - i));
						result += static_cast<uint64_t>(item.m_Numeric) * magintude;
					}
					if (item.HasString())
					{
						const size_t max = (std::size(item.m_String) - 1) * 255;
						const size_t length = std::char_traits<XChar>::length(item.m_String);

						uint64_t sum = max;
						for (size_t i = 0; i < length; i++)
						{
							sum -= item.m_String[i];
						}
						result += sum;
					}
				}
			}
		};
		return 0;
	}

	std::strong_ordering Version::operator<=>(const Version& other) const
	{
		return Private::Version::Compare(*this, other);
	}
	bool Version::operator==(const Version& other) const
	{
		return Private::Version::Compare(*this, other) == 0;
	}
}

namespace kxf
{
	uint64_t BinarySerializer<Version>::Serialize(IOutputStream& stream, const Version& value) const
	{
		return Serialization::WriteObject(stream, value.GetType()) + Serialization::WriteObject(stream, value.ToString());
	}
	uint64_t BinarySerializer<Version>::Deserialize(IInputStream& stream, Version& value) const
	{
		VersionType type = VersionType::None;
		auto read = Serialization::ReadObject(stream, type);

		String buffer;
		read += Serialization::ReadObject(stream, buffer);
		value = {std::move(buffer), type};

		return read;
	}
}
