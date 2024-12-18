#include "kxf-pch.h"
#include "StaticVariableCollection.h"

namespace kxf
{
	// IVariableCollection
	size_t StaticVariableCollection::DoClearItems(const String& ns)
	{
		if (ns.IsEmpty())
		{
			const size_t count = m_StaticItems.size();
			m_StaticItems.clear();

			return count;
		}
		else
		{
			size_t count = 0;
			for (auto it = m_StaticItems.begin(); it != m_StaticItems.end(); ++it)
			{
				if (it->first.Namespace == ns)
				{
					count++;
					m_StaticItems.erase(it);
				}
			}
			return count;
		}
	}
	size_t StaticVariableCollection::DoGetItemCount(const String& ns) const
	{
		if (ns.IsEmpty())
		{
			return m_StaticItems.size();
		}
		else
		{
			size_t count = 0;
			for (auto&& [descriptor, value]: m_StaticItems)
			{
				if (descriptor.Namespace == ns)
				{
					count++;
				}
			}
			return count;
		}
	}
	size_t StaticVariableCollection::DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const
	{
		size_t count = 0;
		for (auto&& [descriptor, value]: m_StaticItems)
		{
			count++;
			if (!std::invoke(func, descriptor.Namespace, descriptor.Value, value))
			{
				break;
			}
		}
		return count;
	}
}
