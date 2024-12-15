#pragma once
#include "Common.h"
#include "ILocalizationPackage.h"
#include "Private/LocalizationPackageHelper.h"
#include "kxf/Core/Version.h"

namespace kxf
{
	class XMLDocument;
}

namespace kxf
{
	class KX_API QtLocalizationPackage:
		public RTTI::DynamicImplementation<QtLocalizationPackage, ILocalizationPackage>,
		private Localization::Private::XMLPackageHelper
	{
		private:
			std::unordered_map<ResourceID, LocalizationItem> m_Items;
			Locale m_Locale;
			Version m_Version;

		private:
			bool DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme);
			void DoSetLocale(const Locale& locale) override
			{
				if (locale)
				{
					m_Locale = locale;
				}
			}

		public:
			QtLocalizationPackage() = default;
			QtLocalizationPackage(const QtLocalizationPackage&) = delete;
			QtLocalizationPackage(QtLocalizationPackage&&) noexcept = default;

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
			size_t EnumItems(CallbackFunction<const ResourceID&, const LocalizationItem&> func) const override
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

			// QtLocalizationPackage
			Version GetVersion() const
			{
				return m_Version;
			}

		public:
			QtLocalizationPackage& operator=(const QtLocalizationPackage&) = delete;
			QtLocalizationPackage& operator=(QtLocalizationPackage&&) noexcept = default;
	};
}
