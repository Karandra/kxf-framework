#include "kxf-pch.h"
#include "IWidget.h"
#include "Private/Common.h"

namespace kxf
{
	std::shared_ptr<IWidget> IWidget::FindFocus() noexcept
	{
		if (auto window = wxWindow::FindFocus())
		{
			return Private::FindByWXObject(*window);
		}
		return nullptr;
	}
	void IWidget::InheritVisualAttributes(const IWidget& parent)
	{
		SetFont(parent.GetFont());
		SetFocusVisible(parent.IsFocusVisible());
		SetLayoutDirection(parent.GetLayoutDirection());

		SetTransparency(parent.GetTransparency());
		SetColor(parent.GetColor(WidgetColorFlag::Background), WidgetColorFlag::Background);
		SetColor(parent.GetColor(WidgetColorFlag::Foreground), WidgetColorFlag::Foreground);
		SetColor(parent.GetColor(WidgetColorFlag::Border), WidgetColorFlag::Border);
		SetColor(parent.GetColor(WidgetColorFlag::Text), WidgetColorFlag::Text);
	}
}
