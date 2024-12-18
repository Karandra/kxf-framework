#pragma once
#include "Common.h"
#include "IVariableCollection.h"
#include "kxf/Utility/Container.h"

namespace kxf
{
	class KXF_API CombinedVariableCollection final: public RTTI::Implementation<CombinedVariableCollection, IVariableCollection>
	{
		private:
			std::vector<IVariableCollection*> m_Items;
			IVariableCollection* m_EditableCollection = nullptr;

		protected:
			// IVariableCollection
			size_t DoClearItems(const String& ns) override;
			size_t DoGetItemCount(const String& ns) const override;
			size_t DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const override;

			bool DoHasItem(const String& ns, const String& id) const override;
			Any DoGetItem(const String& ns, const String& id) const override;
			void DoSetItem(const String& ns, const String& id, Any item) override;

		public:
			CombinedVariableCollection(IVariableCollection& editableCollection, size_t initialCount = 1)
				:m_EditableCollection(&editableCollection)
			{
				m_Items.reserve(std::max<size_t>(initialCount, 1));
				m_Items.emplace_back(&editableCollection);
			}
			CombinedVariableCollection(const CombinedVariableCollection&) = default;
			CombinedVariableCollection(CombinedVariableCollection&&) = delete;

		public:
			void AddCollection(IVariableCollection& collection)
			{
				m_Items.emplace_back(&collection);
			}
			void RemoveCollection(const IVariableCollection& collection)
			{
				Utility::Container::RemoveEachIf(m_Items, [&](IVariableCollection* item)
				{
					return item == &collection;
				});
			}
			IVariableCollection& GetEditableCollection()
			{
				return *m_EditableCollection;
			}

		public:
			CombinedVariableCollection& operator=(const CombinedVariableCollection&) = default;
			CombinedVariableCollection& operator=(CombinedVariableCollection&&) = delete;
	};
}
