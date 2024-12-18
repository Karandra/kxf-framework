#pragma once
#include "../Common.h"
#include "../Locale.h"

namespace kxf::Localization::Private
{
	namespace EmbeddedResourceType
	{
		constexpr XChar Android[] = kxfS("AndroidLocalizationPackage");
		constexpr XChar Windows[] = kxfS("WindowsLocalizationPackage";)
		constexpr XChar Qt[] = kxfS("QtLocalizationPackage");
	}

	inline Locale LocaleFromFileName(const String& name)
	{
		return name.BeforeFirst('.');
	}
}
