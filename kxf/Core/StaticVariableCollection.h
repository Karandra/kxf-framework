#pragma once
#include "Common.h"
#include "IVariableCollection.h"

namespace kxf
{
	class KXF_API StaticVariableCollection: public RTTI::Implementation<StaticVariableCollection, IVariableCollection>
	{
		protected:
			struct Item final
			{
				public:
					String Namespace;
					String Value;

				private:
					auto AsPair() const
					{
						return std::make_pair(std::cref(Namespace), std::cref(Value));
					}

				public:
					bool operator<(const Item& other) const
					{
						return AsPair() < other.AsPair();
					}
			};

		private:
			std::map<Item, Any> m_StaticItems;

		protected:
			// IVariableCollection
			size_t DoClearItems(const String& ns) override;
			size_t DoGetItemCount(const String& ns) const override;
			size_t DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const override;

			bool DoHasItem(const String& ns, const String& id) const override
			{
				return m_StaticItems.find({ns, id}) != m_StaticItems.end();
			}
			Any DoGetItem(const String& ns, const String& id) const override
			{
				auto it = m_StaticItems.find({ns, id});
				if (it != m_StaticItems.end())
				{
					return it->second;
				}
				return {};
			}
			void DoSetItem(const String& ns, const String& id, Any item) override
			{
				m_StaticItems.insert_or_assign({ns, id}, std::move(item));
			}

		public:
			StaticVariableCollection() = default;
	};
}
