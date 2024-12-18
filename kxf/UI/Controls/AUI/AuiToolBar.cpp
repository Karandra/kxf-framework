#include "kxf-pch.h"
#include "AuiToolBar.h"
#include <wx/wupdlock.h>
#include <wx/dc.h>

namespace
{
	constexpr bool IsValidID(wxWindowID id) noexcept
	{
		return id != wxID_NONE && id != wxID_SEPARATOR && id != wxID_ANY;
	}
}

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(AuiToolBar, wxAuiToolBar);

	void AuiToolBar::EventHandler(wxAuiToolBarEvent& event)
	{
		if (IsValidID(event.GetToolId()))
		{
			if (AuiToolBarItem* item = FindToolByID(event.GetToolId()))
			{
				bool canShowMenu = false;
				EventID type = event.GetEventType();
				if (type == wxEVT_AUITOOLBAR_RIGHT_CLICK)
				{
					type = AuiToolBarEvent::EvtItemRightClick;
					//canShowMenu = item->HasDropdownMenu() && item->ContainsOption(AuiToolBarItemOption::MenuOnRightClick);
				}
				else if (type == wxEVT_AUITOOLBAR_MIDDLE_CLICK)
				{
					type = AuiToolBarEvent::EvtItemMiddleClick;
					//canShowMenu = item->HasDropdownMenu() && item->ContainsOption(AuiToolBarItemOption::MenuOnMiddleClick);
				}
				else if (type == wxEVT_AUITOOLBAR_TOOL_DROPDOWN)
				{
					type = AuiToolBarEvent::EvtItemDropdown;
				}
				else if (type == wxEVT_AUITOOLBAR_OVERFLOW_CLICK)
				{
					type = AuiToolBarEvent::EvtOverflowClick;
				}
				else
				{
					type = wxEVT_NULL;
				}

				if (type != wxEVT_NULL)
				{
					AuiToolBarEvent newEvent(event);
					newEvent.SetId(GetId());
					newEvent.SetEventType(type.AsInt());
					newEvent.SetEventObject(item);
					newEvent.SetInt(item->IsToggled());
					newEvent.SetItem(item);

					bool wasProcessed = item->ProcessEvent(newEvent);

					// Handle dropdown menu
					if (canShowMenu && (!wasProcessed || newEvent.GetSkipped()))
					{
						item->ShowDropdownMenu();
						item->Refresh();
						DoIdleUpdate();
					}
				}
			}
		}
		event.Skip();
	}
	void AuiToolBar::OnLeftClick(wxCommandEvent& event)
	{
		if (IsValidID(event.GetId()))
		{
			if (AuiToolBarItem* item = FindToolByID(event.GetId()))
			{
				AuiToolBarEvent newEvent(AuiToolBarEvent::EvtItemClick, GetId());
				newEvent.SetEventObject(item);
				newEvent.SetToolId(item->GetID());
				newEvent.SetInt(item->IsToggled());
				newEvent.SetItem(item);
				bool wasProcessed = item->ProcessEvent(newEvent);

				// Handle dropdown menu
				if (item->ContainsOption(AuiToolBarItemOption::MenuOnLeftClick) /*&& item->HasDropdownMenu()*/ && (!wasProcessed || newEvent.GetSkipped()))
				{
					item->ShowDropdownMenu();
					item->Refresh();
					DoIdleUpdate();
				}
			}
		}
		event.Skip();
	}

	AuiToolBarItem* AuiToolBar::DoCreateTool(wxAuiToolBarItem* item)
	{
		if (item)
		{
			auto it = m_Items.insert_or_assign(item->GetId(), std::make_unique<AuiToolBarItem>(*this, *item));
			return it.first->second.get();
		}
		return nullptr;
	}
	AuiToolBarItem* AuiToolBar::DoGetTool(const wxAuiToolBarItem& item)
	{
		auto it = m_Items.find(item.GetId());
		return it != m_Items.end() ? it->second.get() : nullptr;
	}
	bool AuiToolBar::DoRemoveTool(wxAuiToolBarItem& item)
	{
		auto it = m_Items.find(item.GetId());
		if (it != m_Items.end())
		{
			wxAuiToolBar::DeleteTool(item.GetId());
			m_Items.erase(it);

			return true;
		}
		return false;
	}

	size_t AuiToolBar::DoGetToolIndex(const AuiToolBarItem& item) const
	{
		const auto& items = wxAuiToolBar::m_items;
		for (size_t i = 0; i < items.size(); i++)
		{
			if (&items[i] == item.m_Item)
			{
				return i;
			}
		}
		return AuiToolBarItem::npos;
	}
	bool AuiToolBar::DoSetToolIndex(AuiToolBarItem& item, size_t newIndex)
	{
		if (newIndex < GetToolCount())
		{
			if (item.GetIndex() == newIndex)
			{
				return true;
			}

			auto& items = wxAuiToolBar::m_items;
			for (size_t i = 0; i < items.size(); i++)
			{
				if (&items[i] == item.m_Item)
				{
					items.Insert(*item.m_Item, newIndex, 1);
					if (newIndex > i)
					{
						items.RemoveAt(i);
					}
					else
					{
						items.RemoveAt(i + 1);
					}
					return true;
				}
			}
		}
		return false;
	}

	bool AuiToolBar::Create(wxWindow* parent,
							wxWindowID id,
							FlagSet<AuiToolBarStyle> style
	)
	{
		if (wxAuiToolBar::Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt()))
		{
			SetMargins(0, 0, 0, 0);
			SetArtProvider(new AuiToolBarArt(*this));

			Bind(wxEVT_MENU, &AuiToolBar::OnLeftClick, this);
			Bind(wxEVT_AUITOOLBAR_MIDDLE_CLICK, &AuiToolBar::EventHandler, this);
			Bind(wxEVT_AUITOOLBAR_RIGHT_CLICK, &AuiToolBar::EventHandler, this);
			Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &AuiToolBar::EventHandler, this);
			Bind(wxEVT_AUITOOLBAR_OVERFLOW_CLICK, &AuiToolBar::EventHandler, this);
			return true;
		}
		return false;
	}
	void AuiToolBar::ClearTools()
	{
		m_Items.clear();
		wxAuiToolBar::ClearTools();
	}

	AuiToolBarItem* AuiToolBar::AddTool(const String& label, const GDIBitmap& bitmap, wxItemKind kind, const String& shortHelp)
	{
		return DoCreateTool(wxAuiToolBar::AddTool(wxID_ANY, label, bitmap.ToWxBitmap(), shortHelp, kind == wxITEM_DROPDOWN ? wxITEM_NORMAL : kind));
	}
	AuiToolBarItem* AuiToolBar::AddTool(const String& label, const GDIBitmap& bitmap, const GDIBitmap& disabledBitmap, wxItemKind kind, const String& shortHelp)
	{
		return DoCreateTool(wxAuiToolBar::AddTool(wxID_ANY, label, bitmap.ToWxBitmap(), disabledBitmap.ToWxBitmap(), kind, shortHelp, {}, nullptr));
	}
	AuiToolBarItem* AuiToolBar::AddTool(const GDIBitmap& bitmap, const GDIBitmap& disabledBitmap, bool toggle, const String& shortHelp, const String& longHelp)
	{
		return DoCreateTool(wxAuiToolBar::AddTool(wxID_ANY, bitmap.ToWxBitmap(), disabledBitmap.ToWxBitmap(), toggle, nullptr, shortHelp, longHelp));;
	}

	AuiToolBarItem* AuiToolBar::AddLabel(const String& label, const int width)
	{
		return DoCreateTool(wxAuiToolBar::AddLabel(wxID_ANY, label, width));
	}
	AuiToolBarItem* AuiToolBar::AddControl(wxControl* control, const String& label)
	{
		return DoCreateTool(wxAuiToolBar::AddControl(control, label));
	}
	AuiToolBarItem* AuiToolBar::AddSeparator()
	{
		return DoCreateTool(wxAuiToolBar::AddSeparator());
	}
	AuiToolBarItem* AuiToolBar::AddSpacer(int pixels)
	{
		return DoCreateTool(wxAuiToolBar::AddSpacer(pixels));
	}
	AuiToolBarItem* AuiToolBar::AddStretchSpacer(int proportion)
	{
		return DoCreateTool(wxAuiToolBar::AddStretchSpacer(proportion));
	}

	AuiToolBarItem* AuiToolBar::FindToolByPosition(const Point& pos) const
	{
		if (wxAuiToolBarItem* item = wxAuiToolBar::FindToolByPosition(pos.GetX(), pos.GetY()))
		{
			return const_cast<AuiToolBar&>(*this).DoGetTool(*item);
		}
		return nullptr;
	}
	AuiToolBarItem* AuiToolBar::FindToolByIndex(int index) const
	{
		if (wxAuiToolBarItem* item = wxAuiToolBar::FindToolByIndex(index))
		{
			return const_cast<AuiToolBar&>(*this).DoGetTool(*item);
		}
		return nullptr;
	}
	AuiToolBarItem* AuiToolBar::FindToolByID(wxWindowID id) const
	{
		if (wxAuiToolBarItem* item = wxAuiToolBar::FindTool(id))
		{
			return const_cast<AuiToolBar&>(*this).DoGetTool(*item);
		}
		return nullptr;
	}

	bool AuiToolBar::RemoveTool(AuiToolBarItem& tool)
	{
		return DoRemoveTool(tool);
	}
	bool AuiToolBar::RemoveTool(int index)
	{
		if (AuiToolBarItem* item = FindToolByIndex(index))
		{
			return RemoveTool(*item);
		}
		return false;
	}

	void AuiToolBar::UpdateUI()
	{
		wxWindowUpdateLocker lock1(GetParent());
		wxWindowUpdateLocker lock2(this);

		Realize();
		GetParent()->Layout();
		ScheduleRefresh();
	}
}

namespace kxf::UI
{
	void AuiToolBarArt::DrawPlainBackground(wxDC& dc, wxWindow* window, const wxRect& rect)
	{
		if (Color colorBG = m_Instance->GetBackgroundColour())
		{
			dc.SetBackground(colorBG);
			dc.Clear();
		}
		else
		{
			wxAuiDefaultToolBarArt::DrawPlainBackground(dc, window, rect);
		}

		if (Color colorBorder = m_Instance->GetBorderColor())
		{
			dc.SetPen(colorBorder);
			dc.DrawLine(rect.GetLeftBottom(), rect.GetRightBottom() + Point(1, 0));
		}
	}
}
