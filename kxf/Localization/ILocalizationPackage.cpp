#include "kxf-pch.h"
#include "ILocalizationPackage.h"
#include "kxf/Utility/Numeric.h"

namespace
{
	using namespace kxf;

	const String& PluralStringRussian(const LocalizationItem& item, int value)
	{
		const int lastDigit = Utility::LastDecimalDigit(value);
		if (value == 1 || (value >= 21 && lastDigit == 1))
		{
			return item.GetPluralString(LocalizationItemQuantity::One);
		}
		else if ((value > 0 && value <= 4) || (value >= 21 && lastDigit <= 4))
		{
			return item.GetPluralString(LocalizationItemQuantity::Few);
		}
		else if (value == 0 || value > 4)
		{
			return item.GetPluralString(LocalizationItemQuantity::Many);
		}
		return item.GetPluralString(LocalizationItemQuantity::Other);
	};
}

namespace kxf
{
	const kxf::String& ILocalizationPackage::GetPluralStringForNumber(const LocalizationItem& item, int value) const
	{
		// TODO: Make this configurable inside the localization file itself somehow
		if (GetLocale() == "ru-RU")
		{
			return PluralStringRussian(item, value);
		}
		
		// LocalizationItem will use default algorithm (for English) if we return an empty string
		return NullString;
	}
}
