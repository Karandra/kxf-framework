#pragma once
#include "WidgetEvent.h"
#include "../IBookWidget.h"

namespace kxf
{
	class KXF_API BookWidgetEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(BookWidgetEvent, PageChange);
			kxf_EVENT_MEMBER(BookWidgetEvent, PageChanged);

			kxf_EVENT_MEMBER(BookWidgetEvent, PageClose);
			kxf_EVENT_MEMBER(BookWidgetEvent, PageClosed);

			kxf_EVENT_MEMBER(BookWidgetEvent, DragAllow);
			kxf_EVENT_MEMBER(BookWidgetEvent, DragBegin);
			kxf_EVENT_MEMBER(BookWidgetEvent, DragMove);
			kxf_EVENT_MEMBER(BookWidgetEvent, DragEnd);
			kxf_EVENT_MEMBER(BookWidgetEvent, DragDone);

			kxf_EVENT_MEMBER(BookWidgetEvent, ButtonMiddleUp);
			kxf_EVENT_MEMBER(BookWidgetEvent, ButtonMiddleDown);

			kxf_EVENT_MEMBER(BookWidgetEvent, ButtonRightUp);
			kxf_EVENT_MEMBER(BookWidgetEvent, ButtonRightDown);

			kxf_EVENT_MEMBER(BookWidgetEvent, PageMenu);
			kxf_EVENT_MEMBER(BookWidgetEvent, BackgroundDoubleClick);

		private:
			size_t m_OldSelection = IBookWidget::npos;
			size_t m_NewSelection = IBookWidget::npos;

		public:
			BookWidgetEvent(IBookWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}
			BookWidgetEvent(IBookWidget& widget, size_t oldSelection, size_t newSelection) noexcept
				:WidgetEvent(widget), m_OldSelection(oldSelection), m_NewSelection(newSelection)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<BookWidgetEvent>(std::move(*this));
			}

			// ButtonWidgetEvent
			std::shared_ptr<IBookWidget> GetBookWidget() const noexcept
			{
				return GetWidget()->QueryInterface<IBookWidget>();
			}

			size_t GetOldSelection() const noexcept
			{
				return m_OldSelection;
			}
			size_t GetNewSelection() const noexcept
			{
				return m_NewSelection;
			}
	};
}
