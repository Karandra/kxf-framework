#pragma once
#include "Common.h"
#include "IWidgetItem.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/EventSystem/IEvent.h"

namespace kxf
{
	class IMenuWidget;
}

namespace kxf
{
	class KXF_API IMenuWidgetItem: public RTTI::ExtendInterface<IMenuWidgetItem, IWidgetItem>
	{
		kxf_RTTI_DeclareIID(IMenuWidgetItem, {0xb1e0d965, 0x3eec, 0x42d8, {0xa4, 0x4e, 0xd9, 0xce, 0xd7, 0xb7, 0x93, 0x3e}});

		public:
			enum class ItemType
			{
				None = -1,

				Regular,
				SubMenu,
				Separator,
				CheckItem,
				RadioItem
			};

		public:
			// General
			std::shared_ptr<IMenuWidget> GetOwningMenu() const;

			virtual std::shared_ptr<IMenuWidget> GetSubMenu() const = 0;
			virtual void SetSubMenu(IMenuWidget& subMenu) = 0;

			virtual bool IsDefaultItem() const = 0;
			virtual void SetDefaultItem() = 0;

			// Properties
			virtual ItemType GetItemType() const = 0;
			bool IsRegularItem() const
			{
				return GetItemType() == ItemType::Separator;
			}
			bool IsSeparatorItem() const
			{
				return GetItemType() == ItemType::Separator;
			}
			bool IsCheckItem() const
			{
				return GetItemType() == ItemType::CheckItem;
			}
			bool IsRadioItem() const
			{
				return GetItemType() == ItemType::RadioItem;
			}
			bool IsSubMenuItem() const
			{
				return GetItemType() == ItemType::SubMenu;
			}

			virtual bool IsChecked() const = 0;
			virtual void SetChecked(bool checked = true) = 0;
	};
}
