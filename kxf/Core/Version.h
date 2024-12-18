#pragma once
#include "Common.h"
#include "Private/VersionImpl.h"
#include "String.h"
#include <variant>
class wxVersionInfo;

namespace kxf
{
	enum class VersionType
	{
		None = -1,

		Default,
		DateTime,
	};
}

namespace kxf
{
	class KXF_API Version final
	{
		friend std::strong_ordering Private::Version::Compare(const kxf::Version& left, const kxf::Version& right);

		public:
			using TDefaultItem = Private::Version::DefaultFormat::Array;

		private:
			std::variant<TDefaultItem, DateTime> m_Value;
			VersionType m_Type = VersionType::None;
			size_t m_ComponentCount = 0;

		private:
			bool Parse(const String& source, VersionType type);
			VersionType ParseUnknown(const String& source);

			TDefaultItem& AssignDefault(TDefaultItem value = {})
			{
				m_Value = std::move(value);
				return std::get<static_cast<size_t>(VersionType::Default)>(m_Value);
			}
			DateTime& AssignDateTime(DateTime value = {})
			{
				m_Value = std::move(value);
				return std::get<static_cast<size_t>(VersionType::DateTime)>(m_Value);
			}

		public:
			Version() = default;
			Version(const String& source, VersionType type = VersionType::None)
			{
				if (type == VersionType::None)
				{
					m_Type = ParseUnknown(source);
				}
				else if (Parse(source, type))
				{
					m_Type = type;
				}
			}
			Version(const char* data, VersionType type = VersionType::None)
				:Version(String(data), type)
			{
			}
			Version(const wchar_t* data, VersionType type = VersionType::None)
				:Version(String(data), type)
			{
			}
			Version(const DateTime& dateTime)
			{
				if (dateTime.IsValid())
				{
					m_Value = dateTime;
					m_Type = VersionType::DateTime;
					m_ComponentCount = GetComponentCount();
				}
			}
			Version(const wxVersionInfo& versionInfo);
			Version(std::initializer_list<int> items);

		public:
			bool IsEmpty() const
			{
				return m_Type == VersionType::None || m_ComponentCount == 0 || m_Value.valueless_by_exception();
			}
			VersionType GetType() const
			{
				return m_Type;
			}
			size_t GetComponentCount() const;
			
			String ToString() const;
			wxVersionInfo ToWxVersionInfo(const String& name = {}, const String& description = {}, const String& copyright = {}) const;
			DateTime ToDateTime() const;
			uint64_t ToInteger() const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsEmpty();
			}
			bool operator!() const noexcept
			{
				return IsEmpty();
			}

			std::strong_ordering operator<=>(const Version& other) const;
			bool operator==(const Version& other) const;

			operator String() const
			{
				return ToString();
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<Version> final
	{
		uint64_t Serialize(IOutputStream& stream, const Version& value) const;
		uint64_t Deserialize(IInputStream& stream, Version& value) const;
	};
}
