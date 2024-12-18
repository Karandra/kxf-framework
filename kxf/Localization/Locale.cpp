#include "kxf-pch.h"
#include "Locale.h"
#include "Private/LocaleOptions.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/String.h"

namespace
{
	using CharTraits = std::char_traits<wchar_t>;
	constexpr wchar_t g_InvalidChar = std::numeric_limits<wchar_t>::max();

	template<size_t N>
	void CopyToBuffer(wchar_t(&buffer)[N], std::wstring_view localeName) noexcept
	{
		CharTraits::copy(buffer, localeName.data(), std::min(std::size(buffer) - 1, localeName.length()));
	}

	template<size_t N>
	void InvalidateBuffer(wchar_t(&buffer)[N]) noexcept
	{
		buffer[0] = std::numeric_limits<wchar_t>::max();
		buffer[1] = L'\0';
	}

	template<size_t N, class TFunc>
	bool CallGetLocale1(wchar_t(&localeName)[N], TFunc&& func) noexcept
	{
		return std::invoke(func, localeName, std::size(localeName));
	}

	template<class TFunc>
	bool CallGetLocale2(std::wstring& buffer, TFunc&& func)
	{
		ULONG langCount = 0;
		ULONG bufferSize = 0;
		if (std::invoke(func, MUI_LANGUAGE_NAME, &langCount, nullptr, &bufferSize))
		{
			buffer.resize(bufferSize);
			return std::invoke(func, MUI_LANGUAGE_NAME, &langCount, buffer.data(), &bufferSize);
		}
		return false;
	}

	template<class T>
	std::optional<uint32_t> GetLocaleInfoInt(T&& localeName, uint32_t option) noexcept
	{
		DWORD result = std::numeric_limits<DWORD>::max();
		if (::GetLocaleInfoEx(localeName, option|LOCALE_RETURN_NUMBER, reinterpret_cast<wchar_t*>(&result), sizeof(result)) != 0)
		{
			return result;
		}
		return {};
	}
	
	template<class T>
	std::optional<kxf::String> GetLocaleInfoString(T&& localeName, uint32_t option) noexcept
	{
		using namespace kxf;

		int length = ::GetLocaleInfoEx(localeName, option, nullptr, 0);
		if (length != 0)
		{
			String value;
			if (::GetLocaleInfoEx(localeName, option, Utility::StringBuffer(value, length), length) != 0)
			{
				return value;
			}
		}
		return {};
	}
}

namespace kxf
{
	Locale Locale::GetInvariant() noexcept
	{
		Locale locale;
		CharTraits::copy(locale.m_LocaleName, LOCALE_NAME_INVARIANT, std::size(LOCALE_NAME_INVARIANT));

		return locale;
	}
	Locale Locale::GetUserDefault() noexcept
	{
		Locale locale;
		if (CallGetLocale1(locale.m_LocaleName, ::GetUserDefaultLocaleName))
		{
			return locale;
		}
		return {};
	}
	Locale Locale::GetSystemDefault() noexcept
	{
		Locale locale;
		if (CallGetLocale1(locale.m_LocaleName, ::GetSystemDefaultLocaleName))
		{
			return locale;
		}
		return {};
	}
	Locale Locale::GetSystemPreferred() noexcept
	{
		std::wstring buffer;
		if (CallGetLocale2(buffer, ::GetSystemPreferredUILanguages))
		{
			return StringViewOf(buffer.c_str());
		}
		return {};
	}
	Locale Locale::GetThreadPreferred() noexcept
	{
		std::wstring buffer;
		if (CallGetLocale2(buffer, ::GetThreadPreferredUILanguages))
		{
			return StringViewOf(buffer.c_str());
		}
		return {};
	}

	Locale Locale::FromLCID(uint32_t lcid) noexcept
	{
		Locale locale;
		if (::LCIDToLocaleName(lcid, locale.m_LocaleName, std::size(locale.m_LocaleName), LOCALE_ALLOW_NEUTRAL_NAMES) != 0)
		{
			return locale;
		}
		return {};
	}
	Locale Locale::FromLangID(Localization::LangID langID) noexcept
	{
		return FromLCID(MAKELCID(langID.ID, langID.SortOrder));
	}

	Locale::Locale(std::string_view localeName) noexcept
	{
		if (!localeName.empty())
		{
			String temp = String::FromUTF8(localeName);
			CopyToBuffer(m_LocaleName, StringViewOf(temp));
		}
		else
		{
			InvalidateBuffer(m_LocaleName);
		}
	}
	Locale::Locale(std::wstring_view localeName) noexcept
	{
		static_assert(Utility::ArraySize<decltype(m_LocaleName)>::value >= LOCALE_NAME_MAX_LENGTH, "insufficient locale name buffer");

		if (!localeName.empty())
		{
			CopyToBuffer(m_LocaleName, localeName);
		}
		else
		{
			InvalidateBuffer(m_LocaleName);
		}
	}

	bool Locale::IsNull() const noexcept
	{
		return *m_LocaleName == g_InvalidChar || !::IsValidLocaleName(m_LocaleName);
	}
	bool Locale::IsInvariant() const noexcept
	{
		return !IsNull() && std::wstring_view(m_LocaleName) == LOCALE_NAME_INVARIANT;
	}

	std::optional<String> Locale::GetOption(LocaleStrOption option) const
	{
		if (auto nativeOption = Localization::Private::MapLocaleOption(option))
		{
			return GetLocaleInfoString(m_LocaleName, *nativeOption);
		}
		return {};
	}
	std::optional<uint32_t> Locale::GetOption(LocaleIntOption option) const noexcept
	{
		if (auto nativeOption = Localization::Private::MapLocaleOption(option))
		{
			return GetLocaleInfoInt(m_LocaleName, *nativeOption);
		}
		return {};
	}

	bool Locale::SetOption(LocaleStrOption option, const String& value)
	{
		return false;
	}
	bool Locale::SetOption(LocaleIntOption option, uint32_t value)
	{
		return false;
	}

	std::optional<uint32_t> Locale::GetLCID() const noexcept
	{
		uint32_t lcid = ::LocaleNameToLCID(m_LocaleName, 0);
		if (lcid != 0)
		{
			return lcid;
		}
		return {};
	}
	std::optional<Localization::LangID> Locale::GetLangID() const noexcept
	{
		if (auto value = GetLocaleInfoInt(m_LocaleName, LOCALE_ILANGUAGE))
		{
			return Localization::LangID(PRIMARYLANGID(*value), SUBLANGID(*value));
		}
		return {};
	}

	std::optional<String> Locale::GetMonthName(Month month, FlagSet<UnitNameFlag> flags) const
	{
		auto GetValue = [&](LocaleStrOption full, LocaleStrOption abbreviated)
		{
			return GetOption(flags & UnitNameFlag::Abbreviated ? abbreviated : full);
		};

		switch (month)
		{
			case Month::January:
			{
				return GetValue(LocaleStrOption::NativeMonthJanuary, LocaleStrOption::NativeMonthAbbrJanuary);
			}
			case Month::February:
			{
				return GetValue(LocaleStrOption::NativeMonthFebruary, LocaleStrOption::NativeMonthAbbrFebruary);
			}
			case Month::March:
			{
				return GetValue(LocaleStrOption::NativeMonthMarch, LocaleStrOption::NativeMonthAbbrMarch);
			}
			case Month::April:
			{
				return GetValue(LocaleStrOption::NativeMonthApril, LocaleStrOption::NativeMonthAbbrApril);
			}
			case Month::May:
			{
				return GetValue(LocaleStrOption::NativeMonthMay, LocaleStrOption::NativeMonthAbbrMay);
			}
			case Month::June:
			{
				return GetValue(LocaleStrOption::NativeMonthJune, LocaleStrOption::NativeMonthAbbrJune);
			}
			case Month::July:
			{
				return GetValue(LocaleStrOption::NativeMonthJuly, LocaleStrOption::NativeMonthAbbrJuly);
			}
			case Month::August:
			{
				return GetValue(LocaleStrOption::NativeMonthAugust, LocaleStrOption::NativeMonthAbbrAugust);
			}
			case Month::September:
			{
				return GetValue(LocaleStrOption::NativeMonthSeptember, LocaleStrOption::NativeMonthAbbrSeptember);
			}
			case Month::October:
			{
				return GetValue(LocaleStrOption::NativeMonthOctober, LocaleStrOption::NativeMonthAbbrOctober);
			}
			case Month::November:
			{
				return GetValue(LocaleStrOption::NativeMonthNovember, LocaleStrOption::NativeMonthAbbrNovember);
			}
			case Month::December:
			{
				return GetValue(LocaleStrOption::NativeMonthDecember, LocaleStrOption::NativeMonthAbbrDecember);
			}
		};
		return {};
	}
	std::optional<String> Locale::GetWeekDayName(WeekDay weekDay, FlagSet<UnitNameFlag> flags) const
	{
		auto GetValue = [&](LocaleStrOption full, LocaleStrOption abbreviated)
		{
			return GetOption(flags & UnitNameFlag::Abbreviated ? abbreviated : full);
		};

		switch (weekDay)
		{
			case WeekDay::Monday:
			{
				return GetValue(LocaleStrOption::NativeDayMonday, LocaleStrOption::NativeDayAbbrMonday);
			}
			case WeekDay::Tuesday:
			{
				return GetValue(LocaleStrOption::NativeDayTuesday, LocaleStrOption::NativeDayAbbrTuesday);
			}
			case WeekDay::Wednesday:
			{
				return GetValue(LocaleStrOption::NativeDayWednesday, LocaleStrOption::NativeDayAbbrWednesday);
			}
			case WeekDay::Thursday:
			{
				return GetValue(LocaleStrOption::NativeDayTuesday, LocaleStrOption::NativeDayAbbrThursday);
			}
			case WeekDay::Friday:
			{
				return GetValue(LocaleStrOption::NativeDayFriday, LocaleStrOption::NativeDayAbbrFriday);
			}
			case WeekDay::Saturday:
			{
				return GetValue(LocaleStrOption::NativeDaySaturday, LocaleStrOption::NativeDayAbbrSaturday);
			}
			case WeekDay::Sunday:
			{
				return GetValue(LocaleStrOption::NativeDaySunday, LocaleStrOption::NativeDayAbbrSunday);
			}
		};
		return {};
	}
}
