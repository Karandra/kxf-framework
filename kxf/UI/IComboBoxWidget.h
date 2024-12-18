#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class KXF_API IComboBoxWidget: public RTTI::ExtendInterface<IComboBoxWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(IComboBoxWidget, {0xea5eeb92, 0x2720, 0x4b56, {0x93, 0xb2, 0xa6, 0x81, 0x44, 0x69, 0xeb, 0x5}});

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		public:
			virtual void ShowDropdown() = 0;
			virtual void DismissDropdown() = 0;

			virtual size_t InsertItem(size_t index, const String& label, void* data = nullptr) = 0;
			size_t AddItem(const String& label, void* data = nullptr)
			{
				return InsertItem(npos, label, data);
			}
			virtual void RemoveItem(size_t index) = 0;
			virtual void ClearItems() = 0;

			virtual String GetItemLabel(size_t index) const = 0;
			virtual void SetItemLabel(size_t index, const String& label) = 0;
			
			virtual void* GetItemData(size_t index) const = 0;
			virtual void SetItemData(size_t index, void* data) = 0;

			virtual size_t GetItemCount() const = 0;
			virtual void SetVisibleItemCount(size_t count) = 0;

			virtual size_t GetSelectedItem() const = 0;
			virtual void SetSelectedItem(size_t index) = 0;
	};
}
