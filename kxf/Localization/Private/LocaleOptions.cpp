#include "kxf-pch.h"
#include "LocaleOptions.h"
#include <WinNls.h>

namespace kxf::Localization::Private
{
	std::optional<uint32_t> MapLocaleOption(LocaleStrOption option) noexcept
	{
		switch (option)
		{
			case LocaleStrOption::LocaleName:
			{
				return LOCALE_SNAME;
			}
			case LocaleStrOption::DecimalSeparator:
			{
				return LOCALE_SDECIMAL;
			}

			case LocaleStrOption::ISOCountryName:
			{
				return LOCALE_SISO3166CTRYNAME2;
			}
			case LocaleStrOption::ISOLanguageName:
			{
				return LOCALE_SISO639LANGNAME2;
			}

			case LocaleStrOption::EnglishCountryName:
			{
				return LOCALE_SENGLISHCOUNTRYNAME;
			}
			case LocaleStrOption::EnglishLanguageName:
			{
				return LOCALE_SENGLISHLANGUAGENAME;
			}
			case LocaleStrOption::EnglishLanguageDisplayName:
			{
				return LOCALE_SENGLISHDISPLAYNAME;
			}

			case LocaleStrOption::LocalizedCountryName:
			{
				return LOCALE_SLOCALIZEDCOUNTRYNAME;
			}
			case LocaleStrOption::LocalizedLanguageName:
			{
				return LOCALE_SLOCALIZEDLANGUAGENAME;
			}
			case LocaleStrOption::LocalizedLanguageDisplayName:
			{
				return LOCALE_SLOCALIZEDDISPLAYNAME;
			}

			case LocaleStrOption::NativeCountryName:
			{
				return LOCALE_SNATIVECOUNTRYNAME;
			}
			case LocaleStrOption::NativeCurrencyName:
			{
				return LOCALE_SNATIVECURRNAME;
			}
			case LocaleStrOption::NativeLanguageName:
			{
				return LOCALE_SNATIVELANGUAGENAME;
			}
			case LocaleStrOption::NativeDisplayName:
			{
				return LOCALE_SNATIVEDISPLAYNAME;
			}
			case LocaleStrOption::NativeDigits:
			{
				return LOCALE_SNATIVEDIGITS;
			}

			case LocaleStrOption::ShortDate:
			{
				return LOCALE_SSHORTDATE;
			}
			case LocaleStrOption::ShortTime:
			{
				return LOCALE_SSHORTTIME;
			}
			case LocaleStrOption::LongDate:
			{
				return LOCALE_SLONGDATE;
			}
			case LocaleStrOption::LongTime:
			{
				return LOCALE_STIMEFORMAT;
			}

			case LocaleStrOption::NativeMonthJanuary:
			{
				return LOCALE_SMONTHNAME1;
			}
			case LocaleStrOption::NativeMonthFebruary:
			{
				return LOCALE_SMONTHNAME2;
			}
			case LocaleStrOption::NativeMonthMarch:
			{
				return LOCALE_SMONTHNAME3;
			}
			case LocaleStrOption::NativeMonthApril:
			{
				return LOCALE_SMONTHNAME4;
			}
			case LocaleStrOption::NativeMonthMay:
			{
				return LOCALE_SMONTHNAME5;
			}
			case LocaleStrOption::NativeMonthJune:
			{
				return LOCALE_SMONTHNAME6;
			}
			case LocaleStrOption::NativeMonthJuly:
			{
				return LOCALE_SMONTHNAME7;
			}
			case LocaleStrOption::NativeMonthAugust:
			{
				return LOCALE_SMONTHNAME8;
			}
			case LocaleStrOption::NativeMonthSeptember:
			{
				return LOCALE_SMONTHNAME9;
			}
			case LocaleStrOption::NativeMonthOctober:
			{
				return LOCALE_SMONTHNAME10;
			}
			case LocaleStrOption::NativeMonthNovember:
			{
				return LOCALE_SMONTHNAME11;
			}
			case LocaleStrOption::NativeMonthDecember:
			{
				return LOCALE_SMONTHNAME12;
			}

			case LocaleStrOption::NativeMonthAbbrJanuary:
			{
				return LOCALE_SABBREVMONTHNAME1;
			}
			case LocaleStrOption::NativeMonthAbbrFebruary:
			{
				return LOCALE_SABBREVMONTHNAME2;
			}
			case LocaleStrOption::NativeMonthAbbrMarch:
			{
				return LOCALE_SABBREVMONTHNAME3;
			}
			case LocaleStrOption::NativeMonthAbbrApril:
			{
				return LOCALE_SABBREVMONTHNAME4;
			}
			case LocaleStrOption::NativeMonthAbbrMay:
			{
				return LOCALE_SABBREVMONTHNAME5;
			}
			case LocaleStrOption::NativeMonthAbbrJune:
			{
				return LOCALE_SABBREVMONTHNAME6;
			}
			case LocaleStrOption::NativeMonthAbbrJuly:
			{
				return LOCALE_SABBREVMONTHNAME7;
			}
			case LocaleStrOption::NativeMonthAbbrAugust:
			{
				return LOCALE_SABBREVMONTHNAME8;
			}
			case LocaleStrOption::NativeMonthAbbrSeptember:
			{
				return LOCALE_SABBREVMONTHNAME9;
			}
			case LocaleStrOption::NativeMonthAbbrOctober:
			{
				return LOCALE_SABBREVMONTHNAME10;
			}
			case LocaleStrOption::NativeMonthAbbrNovember:
			{
				return LOCALE_SABBREVMONTHNAME11;
			}
			case LocaleStrOption::NativeMonthAbbrDecember:
			{
				return LOCALE_SABBREVMONTHNAME12;
			}

			case LocaleStrOption::NativeDayMonday:
			{
				return LOCALE_SDAYNAME1;
			}
			case LocaleStrOption::NativeDayTuesday:
			{
				return LOCALE_SDAYNAME2;
			}
			case LocaleStrOption::NativeDayWednesday:
			{
				return LOCALE_SDAYNAME3;
			}
			case LocaleStrOption::NativeDayThursday:
			{
				return LOCALE_SDAYNAME4;
			}
			case LocaleStrOption::NativeDayFriday:
			{
				return LOCALE_SDAYNAME5;
			}
			case LocaleStrOption::NativeDaySaturday:
			{
				return LOCALE_SDAYNAME6;
			}
			case LocaleStrOption::NativeDaySunday:
			{
				return LOCALE_SDAYNAME7;
			}

			case LocaleStrOption::NativeDayAbbrMonday:
			{
				return LOCALE_SABBREVDAYNAME1;
			}
			case LocaleStrOption::NativeDayAbbrTuesday:
			{
				return LOCALE_SABBREVDAYNAME2;
			}
			case LocaleStrOption::NativeDayAbbrWednesday:
			{
				return LOCALE_SABBREVDAYNAME3;
			}
			case LocaleStrOption::NativeDayAbbrThursday:
			{
				return LOCALE_SABBREVDAYNAME4;
			}
			case LocaleStrOption::NativeDayAbbrFriday:
			{
				return LOCALE_SABBREVDAYNAME5;
			}
			case LocaleStrOption::NativeDayAbbrSaturday:
			{
				return LOCALE_SABBREVDAYNAME6;
			}
			case LocaleStrOption::NativeDayAbbrSunday:
			{
				return LOCALE_SABBREVDAYNAME7;
			}
		};
		return {};
	}
	std::optional<uint32_t> MapLocaleOption(LocaleIntOption option) noexcept
	{
		switch (option)
		{
			case LocaleIntOption::MeasureSystem:
			{
				return LOCALE_IMEASURE;
			}
			case LocaleIntOption::TimeFormat:
			{
				return LOCALE_ITIME;
			}
			case LocaleIntOption::DateFormat:
			{
				return LOCALE_IDATE;
			}
		};
		return {};
	}
}
