#pragma once
#include "Common.h"
#include "StaticVariableCollection.h"

namespace kxf
{
	class KXF_API DynamicVariableCollection: public RTTI::Implementation<DynamicVariableCollection, StaticVariableCollection>
	{
		public:
			using TValue = std::function<Any(const String& ns, const String& id)>;

		private:
			std::map<StaticVariableCollection::Item, TValue> m_DynamicItems;

		protected:
			// IVariableCollection
			size_t DoClearItems(const String& ns) override;
			size_t DoGetItemCount(const String& ns) const override;
			size_t DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const override;

			bool DoHasItem(const String& ns, const String& id) const override;
			Any DoGetItem(const String& ns, const String& id) const override;
			void DoSetItem(const String& ns, const String& id, Any item) override;

			// DynamicVariableCollection
			void DoSetDynamicItem(const String& ns, const String& id, TValue func)
			{
				m_DynamicItems.insert_or_assign({ns, id}, std::move(func));
			}

		public:
			// DynamicVariableCollection
			void SetDynamicItem(const String& id, TValue func)
			{
				DoSetDynamicItem({}, id, std::move(func));
			}
			void SetDynamicItem(const String& ns, const String& id, TValue func)
			{
				DoSetDynamicItem(ns, id, std::move(func));
			}

			void SetDynamicItem(const String& id, std::function<Any(void)> func)
			{
				DoSetDynamicItem({}, id, [func = std::move(func)](const String&, const String&)
				{
					return std::invoke(func);
				});
			}
			void SetDynamicItem(const String& ns, const String& id, std::function<Any(void)> func)
			{
				DoSetDynamicItem(ns, id, [func = std::move(func)](const String&, const String&)
				{
					return std::invoke(func);
				});
			}
	};
}
