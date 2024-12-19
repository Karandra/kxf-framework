#include "kxf-pch.h"
#include "ToolBar.h"
#include "ToolBarItem.h"
#include "WXUI/ToolBar.h"
#include "kxf/wxWidgets/MapCore.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIBitmap.h"
#include <wx/aui/auibar.h>

namespace kxf::Widgets
{
	// ToolBarWidgetItem
	void ToolBarItem::OnWXItemDestroyed()
	{
		m_Item = nullptr;
		m_ItemID = {};
		m_OwningToolBar = {};
	}
	void ToolBarItem::ScheduleRefreshItem()
	{
		if (auto toolBar = m_OwningToolBar.lock())
		{
			return toolBar->Get()->ScheduleRefreshRect(GetRect());
		}
	}

	ToolBarItem::ToolBarItem()
	{
	}
	ToolBarItem::ToolBarItem(ToolBar& toolBar, wxAuiToolBarItem& item)
		:m_Item(&item), m_OwningToolBar(toolBar.QueryInterface<ToolBar>())
	{
	}
	ToolBarItem::~ToolBarItem()
	{
	}

	// IWidgetItem
	std::shared_ptr<IWidget> ToolBarItem::GetOwningWidget() const
	{
		return m_OwningToolBar.lock();
	}

	String ToolBarItem::GetLabel(FlagSet<WidgetTextFlag> flags) const
	{
		if (m_Item)
		{
			if (flags.Contains(WidgetTextFlag::WithMnemonics))
			{
				return m_Item->GetLabel();
			}
			else
			{
				return wxControl::RemoveMnemonics(m_Item->GetLabel());
			}
		}
		return {};
	}
	void ToolBarItem::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
	{
		if (m_Item)
		{
			ScheduleRefreshItem();
			if (flags.Contains(WidgetTextFlag::WithMnemonics))
			{
				return m_Item->SetLabel(wxControl::RemoveMnemonics(label));
			}
			else
			{
				return m_Item->SetLabel(label);
			}
		}
	}

	String ToolBarItem::GetDescription() const
	{
		if (m_Item)
		{
			return m_Item->GetLongHelp();
		}
		return {};
	}
	void ToolBarItem::SetDescription(const String& description)
	{
		if (m_Item)
		{
			m_Item->SetLongHelp(description);
		}
	}

	WidgetID ToolBarItem::GetID() const
	{
		if (m_Item)
		{
			return m_ItemID;
		}
		return {};
	}
	void ToolBarItem::SetID(WidgetID id)
	{
		m_ItemID = std::move(id);
	}

	BitmapImage ToolBarItem::GetIcon() const
	{
		if (m_Item)
		{
			return m_Item->GetBitmap();
		}
		return {};
	}
	void ToolBarItem::SetIcon(const BitmapImage& icon)
	{
		if (m_Item)
		{
			m_Item->SetBitmap(icon.ToWXBitmap());
			ScheduleRefreshItem();
		}
	}

	bool ToolBarItem::IsEnabled() const
	{
		if (m_Item)
		{
			if (auto toolBar = m_OwningToolBar.lock())
			{
				return toolBar->Get()->GetToolEnabled(m_Item->GetId());
			}
		}
		return false;
	}
	void ToolBarItem::SetEnabled(bool enabled)
	{
		if (m_Item)
		{
			if (auto toolBar = m_OwningToolBar.lock())
			{
				ScheduleRefreshItem();
				return toolBar->Get()->EnableTool(m_Item->GetId(), enabled);
			}
		}
	}

	bool ToolBarItem::IsVisible() const
	{
		if (m_Item)
		{
			return m_Item->GetSizerItem()->IsShown();
		}
		return false;
	}
	void ToolBarItem::SetVisible(bool visible)
	{
		if (m_Item)
		{
			m_Item->GetSizerItem()->Show(visible);
			ScheduleRefreshItem();
		}
	}

	Point ToolBarItem::GetPosition() const
	{
		return ToolBarItem::GetRect().GetPosition();
	}
	void ToolBarItem::SetPosition(const Point& pos)
	{
	}

	Rect ToolBarItem::GetRect(WidgetSizeFlag sizeType) const
	{
		if (m_Item)
		{
			if (auto toolBar = m_OwningToolBar.lock())
			{
				auto rect = Rect(toolBar->Get()->GetToolRect(m_Item->GetId()));

				switch (sizeType)
				{
					case WidgetSizeFlag::Border:
					{
						int value = m_Item->GetSpacerPixels();
						if (toolBar->Get()->IsVertical())
						{
							rect.SetWidth(Geometry::DefaultCoord);
							rect.SetHeight(value);
						}
						else
						{
							rect.SetWidth(value);
							rect.SetHeight(Geometry::DefaultCoord);
						}
						return rect;
					}
					case WidgetSizeFlag::Client:
					case WidgetSizeFlag::Widget:
					{
						return rect;
					}
					case WidgetSizeFlag::ClientMin:
					case WidgetSizeFlag::WidgetMin:
					{
						return {rect.GetPosition(), Size(m_Item->GetMinSize())};
					}
				};
			}
		}
		return Rect::UnspecifiedRect();
	}
	Size ToolBarItem::GetSize(WidgetSizeFlag sizeType) const
	{
		return ToolBarItem::GetRect(sizeType).GetSize();
	}
	void ToolBarItem::SetSize(const Size& size, FlagSet<WidgetSizeFlag> sizeType)
	{
		if (m_Item)
		{
			if (sizeType.Contains(WidgetSizeFlag::Border))
			{
				if (auto toolBar = m_OwningToolBar.lock())
				{
					m_Item->SetSpacerPixels(toolBar->Get()->IsVertical() ? size.GetHeight() : size.GetWidth());
					ScheduleRefreshItem();
				}
			}
		}
	}

	// IToolBarWidgetItem
	String ToolBarItem::GetTooltip() const
	{
		if (m_Item)
		{
			return m_Item->GetShortHelp();
		}
		return {};
	}
	void ToolBarItem::SetTooltip(const String& tooltip)
	{
		if (m_Item)
		{
			m_Item->SetShortHelp(tooltip);
		}
	}

	bool ToolBarItem::IsChecked() const
	{
		if (m_Item)
		{
			if (auto toolBar = m_OwningToolBar.lock())
			{
				return toolBar->Get()->GetToolToggled(m_Item->GetId());
			}
		}
		return false;
	}
	void ToolBarItem::SetChecked(bool checked)
	{
		if (m_Item)
		{
			if (auto toolBar = m_OwningToolBar.lock())
			{
				toolBar->Get()->ToggleTool(m_Item->GetId(), checked);
				ScheduleRefreshItem();
			}
		}
	}

	int ToolBarItem::GetProportion() const
	{
		if (m_Item)
		{
			return m_Item->GetProportion();
		}
		return 0;
	}
	void ToolBarItem::SetProportion(int proportion)
	{
		if (m_Item)
		{
			m_Item->SetProportion(proportion);
			ScheduleRefreshItem();
		}
	}

	int ToolBarItem::GetSpacerSize() const
	{
		if (m_Item)
		{
			return m_Item->GetSpacerPixels();
		}
		return 0;
	}
	void ToolBarItem::SetSpacerSize(int size)
	{
		if (m_Item && size >= 0)
		{
			m_Item->SetSpacerPixels(size);
			ScheduleRefreshItem();
		}
	}

	FlagSet<Alignment> ToolBarItem::GetAlignment() const
	{
		if (m_Item)
		{
			return wxWidgets::MapAlignment(static_cast<wxAlignment>(m_Item->GetAlignment()));
		}
		return {};
	}
	void ToolBarItem::SetAlignment(FlagSet<Alignment> alignment)
	{
		if (m_Item)
		{
			m_Item->SetAlignment(*wxWidgets::MapAlignment(alignment));
			ScheduleRefreshItem();
		}
	}

	std::shared_ptr<IWidget> ToolBarItem::GetHostedWidget() const
	{
		if (m_Item)
		{
			if (auto window = m_Item->GetWindow())
			{
				return kxf::Private::FindByWXObject(*window);
			}
		}
		return nullptr;
	}
	size_t ToolBarItem::GetIndex() const
	{
		if (m_Item)
		{
			if (auto toolBar = m_OwningToolBar.lock())
			{
				return toolBar->Get()->DoGetItemIndex(*m_Item);
			}
		}
		return npos;
	}
	IToolBarWidgetItem::ItemType ToolBarItem::GetItemType() const
	{
		if (m_Item)
		{
			if (m_Item->GetWindow())
			{
				return ItemType::Widget;
			}

			switch (m_Item->GetKind())
			{
				case wxItemKind::wxITEM_NORMAL:
				{
					if (m_Item->GetSpacerPixels() > 0 && m_Item->GetLabel().IsEmpty())
					{
						return ItemType::Spacer;
					}
					else
					{
						return ItemType::Regular;
					}
				}
				case wxItemKind::wxITEM_DROPDOWN:
				{
					return ItemType::Dropdown;
				}
				case wxItemKind::wxITEM_CHECK:
				{
					return ItemType::CheckItem;
				}
				case wxItemKind::wxITEM_RADIO:
				{
					return ItemType::RadioItem;
				}
				case wxItemKind::wxITEM_SEPARATOR:
				{
					return ItemType::Separator;
				}
			};
		}
		return ItemType::None;
	}
}
