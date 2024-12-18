#include "kxf-pch.h"
#include "Common.h"
#include "Locale.h"
#include "ILocalizationPackage.h"
#include "kxf/Widgets/WidgetID.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/wxWidgets/StandardLocalization.h"
#include "kxf/Utility/String.h"
#include "Private/LocalizationResources.h"

namespace
{
	kxf::CallbackCommand OnSearchPackage(kxf::CallbackFunction<kxf::Locale, kxf::FileItem, kxf::String>& func, kxf::FileItem item)
	{
		using namespace kxf;

		// Extract locale name from names like 'en-US.Application.xml'
		auto fileName = item.GetName();
		if (Locale locale = Localization::Private::LocaleFromFileName(fileName))
		{
			String moduleName = fileName.BeforeLast('.');
			if (moduleName == locale.GetName())
			{
				moduleName = {};
			}

			return func.Invoke(std::move(locale), std::move(item), std::move(moduleName)).GetLastCommand();
		}
		return CallbackCommand::Discard;
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

	CallbackResult<void> SearchPackages(const IFileSystem& fileSystem, const FSPath& directory, CallbackFunction<Locale, FileItem, String> func)
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
		return func.Finalize();
	}
	CallbackResult<void> SearchPackages(const DynamicLibrary& library, CallbackFunction<Locale, FileItem, String> func)
	{
		if (library)
		{
			using namespace Localization::Private;

			library.EnumResourceNames(EmbeddedResourceType::Android, [&](String fileName)
			{
				return OnSearchPackage(func, FileItem(std::move(fileName)));
			});
			library.EnumResourceNames(EmbeddedResourceType::Windows, [&](String fileName)
			{
				return OnSearchPackage(func, FileItem(std::move(fileName)));
			});
			library.EnumResourceNames(EmbeddedResourceType::Qt, [&](String fileName)
			{
				return OnSearchPackage(func, FileItem(std::move(fileName)));
			});

			return func.Finalize();
		}
		return {};
	}
}
