#pragma once
#include "../Common.h"
#include "../ILocalizationPackage.h"

namespace kxf
{
	class XMLDocument;
}

namespace kxf::Localization::Private
{
	class KX_API ItemsPackageHelper final
	{
		public:
			using TItems = std::unordered_map<ResourceID, LocalizationItem>;

		private:
			const TItems* m_Items = nullptr;

		public:
			ItemsPackageHelper(const TItems& items)
				:m_Items(&items)
			{
			}
			ItemsPackageHelper(const ItemsPackageHelper&) = delete;
			ItemsPackageHelper(ItemsPackageHelper&&) noexcept = default;

		public:
			const LocalizationItem* GetItem(const ResourceID& id) const;
			size_t EnumItems(CallbackFunction<const ResourceID&, const LocalizationItem&> func) const;

		public:
			ItemsPackageHelper& operator=(const ItemsPackageHelper&) = delete;
			ItemsPackageHelper& operator=(ItemsPackageHelper&& other) noexcept = default;
	};

	class KX_API XMLPackageHelper
	{
		protected:
			virtual bool DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme) = 0;
			virtual void DoSetLocale(const Locale& locale) = 0;

		public:
			bool Load(IInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace);
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace);
	};
}
