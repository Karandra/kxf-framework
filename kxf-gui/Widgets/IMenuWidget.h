#pragma once
#include "Common.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "IWidget.h"
#include "IMenuWidgetItem.h"

namespace kxf
{
	class KXF_API IMenuWidget: public RTTI::ExtendInterface<IMenuWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(IMenuWidget, {0x1f082719, 0x1c5e, 0x4253, {0x87, 0xb2, 0x5a, 0x7a, 0xb, 0xb8, 0xf2, 0x6e}});

		public:
			using ItemType = IMenuWidgetItem::ItemType;
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		private:
			std::shared_ptr<IMenuWidgetItem> CreateAndInsert(ItemType type, const String& label, WidgetID id, size_t index)
			{
				if (auto item = CreateItem(label, type))
				{
					return InsertItem(*item, index);
				}
				return nullptr;
			}

		public:
			virtual ~IMenuWidget() = default;

		public:
			virtual String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const = 0;
			virtual void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) = 0;

			virtual String GetDescription() const = 0;
			virtual void SetDescription(const String& description) = 0;

			virtual std::shared_ptr<IMenuWidgetItem> InsertItem(IMenuWidgetItem& item, size_t index = npos) = 0;
			virtual std::shared_ptr<IMenuWidgetItem> InsertMenu(IMenuWidget& subMenu, const String& label = {}, WidgetID id = {}, size_t index = npos) = 0;

			virtual std::shared_ptr<IMenuWidgetItem> CreateItem(const String& label, ItemType type = ItemType::Regular, WidgetID id = {}) = 0;
			virtual std::shared_ptr<IMenuWidgetItem> GetDefaultItem() const = 0;

			virtual size_t GetMenuItemCount() const = 0;
			virtual Enumerator<std::shared_ptr<IMenuWidgetItem>> EnumMenuItems() const = 0;

			virtual void Show(Point pos = Point::UnspecifiedPosition(), FlagSet<Alignment> alignment = {}) = 0;
			virtual void ShowAt(const IWidget& widget, Point pos = Point::UnspecifiedPosition(), FlagSet<Alignment> alignment = {}) = 0;
			virtual void ShowWithOffset(const IWidget& widget, int offset = 1, FlagSet<Alignment> alignment = {}) = 0;

			virtual Point GetInvokingPosition() const = 0;
			virtual std::shared_ptr<IWidget> GetInvokingWidget() const = 0;

		public:
			std::shared_ptr<IMenuWidgetItem> InsertItem(const String& label, WidgetID id = {}, size_t index = npos)
			{
				return CreateAndInsert(ItemType::Regular, label, id, index);
			}
			std::shared_ptr<IMenuWidgetItem> InsertCheckItem(const String& label, WidgetID id = {}, size_t index = npos)
			{
				return CreateAndInsert(ItemType::CheckItem, label, id, index);
			}
			std::shared_ptr<IMenuWidgetItem> InsertRadioItem(const String& label, WidgetID id = {}, size_t index = npos)
			{
				return CreateAndInsert(ItemType::RadioItem, label, id, index);
			}
			std::shared_ptr<IMenuWidgetItem> InsertSeparator(size_t index = npos)
			{
				return CreateAndInsert(ItemType::Separator, {}, {}, index);
			}
	};
}
