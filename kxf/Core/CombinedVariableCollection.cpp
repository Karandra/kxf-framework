#include "kxf-pch.h"
#include "CombinedVariableCollection.h"

namespace kxf
{
	// IVariableCollection
	size_t CombinedVariableCollection::DoClearItems(const String& ns)
	{
		return m_EditableCollection->ClearItems(ns);
	}
	size_t CombinedVariableCollection::DoGetItemCount(const String& ns) const
	{
		size_t count = 0;
		for (IVariableCollection* collection: m_Items)
		{
			count += collection->GetItemCount(ns);
		}
		return count;
	}
	size_t CombinedVariableCollection::DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const
	{
		size_t count = 0;
		bool canContinue = true;

		for (IVariableCollection* collection: m_Items)
		{
			count += collection->EnumItems([&](const String& ns, const String& id, Any value)
			{
				canContinue = std::invoke(func, ns, id, std::move(value));
				return canContinue;
			});

			if (!canContinue)
			{
				break;
			}
		}
		return count;
	}

	bool CombinedVariableCollection::DoHasItem(const String& ns, const String& id) const
	{
		for (IVariableCollection* collection: m_Items)
		{
			if (collection->HasItem(ns, id))
			{
				return true;
			}
		}
		return false;
	}
	Any CombinedVariableCollection::DoGetItem(const String& ns, const String& id) const
	{
		for (IVariableCollection* collection: m_Items)
		{
			if (auto value = collection->GetItem(ns, id))
			{
				return value;
			}
		}
		return {};
	}
	void CombinedVariableCollection::DoSetItem(const String& ns, const String& id, Any item)
	{
		m_EditableCollection->SetItem(ns, id, std::move(item));
	}
}
