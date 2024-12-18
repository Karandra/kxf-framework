#pragma once
#include "WidgetItemEvent.h"
#include "../IMenuWidget.h"
#include "../IMenuWidgetItem.h"

namespace kxf
{
	class KXF_API MenuWidgetEvent: public WidgetItemEvent
	{
		public:
			kxf_EVENT_MEMBER(MenuWidgetEvent, Open);
			kxf_EVENT_MEMBER(MenuWidgetEvent, Close);

			kxf_EVENT_MEMBER_AS(MenuWidgetEvent, Selected, WidgetItemEvent::EvtSelected);
			kxf_EVENT_MEMBER_AS(MenuWidgetEvent, Focused, WidgetItemEvent::EvtEnter);

		private:
			std::shared_ptr<IWidget> m_InvokingWidget;
			Point m_PopupPosition = Point::UnspecifiedPosition();

		public:
			MenuWidgetEvent(IMenuWidget& widget, std::shared_ptr<IWidget> invokingWidget = nullptr) noexcept
				:WidgetItemEvent(widget), m_InvokingWidget(std::move(invokingWidget))
			{
			}
			MenuWidgetEvent(IMenuWidget& widget, IMenuWidgetItem& menuItem, std::shared_ptr<IWidget> invokingWidget = nullptr) noexcept
				:WidgetItemEvent(widget, menuItem), m_InvokingWidget(std::move(invokingWidget))
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<MenuWidgetEvent>(std::move(*this));
			}

			// MenuWidgetEvent
			std::shared_ptr<IMenuWidget> GetMenuWidget() const noexcept
			{
				return GetWidget()->QueryInterface<IMenuWidget>();
			}
			std::shared_ptr<IMenuWidgetItem> GetMenuWidgetItem() const noexcept
			{
				return m_Item ? m_Item->QueryInterface<IMenuWidgetItem>() : nullptr;
			}
			std::shared_ptr<IWidget> GetInvokingWidget() const noexcept
			{
				return m_InvokingWidget;
			}
			
			Point GetPopupPosition() const noexcept
			{
				return m_PopupPosition;
			}
			void SetPopupPosition(const Point& position) noexcept
			{
				m_PopupPosition = position;
			}
	};
}
