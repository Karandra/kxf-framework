#include "KxfPCH.h"
#include "Common.h"
#include "Locale.h"
#include "ILocalizationPackage.h"
#include "kxf/UI/WidgetID.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/wxWidgets/StandardLocalization.h"
#include "Private/LocalizationResources.h"

namespace
{
	template<class TFunc>
	kxf::CallbackCommand OnSearchPackage(TFunc&& func, kxf::FileItem item)
	{
		using namespace kxf;

		// Extract locale name from names like 'en-US.Application.xml'
		if (Locale locale = Localization::Private::LocaleFromFileName(item.GetName()))
		{
			String name = item.GetName().BeforeLast('.');
			if (!name.IsEmpty())
			{
				return std::invoke(func, std::move(locale), std::move(item));
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

	size_t SearchPackages(const IFileSystem& fileSystem, const FSPath& directory, std::function<CallbackCommand(Locale, FileItem)> func)
	{
		std::unordered_set<String> extensions;
		for (auto&& classInfo: RTTI::GetClassInfo<ILocalizationPackage>().EnumDerivedClasses())
		{
			if (auto instance = classInfo.CreateObjectInstance<ILocalizationPackage>())
			{
				for (auto&& ext: instance->EnumFileExtensions())
				{
					extensions.insert(ext.ToLower());
				}
			}
		}

		size_t count = 0;
		for (FileItem item: fileSystem.EnumItems(directory, {}, FSActionFlag::LimitToFiles))
		{
			if (extensions.find(item.GetFileExtension().MakeLower()) != extensions.end())
			{
				count++;
				if (OnSearchPackage(func, std::move(item)) == CallbackCommand::Terminate)
				{
					break;
				}
			}
		}
		return count;
	}
	size_t SearchPackages(const DynamicLibrary& library, std::function<CallbackCommand(Locale, FileItem)> func)
	{
		if (library)
		{
			using namespace Localization::Private;

			size_t count = 0;
			count += library.EnumResourceNames(EmbeddedResourceType::Android, [&](String name)
			{
				return OnSearchPackage(func, FileItem(std::move(name)));
			});
			count += library.EnumResourceNames(EmbeddedResourceType::Windows, [&](String name)
			{
				return OnSearchPackage(func, FileItem(std::move(name)));
			});
			count += library.EnumResourceNames(EmbeddedResourceType::Qt, [&](String name)
			{
				return OnSearchPackage(func, FileItem(std::move(name)));
			});

			return count;
		}
		return 0;
	}
}
