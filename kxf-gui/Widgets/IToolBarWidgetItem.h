#pragma once
#include "Common.h"
#include "IWidgetItem.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/EventSystem/IEvent.h"

namespace kxf
{
	class IToolBarWidget;
}

namespace kxf
{
	class KXF_API IToolBarWidgetItem: public RTTI::ExtendInterface<IToolBarWidgetItem, IWidgetItem>
	{
		kxf_RTTI_DeclareIID(IMenuWidgetItem, {0x362b17c3, 0x685, 0x4af2, {0x81, 0x3a, 0x15, 0x58, 0xef, 0x62, 0x53, 0x5d}});

		public:
			enum class ItemType
			{
				None = -1,

				Widget,
				Regular,
				Dropdown,
				CheckItem,
				RadioItem,
				Separator,
				Spacer
			};
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		public:
			std::shared_ptr<IToolBarWidget> GetOwningToolBar() const;

			virtual String GetTooltip() const = 0;
			virtual void SetTooltip(const String& tooltip) = 0;

			virtual bool IsChecked() const = 0;
			virtual void SetChecked(bool checked = true) = 0;

			virtual int GetProportion() const = 0;
			virtual void SetProportion(int proportion) = 0;

			virtual int GetSpacerSize() const = 0;
			virtual void SetSpacerSize(int size) = 0;

			virtual FlagSet<Alignment> GetAlignment() const = 0;
			virtual void SetAlignment(FlagSet<Alignment> alignment) = 0;

			virtual std::shared_ptr<IWidget> GetHostedWidget() const = 0;
			virtual size_t GetIndex() const = 0;
			virtual ItemType GetItemType() const = 0;
			bool IsWidgetItem() const
			{
				return GetItemType() == ItemType::Widget;
			}
			bool IsSpacerItem() const
			{
				return GetItemType() == ItemType::Spacer;
			}
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
			bool IsDropdownItem() const
			{
				return GetItemType() == ItemType::Dropdown;
			}
	};
}
