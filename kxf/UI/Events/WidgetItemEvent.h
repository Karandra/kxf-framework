#pragma once
#include "WidgetEvent.h"
#include "WidgetMouseEvent.h"
#include "../IWidgetItem.h"

namespace kxf
{
	class KXF_API WidgetItemEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(WidgetItemEvent, Selected);
			kxf_EVENT_MEMBER(WidgetItemEvent, Activated);
			kxf_EVENT_MEMBER(WidgetItemEvent, ContextMenu);

			kxf_EVENT_MEMBER(WidgetItemEvent, Enter);
			kxf_EVENT_MEMBER(WidgetItemEvent, Leave);

		protected:
			WidgetMouseEvent m_MouseEvent;
			std::shared_ptr<IWidgetItem> m_Item;
			WidgetID m_ItemID;

		public:
			WidgetItemEvent(IWidget& owningWidget, WidgetID id = {}, WidgetMouseEvent mouseEvent = {}) noexcept
				:WidgetEvent(owningWidget), m_MouseEvent(std::move(mouseEvent)), m_ItemID(std::move(id))
			{
			}
			WidgetItemEvent(IWidget& owningWidget, IWidgetItem& widgetItem, WidgetMouseEvent mouseEvent = {}) noexcept
				:WidgetEvent(owningWidget), m_MouseEvent(std::move(mouseEvent)), m_Item(widgetItem.QueryInterface<IWidgetItem>())
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetItemEvent>(std::move(*this));
			}

			// WidgetItemEvent
			std::shared_ptr<IWidgetItem> GetWidgetItem() const noexcept
			{
				return m_Item;
			}
			const WidgetMouseEvent& GetMouseEvent() const noexcept
			{
				return m_MouseEvent;
			}
			WidgetID GetItemID() const noexcept
			{
				return m_ItemID;
			}
	};
}
