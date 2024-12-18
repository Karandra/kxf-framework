#include "kxf-pch.h"
#include "WidgetEvent.h"
#include "../IWidget.h"

namespace kxf
{
	// WidgetEvent
	WidgetEvent::WidgetEvent(IWidget& widget) noexcept
		:m_Widget(widget.QueryInterface<IWidget>())
	{
	}

	// IWidgetEvent
	WidgetID WidgetEvent::GetWidgetID() const noexcept
	{
		if (m_Widget && m_WidgetID.IsNone())
		{
			return m_Widget->GetWidgetID();
		}
		return m_WidgetID;
	}
}
