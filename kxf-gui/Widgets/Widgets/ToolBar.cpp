#include "kxf-pch.h"
#include "ToolBar.h"
#include "ToolBarItem.h"
#include "WXUI/ToolBar.h"
#include "kxf/Utility/Enumerator.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"

namespace kxf::Widgets
{
	// ToolBar
	ToolBar::ToolBar()
	{
		InitializeWxWidget();
	}
	ToolBar::~ToolBar() = default;

	// IWidget
	bool ToolBar::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	Color ToolBar::GetColor(WidgetColorFlag colorType) const
	{
		if (colorType == WidgetColorFlag::Border)
		{
			return Get()->GetBorderColor();
		}
		return BasicWxWidget::GetColor(colorType);
	}
	void ToolBar::SetColor(const Color& color, FlagSet<WidgetColorFlag> flags)
	{
		if (flags.Contains(WidgetColorFlag::Border))
		{
			Get()->SetBorderColor(color);
		}
		BasicWxWidget::SetColor(color, flags);
	}

	// IToolBarWidget
	std::shared_ptr<IToolBarWidgetItem> ToolBar::InsertItem(const String& label, ItemType type, WidgetID id, size_t index)
	{
		switch (type)
		{
			case ItemType::Regular:
			{
				return Get()->AddItem(index, label, wxITEM_NORMAL, id);
			}
			case ItemType::CheckItem:
			{
				return Get()->AddItem(index, label, wxITEM_CHECK, id);
			}
			case ItemType::RadioItem:
			{
				return Get()->AddItem(index, label, wxITEM_RADIO, id);
			}
			case ItemType::Dropdown:
			{
				return Get()->AddItem(index, label, wxITEM_DROPDOWN, id);
			}
			case ItemType::Separator:
			{
				return Get()->AddSeparator(index, id);
			}
			case ItemType::Spacer:
			{
				return Get()->AddSpacer(index, -1, id);
			}
		};
		return nullptr;
	}
	std::shared_ptr<IToolBarWidgetItem> ToolBar::InsertWidgetItem(IWidget& widget, size_t index)
	{
		if (auto control = dynamic_cast<wxControl*>(widget.GetWxWindow()))
		{
			return Get()->AddWidget(index, control, {});
		}
		return nullptr;
	}
	void ToolBar::RemoveItem(size_t index)
	{
		Get()->RemoveItem(index);
	}

	size_t ToolBar::GetItemCount() const
	{
		return Get()->GetToolCount();
	}
	Enumerator<std::shared_ptr<IToolBarWidgetItem>> ToolBar::EnumItems()
	{
		return Utility::EnumerateIterableContainer<std::shared_ptr<IToolBarWidgetItem>>(Get()->m_Items, {}, [](auto&& items)
		{
			return items.second;
		});
	}

	std::shared_ptr<IToolBarWidgetItem> ToolBar::GetItemByID(const WidgetID& id)
	{
		return Get()->FindToolByID(id);
	}
	std::shared_ptr<IToolBarWidgetItem> ToolBar::GetItemByIndex(size_t index)
	{
		return Get()->FindToolByIndex(index);
	}
	std::shared_ptr<IToolBarWidgetItem> ToolBar::GetItemByPosition(const Point& point)
	{
		return Get()->FindToolByPosition(point);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> ToolBar::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
