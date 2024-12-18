#pragma once
#include "Common.h"
#include "ILocalizationPackage.h"
#include "kxf/Utility/Container.h"

namespace kxf
{
	class KXF_API LocalizationPackageStack: public RTTI::Implementation<LocalizationPackageStack, ILocalizationPackage>
	{
		private:
			std::vector<std::shared_ptr<ILocalizationPackage>> m_Packages;

		private:
			Locale DoGetLocale() const
			{
				if (!m_Packages.empty())
				{
					return m_Packages.front()->GetLocale();
				}
				return {};
			}

		public:
			LocalizationPackageStack() = default;
			LocalizationPackageStack(std::shared_ptr<ILocalizationPackage> localizationPackage)
			{
				Add(std::move(localizationPackage));
			}
			LocalizationPackageStack(const LocalizationPackageStack&) = delete;
			LocalizationPackageStack(LocalizationPackageStack&&) noexcept = default;

		public:
			// ILocalizationPackage
			Locale GetLocale() const override
			{
				return DoGetLocale();
			}
			size_t GetItemCount() const override;
			CallbackResult<void> EnumItems(CallbackFunction<const ResourceID&, const LocalizationItem&> func) const override;
			const LocalizationItem& GetItem(const ResourceID& id) const override;

			bool Load(IInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return false;
			}
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return false;
			}
			std::vector<String> GetFileExtensions() const override
			{
				return {};
			}

			// LocalizationPackageStack
			ILocalizationPackage& Add(std::shared_ptr<ILocalizationPackage> localizationPackage)
			{
				return *m_Packages.emplace_back(std::move(localizationPackage));
			}
			bool Remove(const ILocalizationPackage& localizationPackage)
			{
				return Utility::Container::RemoveSingleIf(m_Packages, [&](const auto& item)
				{
					return item.get() == &localizationPackage;
				}) == m_Packages.end();
			}
			std::shared_ptr<ILocalizationPackage> Detach(const ILocalizationPackage& localizationPackage)
			{
				auto it = Utility::Container::FindIf(m_Packages, [&](const auto& item)
				{
					return item.get() == &localizationPackage;
				});

				if (it != m_Packages.end())
				{
					auto item = std::move(*it);
					m_Packages.erase(it);
					return item;
				}
				return {};
			}

			CallbackResult<void> EnumLocalizationPackages(CallbackFunction<const ILocalizationPackage&> func) const;
			CallbackResult<void> EnumLocalizationPackages(CallbackFunction<ILocalizationPackage&> func);

		public:
			explicit operator bool() const
			{
				return !m_Packages.empty() && !IsEmpty();
			}
			bool operator!() const
			{
				return m_Packages.empty() || IsEmpty();
			}

			LocalizationPackageStack& operator=(const LocalizationPackageStack&) = delete;
			LocalizationPackageStack& operator=(LocalizationPackageStack&&) noexcept = default;
	};
}
