#pragma once
#include "Common.h"
#include "IWidget.h"
#include "IToolBarWidgetItem.h"

namespace kxf
{
	class KXF_API IToolBarWidget: public RTTI::ExtendInterface<IToolBarWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(IToolBarWidget, {0x4c4c6a53, 0x370a, 0x49bd, {0xba, 0x82, 0xe7, 0xf3, 0xa0, 0x9e, 0x60, 0x5b}});

		public:
			using ItemType = IToolBarWidgetItem::ItemType;
			static constexpr auto npos = IToolBarWidgetItem::npos;

		public:
			virtual std::shared_ptr<IToolBarWidgetItem> InsertItem(const String& label, ItemType type = ItemType::Regular, WidgetID id = {}, size_t index = npos) = 0;
			virtual std::shared_ptr<IToolBarWidgetItem> InsertWidgetItem(IWidget& widget, size_t index = npos) = 0;
			virtual void RemoveItem(size_t index) = 0;

			virtual size_t GetItemCount() const = 0;
			virtual Enumerator<std::shared_ptr<IToolBarWidgetItem>> EnumItems() = 0;

			virtual std::shared_ptr<IToolBarWidgetItem> GetItemByID(const WidgetID& id) = 0;
			virtual std::shared_ptr<IToolBarWidgetItem> GetItemByIndex(size_t index) = 0;
			virtual std::shared_ptr<IToolBarWidgetItem> GetItemByPosition(const Point& point) = 0;

		public:
			std::shared_ptr<IToolBarWidgetItem> InsertTextItem(const String& label, WidgetID id = {}, size_t index = npos)
			{
				return InsertItem(label, ItemType::Regular, id, index);
			}
			std::shared_ptr<IToolBarWidgetItem> InsertCheckItem(const String& label, WidgetID id = {}, size_t index = npos)
			{
				return InsertItem(label, ItemType::CheckItem, id, index);
			}
			std::shared_ptr<IToolBarWidgetItem> InsertRadioItem(const String& label, WidgetID id = {}, size_t index = npos)
			{
				return InsertItem(label, ItemType::RadioItem, id, index);
			}
			std::shared_ptr<IToolBarWidgetItem> InsertSeparator(size_t index = npos)
			{
				return InsertItem({}, ItemType::Separator, {}, index);
			}

			std::shared_ptr<IToolBarWidgetItem> InsertSpacer(int size, size_t index = npos)
			{
				if (auto item = InsertItem({}, ItemType::Spacer, {}, index))
				{
					item->SetSpacerSize(size);
					return item;
				}
				return nullptr;
			}
			std::shared_ptr<IToolBarWidgetItem> InsertStretchSpacer(int proportion, size_t index = npos)
			{
				if (auto item = InsertSpacer(-1, index))
				{
					item->SetProportion(proportion);
					return item;
				}
				return nullptr;
			}
	};
}
