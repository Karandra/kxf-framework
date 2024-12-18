#include "kxf-pch.h"
#include "IMenuWidgetItem.h"
#include "IMenuWidget.h"

namespace kxf
{
	std::shared_ptr<IMenuWidget> IMenuWidgetItem::GetOwningMenu() const
	{
		if (auto widget = GetOwningWidget())
		{
			return widget->QueryInterface<IMenuWidget>();
		}
		return nullptr;
	}
}
