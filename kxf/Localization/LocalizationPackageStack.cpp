#include "KxfPCH.h"
#include "LocalizationPackageStack.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/Utility/Enumerator.h"

namespace kxf
{
	size_t LocalizationPackageStack::GetItemCount() const
	{
		size_t count = 0;
		EnumLocalizationPackages([&](const ILocalizationPackage& package)
		{
			count += package.GetItemCount();
		});

		return count;
	}
	size_t LocalizationPackageStack::EnumItems(CallbackFunction<const ResourceID&, const LocalizationItem&> func) const
	{
		if (!m_Packages.empty())
		{
			func.Reset();
			for (auto& package: m_Packages)
			{
				package->EnumItems([&](const auto& id, const auto& item)
				{
					return func.Invoke(id, item).GetLastCommand();
				});

				if (func.ShouldTerminate())
				{
					break;
				}
			}

			return func.GetCount();
		}
		return 0;
	}
	const LocalizationItem& LocalizationPackageStack::GetItem(const ResourceID& id) const
	{
		const LocalizationItem* ptr = nullptr;
		EnumLocalizationPackages([&](const ILocalizationPackage& package)
		{
			if (const LocalizationItem& item = package.GetItem(id))
			{
				ptr = &item;
				return CallbackCommand::Terminate;
			}
			return CallbackCommand::Continue;
		});

		return ptr ? *ptr : NullLocalizationItem;
	}

	size_t LocalizationPackageStack::EnumLocalizationPackages(CallbackFunction<ILocalizationPackage&> func)
	{
		func.Reset();
		for (auto& package: m_Packages)
		{
			if (func.Invoke(*package).ShouldTerminate())
			{
				break;
			}
		}

		return func.GetCount();
	}
	size_t LocalizationPackageStack::EnumLocalizationPackages(CallbackFunction<const ILocalizationPackage&> func) const
	{
		func.Reset();
		for (auto& package: m_Packages)
		{
			if (func.Invoke(*package).ShouldTerminate())
			{
				break;
			}
		}

		return func.GetCount();
	}
}
