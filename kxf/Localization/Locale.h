#pragma once
#include "Common.h"
#include "kxf/Core/String.h"

namespace kxf::Localization
{
	struct LangID final
	{
		uint16_t ID = 0;
		uint16_t SortOrder = 0;

		LangID() noexcept = default;
		LangID(uint16_t id, uint16_t sortOrder) noexcept
			:ID(id), SortOrder(sortOrder)
		{
		}
	};
}

namespace kxf
{
	class KXF_API Locale final
	{
		friend class DateTime;

		public:
			static Locale GetInvariant() noexcept;
			static Locale GetUserDefault() noexcept;
			static Locale GetSystemDefault() noexcept;
			static Locale GetSystemPreferred() noexcept;
			static Locale GetThreadPreferred() noexcept;

			static Locale FromLCID(uint32_t lcid) noexcept;
			static Locale FromLangID(Localization::LangID langID) noexcept;

		private:
			wchar_t m_LocaleName[128] = {std::numeric_limits<wchar_t>::max()};

		public:
			Locale() noexcept = default;
			Locale(std::string_view localeName) noexcept;
			Locale(std::wstring_view localeName) noexcept;
			Locale(const char* localeName) noexcept
				:Locale(std::string_view(localeName))
			{
			}
			Locale(const wchar_t* localeName) noexcept
				:Locale(std::wstring_view(localeName))
			{
			}
			Locale(const String& localeName) noexcept
				:Locale(StringViewOf(localeName))
			{
			}

		public:
			bool IsNull() const noexcept;
			bool IsInvariant() const noexcept;
			String GetName() const
			{
				return m_LocaleName;
			}

			std::optional<String> GetOption(LocaleStrOption option) const;
			std::optional<uint32_t> GetOption(LocaleIntOption option) const noexcept;

			bool SetOption(LocaleStrOption option, const String& value);
			bool SetOption(LocaleIntOption option, uint32_t value);

			std::optional<uint32_t> GetLCID() const noexcept;
			std::optional<Localization::LangID> GetLangID() const noexcept;

			std::optional<String> GetMonthName(Month month, FlagSet<UnitNameFlag> flags = UnitNameFlag::None) const;
			std::optional<String> GetWeekDayName(WeekDay weekDay, FlagSet<UnitNameFlag> flags = UnitNameFlag::None) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			auto operator<=>(const Locale& other) const noexcept
			{
				if (this == &other)
				{
					return std::strong_ordering::equal;
				}
				return std::wstring_view(m_LocaleName) <=> std::wstring_view(other.m_LocaleName);
			}
			bool operator==(const Locale& other) const noexcept
			{
				if (this == &other)
				{
					return true;
				}
				else if (!IsNull() && !other.IsNull())
				{
					return std::wstring_view(m_LocaleName) == std::wstring_view(other.m_LocaleName);
				}
				return false;
			}
	};
}
