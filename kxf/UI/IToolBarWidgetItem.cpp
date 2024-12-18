#include "kxf-pch.h"
#include "IToolBarWidgetItem.h"
#include "IToolBarWidget.h"

namespace kxf
{
	std::shared_ptr<IToolBarWidget> IToolBarWidgetItem::GetOwningToolBar() const
	{
		if (auto widget = GetOwningWidget())
		{
			return widget->QueryInterface<IToolBarWidget>();
		}
		return nullptr;
	}
}
