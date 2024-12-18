#pragma once
#include "Common.h"
#include "ILocalizationPackage.h"
#include "Private/LocalizationPackageHelper.h"

namespace kxf
{
	class XMLDocument;
}

namespace kxf
{
	class KXF_API WindowsLocalizationPackage:
		public RTTI::DynamicImplementation<WindowsLocalizationPackage, ILocalizationPackage>,
		private Localization::Private::XMLPackageHelper
	{
		private:
			std::unordered_map<ResourceID, LocalizationItem> m_Items;
			Locale m_Locale;

		private:
			bool DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme);
			void DoSetLocale(const Locale& locale) override
			{
				m_Locale = locale;
			}

		public:
			WindowsLocalizationPackage() = default;
			WindowsLocalizationPackage(const WindowsLocalizationPackage&) = delete;
			WindowsLocalizationPackage(WindowsLocalizationPackage&&) noexcept = default;

		public:
			// ILocalizationPackage
			Locale GetLocale() const noexcept override
			{
				return m_Locale;
			}
			size_t GetItemCount() const override
			{
				return m_Items.size();
			}

			const LocalizationItem& GetItem(const ResourceID& id) const override
			{
				Localization::Private::ItemsPackageHelper helper(m_Items);

				if (const LocalizationItem* item = helper.GetItem(id))
				{
					return *item;
				}
				return NullLocalizationItem;
			}
			CallbackResult<void> EnumItems(CallbackFunction<const ResourceID&, const LocalizationItem&> func) const override
			{
				Localization::Private::ItemsPackageHelper helper(m_Items);
				return helper.EnumItems(std::move(func));
			}

			bool Load(IInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return XMLPackageHelper::Load(stream, locale, loadingScheme);
			}
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return XMLPackageHelper::Load(library, name, locale, loadingScheme);
			}
			std::vector<String> GetFileExtensions() const override;

		public:
			WindowsLocalizationPackage& operator=(const WindowsLocalizationPackage&) = delete;
			WindowsLocalizationPackage& operator=(WindowsLocalizationPackage&&) noexcept = default;
	};
}
