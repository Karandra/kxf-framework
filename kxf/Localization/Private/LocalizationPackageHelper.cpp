#include "KxfPCH.h"
#include "LocalizationPackageHelper.h"
#include "LocalizationResources.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/Serialization/XML.h"
#include "kxf/Utility/Enumerator.h"

namespace kxf::Localization::Private
{
	const LocalizationItem* ItemsPackageHelper::GetItem(const ResourceID& id) const
	{
		auto it = m_Items->find(id);
		if (it != m_Items->end())
		{
			return &it->second;
		}
		return nullptr;
	}
	CallbackResult<void> ItemsPackageHelper::EnumItems(CallbackFunction<const ResourceID&, const LocalizationItem&> func) const
	{
		if (m_Items && !m_Items->empty())
		{
			func.ResetState();
			for (const auto& [id, item]: *m_Items)
			{
				if (func.Invoke(id, item).ShouldTerminate())
				{
					break;
				}
			}

			return func.Finalize();
		}
		return {};
	}
}

namespace kxf::Localization::Private
{
	bool XMLPackageHelper::Load(IInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme)
	{
		if (stream && DoLoadXML(XMLDocument(stream), loadingScheme))
		{
			DoSetLocale(locale);
			return true;
		}
		return false;
	}
	bool XMLPackageHelper::Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme)
	{
		auto DoLoad = [&](const String& resType)
		{
			if (auto buffer = library.GetResource(resType, name.GetName()); !buffer.empty())
			{
				Locale usedLocale = locale;
				if (!usedLocale)
				{
					usedLocale = Localization::Private::LocaleFromFileName(name);
				}
				if (DoLoadXML(String::FromUnknownEncoding({reinterpret_cast<const char*>(buffer.data()), buffer.size_bytes()}), loadingScheme))
				{
					DoSetLocale(usedLocale);
					return true;
				}
			}
			return false;
		};

		using namespace Localization::Private::EmbeddedResourceType;
		return DoLoad(Android) || DoLoad(Windows) || DoLoad(Qt);
	}
}
