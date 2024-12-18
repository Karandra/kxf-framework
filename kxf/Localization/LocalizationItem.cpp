#include "kxf-pch.h"
#include "LocalizationItem.h"
#include "ILocalizationPackage.h"
#include "Locale.h"

namespace
{
	using namespace kxf;

	const String& PluralStringEnglish(const LocalizationItem& item, int value)
	{
		if (value == 1)
		{
			return item.GetPluralString(LocalizationItemQuantity::One);
		}
		else if (value <= 4)
		{
			return item.GetPluralString(LocalizationItemQuantity::Few);
		}
		else if (value > 4)
		{
			return item.GetPluralString(LocalizationItemQuantity::Many);
		}
		return item.GetPluralString(LocalizationItemQuantity::Other);
	};
}

namespace kxf
{
	bool LocalizationItem::IsNull() const noexcept
	{
		if (this == &NullLocalizationItem)
		{
			return true;
		}
		else if (m_Package == nullptr || m_Value.valueless_by_exception() || m_Value.index() == 0)
		{
			return true;
		}
		return GetItemCount() == 0;
	}
	size_t LocalizationItem::GetItemCount() const noexcept
	{
		if (auto value = std::get_if<TSingleItem>(&m_Value))
		{
			return !value->IsEmpty() ? 1 : 0;
		}
		else if (auto value = std::get_if<TMultipleItems>(&m_Value))
		{
			return value->size();
		}
		else if (auto value = std::get_if<TPlurals>(&m_Value))
		{
			return value->size();
		}
		return 0;
	}

	const String& LocalizationItem::GetString(size_t index) const& noexcept
	{
		if (auto signleItem = std::get_if<TSingleItem>(&m_Value))
		{
			return *signleItem;
		}
		else if (auto items = std::get_if<TMultipleItems>(&m_Value); items && index < items->size())
		{
			return (*items)[index];
		}
		return NullString;
	}
	String LocalizationItem::GetString(size_t index) && noexcept
	{
		if (auto signleItem = std::get_if<TSingleItem>(&m_Value))
		{
			return std::move(*signleItem);
		}
		else if (auto items = std::get_if<TMultipleItems>(&m_Value); items && index < items->size())
		{
			return std::move((*items)[index]);
		}
		return {};
	}

	const String& LocalizationItem::GetPluralString(LocalizationItemQuantity quantity) const noexcept
	{
		if (auto items = std::get_if<TPlurals>(&m_Value))
		{
			if (auto it = items->find(quantity); it != items->end())
			{
				return it->second;
			}
		}
		return NullString;
	}
	const String& LocalizationItem::GetPluralString(int quantity) const noexcept
	{
		if (m_Package)
		{
			const String& value = m_Package->GetPluralStringForNumber(*this, quantity);
			if (!value.IsEmpty())
			{
				return value;
			}
		}
		
		// Use the default algorithm (for English)
		return PluralStringEnglish(*this, quantity);
	}
}

namespace kxf
{
	const LocalizationItem NullLocalizationItem;
}
