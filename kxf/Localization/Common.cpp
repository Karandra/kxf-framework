#include "KxfPCH.h"
#include "Common.h"
#include "Locale.h"
#include "ILocalizationPackage.h"
#include "kxf/UI/WidgetID.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/wxWidgets/StandardLocalization.h"
#include "kxf/Utility/String.h"
#include "Private/LocalizationResources.h"

namespace
{
	kxf::CallbackCommand OnSearchPackage(kxf::CallbackFunction<kxf::Locale, kxf::FileItem>& func, kxf::FileItem item)
	{
		using namespace kxf;

		// Extract locale name from names like 'en-US.Application.xml'
		if (Locale locale = Localization::Private::LocaleFromFileName(item.GetName()))
		{
			String name = item.GetName().BeforeLast('.');
			if (!name.IsEmpty())
			{
				return func.Invoke(std::move(locale), std::move(item)).GetLastCommand();
			}
		}
		return CallbackCommand::Continue;
	}
}

namespace kxf::Localization
{
	String GetStandardString(StdID id)
	{
		return wxWidgets::LocalizeLabelString(id);
	}
	String GetStandardString(WidgetID id)
	{
		return wxWidgets::LocalizeLabelString(id.GetValue());
	}

	size_t SearchPackages(const IFileSystem& fileSystem, const FSPath& directory, CallbackFunction<Locale, FileItem> func)
	{
		Utility::UnorderedSetNoCase<String> extensions;
		for (auto&& classInfo: RTTI::GetClassInfo<ILocalizationPackage>().EnumDerivedClasses())
		{
			if (auto instance = classInfo.CreateObjectInstance<ILocalizationPackage>())
			{
				for (auto ext: instance->GetFileExtensions())
				{
					extensions.insert(ext);
				}
			}
		}

		func.Reset();
		for (FileItem item: fileSystem.EnumItems(directory, {}, FSActionFlag::LimitToFiles))
		{
			if (extensions.find(item.GetFileExtension()) != extensions.end())
			{
				if (OnSearchPackage(func, std::move(item)) == CallbackCommand::Terminate)
				{
					break;
				}
			}
		}
		return func.GetCount();
	}
	size_t SearchPackages(const DynamicLibrary& library, CallbackFunction<Locale, FileItem> func)
	{
		if (library)
		{
			using namespace Localization::Private;

			func.Reset();
			library.EnumResourceNames(EmbeddedResourceType::Android, [&](String name)
			{
				return OnSearchPackage(func, FileItem(std::move(name)));
			});
			library.EnumResourceNames(EmbeddedResourceType::Windows, [&](String name)
			{
				return OnSearchPackage(func, FileItem(std::move(name)));
			});
			library.EnumResourceNames(EmbeddedResourceType::Qt, [&](String name)
			{
				return OnSearchPackage(func, FileItem(std::move(name)));
			});

			return func.GetCount();
		}
		return 0;
	}
}
