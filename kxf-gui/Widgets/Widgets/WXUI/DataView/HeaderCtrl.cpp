#include "kxf-pch.h"
#include "HeaderCtrl.h"
#include "View.h"
#include "MainWindow.h"
#include "../../DataView/Column.h"
#include "kxf/System/SystemWindow.h" 
#include "kxf/System/SystemInformation.h"
#include "kxf/Utility/Drawing.h"
#include "kxf-gui/Drawing/IRendererNative.h"
#include "kxf-gui/Widgets/Common.h"

#include "kxf/Win32/WrapperUnicode.h"
kxf_MakeWinUnicodeCallWrapper(SendMessage);
#include <wx/headerctrl.h>
#include <CommCtrl.h>

namespace
{
	enum class MouseButtonType
	{
		Left = 0,
		Right = 1,
		Middle = 2,
	};
	wxEventTypeTag<wxHeaderCtrlEvent> GetClickEventType(bool isDoubleClick, MouseButtonType button)
	{
		switch (button)
		{
			case MouseButtonType::Left:
			{
				return isDoubleClick ? wxEVT_HEADER_DCLICK : wxEVT_HEADER_CLICK;
			}
			case MouseButtonType::Right:
			{
				return isDoubleClick ? wxEVT_HEADER_RIGHT_DCLICK : wxEVT_HEADER_RIGHT_CLICK;
			}
			case MouseButtonType::Middle:
			{
				return isDoubleClick ? wxEVT_HEADER_MIDDLE_DCLICK : wxEVT_HEADER_MIDDLE_CLICK;
			}
		};
		return wxEVT_NULL;
	}

	kxf::Rect MakeWidthRect(int value)
	{
		return {0, 0, value, 0};
	};
}

namespace kxf::WXUI::DataView
{
	void* HeaderCtrl::GetHeaderCtrlHandle() const
	{
		return m_HeaderCtrlHandle;
	}
	void HeaderCtrl::DoSetSize(int x, int y, int width, int height, int sizeFlags)
	{
		wxControl::DoSetSize(x + m_ScrollOffset, y, width - m_ScrollOffset, height, sizeFlags & wxSIZE_FORCE);
	}
	void HeaderCtrl::ScrollWidget(int dx, int dy, const Rect& rect)
	{
		// As the native control doesn't support offsetting its contents, we use a
		// hack here to make it appear correctly when the parent is scrolled.
		// Instead of scrolling or repainting we simply move the control window
		// itself. To be precise, offset it by the scroll increment to the left and
		// increment its width to still extend to the right boundary to compensate
		// for it (notice that 'dx' is negative when scrolling to the right)

		m_ScrollOffset += dx;
		wxControl::DoSetSize(GetPosition().x + dx, 0, GetSize().x - dx, -1, wxSIZE_USE_EXISTING);
	}

	void HeaderCtrl::FinishEditing()
	{
		m_View->GetMainWindow()->EndEdit();
	}
	HeaderCtrl::EventResult HeaderCtrl::SendWidgetEvent(DataViewWidgetEvent& event, const EventID& eventID, DV::Column* column, std::optional<Rect> rect)
	{
		if (rect)
		{
			event.SetRect(*rect);
		}

		return {m_View->m_Widget.ProcessEvent(event, eventID), event.IsAllowed()};
	}
	HeaderCtrl::EventResult HeaderCtrl::SendWidgetEvent(const EventID& eventID, DV::Column* column, std::optional<Rect> rect)
	{
		DataViewWidgetEvent event(m_View->m_Widget);
		return SendWidgetEvent(event, eventID, column, std::move(rect));
	}

	void HeaderCtrl::OnCreate(wxWindowCreateEvent& event)
	{
		m_HeaderCtrlHandle = event.GetWindow()->GetHandle();
		if (m_HeaderCtrlHandle)
		{
			// Enable all required styles
			SystemWindow(m_HeaderCtrlHandle).ModWindowStyle(GWL_STYLE, HDS_HORZ|HDS_CHECKBOXES|HDS_BUTTONS|HDS_HOTTRACK|HDS_FULLDRAG, true);
		}

		event.Skip();
	}
	void HeaderCtrl::OnDestroy(wxWindowDestroyEvent& event)
	{
		m_HeaderCtrlHandle = nullptr;
		event.Skip();
	}

	void HeaderCtrl::OnClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		if (auto column = m_View->GetColumnAt(event.GetColumn()))
		{
			if (SendWidgetEvent(DataViewWidgetEvent::EvtColumnHeaderClick, column).Processed)
			{
				return;
			}

			// Default handling for the column click is to sort by this column or  toggle its sort order
			if (!column->GetStyle().Contains(DV::ColumnStyle::Sortable))
			{
				// No default handling for non-sortable columns
				return;
			}

			if (column->GetSortOrder() != SortOrder::None)
			{
				// Already using this column for sorting, just change the order
				column->ToggleSortOrder();
			}
			else
			{
				// Not using this column for sorting yet.
				// We will sort by this column only now, so reset all the previously used ones.
				m_View->ResetAllSortColumns();
				column->SetSortOrder(SortOrder::Ascending);
			}

			m_View->GetMainWindow()->OnShouldResort();
			m_View->OnColumnChange(*column);
			SendWidgetEvent(DataViewWidgetEvent::EvtColumnSorted, column);
		}
	}
	void HeaderCtrl::OnRClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		if (auto column = m_View->GetColumnAt(event.GetColumn()))
		{
			// Event wasn't processed somewhere, use default behavior
			if (!SendWidgetEvent(DataViewWidgetEvent::EvtColumnHeaderRClick, column).Processed && m_View)
			{
				/*
				if (Menu menu; m_View->CreateColumnSelectionMenu(menu))
				{
					m_View->OnColumnSelectionMenu(menu);
				}
				*/
			}
		}
	}
	void HeaderCtrl::OnWindowClick(wxMouseEvent& event)
	{
		Point pos = Point(event.GetPosition());
		if (pos.GetX() > m_View->GetMainWindow()->GetRowWidth())
		{
			SendWidgetEvent(event.GetEventType() == wxEVT_LEFT_UP ? DataViewWidgetEvent::EvtColumnHeaderClick : DataViewWidgetEvent::EvtColumnHeaderRClick, nullptr);
		}
		event.Skip();
	}
	void HeaderCtrl::OnSeparatorDClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		auto column = m_View->GetColumnAt(event.GetColumn());
		if (column && column->GetStyle().Contains(DV::ColumnStyle::Resizeable))
		{
			column->FitContent();
		}
	}

	void HeaderCtrl::OnResize(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		if (auto column = m_View->GetColumnAt(event.GetColumn()))
		{
			column->m_Width = event.GetWidth();
			GetMainWindow()->RefreshDisplay();
		}
	}
	void HeaderCtrl::OnResizeEnd(wxHeaderCtrlEvent& event)
	{
		GetMainWindow()->UpdateDisplay();
	}
	void HeaderCtrl::OnReorderEnd(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		auto column = m_View->GetColumnAt(event.GetColumn());
		if (column)
		{
			m_View->MoveColumnToPhysicalIndex(*column, event.GetNewOrder());
		}
	}

	bool HeaderCtrl::UpdateColumnWidthToFit(size_t index)
	{
		if (auto column = m_View->GetColumnAt(index))
		{
			return column->FitContent();
		}
		return false;
	}
	void HeaderCtrl::DoMakeItem(_HD_ITEMW& item, const DV::Column& column)
	{
		item.mask |= HDI_FORMAT|HDI_TEXT|HDI_LPARAM;
		item.lParam = reinterpret_cast<LPARAM>(&column);

		// Title text
		item.pszText = column.m_Title.wc_str().unsafe_data();
		item.cchTextMax = column.m_Title.length();

		// Bitmap
		const auto& bitmap = column.m_Icon;
		if (bitmap)
		{
			item.mask |= HDI_IMAGE;

			if (!m_ImageList)
			{
				m_ImageList = std::make_unique<GDIImageList>(bitmap.GetSize());
				Header_SetImageList(reinterpret_cast<HWND>(GetHeaderCtrlHandle()), m_ImageList->GetHIMAGELIST());
			}
			item.iImage = m_ImageList->Add(bitmap);
		}
		else
		{
			// No bitmap but we still need to update the item
			item.mask |= HDI_IMAGE;
			item.iImage = I_IMAGENONE;
		}

		// Alignment
		if (column.GetTitleAlignment() != Alignment::None)
		{
			item.mask |= HDF_LEFT;
			switch (*column.GetTitleAlignment())
			{
				case Alignment::Left:
				{
					item.fmt |= HDF_LEFT;
					break;
				}
				case Alignment::Center:
				case Alignment::CenterVertical:
				case Alignment::CenterHorizontal:
				{
					item.fmt |= HDF_CENTER;
					break;
				}
				case Alignment::Right:
				{
					item.fmt |= HDF_RIGHT;
					break;
				}
			};
		}

		// Sort order
		if (auto sortOrder = column.GetSortOrder(); sortOrder != SortOrder::None)
		{
			item.fmt |= sortOrder == SortOrder::Ascending ? HDF_SORTUP : HDF_SORTDOWN;
		}

		// Resizing
		if (!column.GetStyle().Contains(DV::ColumnStyle::Resizeable))
		{
			item.fmt |= HDF_FIXEDWIDTH;
		}

		// Width
		item.mask |= HDI_WIDTH;
		item.cxy = column.GetWidth();

		// Display order (doesn't work properly, we're using custom ordering)
		//item.mask |= HDI_ORDER;
		//item.iOrder = column.GetPhysicalDisplayIndex();

		// Dropdown
		if (column.GetStyle().Contains(DV::ColumnStyle::Dropdown))
		{
			item.fmt |= HDF_SPLITBUTTON;
		}

		// Check-box
		if (column.GetStyle().Contains(DV::ColumnStyle::CheckBox))
		{
			item.fmt |= HDF_CHECKBOX;
			item.fmt |= column.IsChecked() ? HDF_CHECKED : 0;
		}
	}
	bool HeaderCtrl::MSWOnNotify(int ctrlID, WXLPARAM lParam, WXLPARAM* result)
	{
		MainWindow* mainWindow = GetMainWindow();
		const NMHEADERW* header = reinterpret_cast<NMHEADERW*>(lParam);
		const int notification = header->hdr.code;

		auto GetColumn = [this, header](bool hitTest = false)
		{
			if (hitTest)
			{
				DV::Column* column = nullptr;
				GetMainWindow()->HitTest(Point(m_View->ScreenToClient(::wxGetMousePosition())), nullptr, &column);

				return column;
			}
			else
			{
				HDITEMW headerItem = {};
				headerItem.mask = HDI_LPARAM;
				Header_GetItem(reinterpret_cast<HWND>(GetHeaderCtrlHandle()), header->iItem, &headerItem);

				return reinterpret_cast<DV::Column*>(headerItem.lParam);
			}
		};
		auto NewHeaderEvent = [this](wxEventType eventType, DV::Column* column)
		{
			wxHeaderCtrlEvent event(eventType, GetId());
			event.SetEventObject(this);
			event.SetColumn(column ? column->GetIndex() : -1);

			return event;
		};
		auto SendHeaderEvent = [this, result](wxHeaderCtrlEvent& event)
		{
			// All of HDN_BEGIN{DRAG,TRACK}, HDN_TRACK and HDN_ITEMCHANGING
			// interpret TRUE in '*result' as a meaning to stop the control
			// default handling of the message.

			if (ProcessWindowEvent(event) && !event.IsAllowed())
			{
				*result = TRUE;
			}
		};

		switch (notification)
		{
			// Click
			case (int)HDN_ITEMCLICK:
			case (int)HDN_ITEMDBLCLICK:
			{
				m_DraggedColumn = nullptr;
				if (auto column = GetColumn())
				{
					wxEventType eventType = ::GetClickEventType(notification == static_cast<int>(HDN_ITEMDBLCLICK), static_cast<MouseButtonType>(header->iButton));
					wxHeaderCtrlEvent event = NewHeaderEvent(eventType, column);
					SendHeaderEvent(event);

					return true;
				}
				return false;
			}
			case (int)HDN_DIVIDERDBLCLICK:
			{
				if (auto column = GetColumn())
				{
					if (!SendWidgetEvent(DataViewWidgetEvent::EvtColumnHeaderSeparatorDClick, column).Processed)
					{
						wxHeaderCtrlEvent event = NewHeaderEvent(wxEVT_HEADER_SEPARATOR_DCLICK, column);
						SendHeaderEvent(event);
					}
					else
					{
						*result = TRUE;
					}
					return true;
				}
				return false;
			}
			case (int)NM_RCLICK:
			case (int)NM_RDBLCLK:
			{
				// These two messages aren't from Header control and they contain no item index
				// so we have to use hit-testing to get the column.
				m_DraggedColumn = nullptr;
				if (DV::Column* column = GetColumn(true))
				{
					wxEventType eventType = ::GetClickEventType(notification == NM_RDBLCLK, MouseButtonType::Right);
					wxHeaderCtrlEvent event = NewHeaderEvent(eventType, column);
					SendHeaderEvent(event);

					return true;
				}
				return false;
			}

			// Resizing
			case (int)HDN_BEGINTRACKA:
			case (int)HDN_BEGINTRACKW:
			{
				if (!m_ResizedColumn)
				{
					// These messages are from header control but active area of grabbing column edge
					// is wider than column rect and hit-testing won't work here.
					auto column = GetColumn();
					if (column && column->GetStyle().Contains(DV::ColumnStyle::Resizeable))
					{
						m_ResizedColumn = column;

						if (SendWidgetEvent(DataViewWidgetEvent::EvtColumnBeginResize, m_ResizedColumn).Allowed)
						{
							wxHeaderCtrlEvent event = NewHeaderEvent(wxEVT_HEADER_BEGIN_RESIZE, m_ResizedColumn);
							SendHeaderEvent(event);
						}
						else
						{
							*result = TRUE;
						}
						return true;
					}
				}

				*result = TRUE;
				return true;
			}
			case (int)HDN_ENDTRACKA:
			case (int)HDN_ENDTRACKW:
			{
				if (m_ResizedColumn)
				{
					int& width = header->pitem->cxy;

					const bool isLess = width < m_ResizedColumn->GetMinWidth();
					if (isLess)
					{
						width = m_ResizedColumn->GetMinWidth();
					}

					DataViewWidgetEvent ctrlEvent(m_View->m_Widget);
					if (!SendWidgetEvent(ctrlEvent, DataViewWidgetEvent::EvtColumnEndResize, m_ResizedColumn, MakeWidthRect(width)).Allowed || isLess)
					{
						*result = TRUE;
					}
					else
					{
						wxHeaderCtrlEvent event = NewHeaderEvent(wxEVT_HEADER_END_RESIZE, m_ResizedColumn);
						event.SetWidth(ctrlEvent.GetSize().GetWidth());
						SendHeaderEvent(event);
					}

					m_ResizedColumn = nullptr;
					return true;
				}
				return false;
			}
			case (int)HDN_TRACK:
			case (int)HDN_ITEMCHANGING:
			{
				if (m_ResizedColumn && header->pitem && (header->pitem->mask & HDI_WIDTH))
				{
					int& width = header->pitem->cxy;
					if (m_ResizedColumn->GetStyle().Contains(DV::ColumnStyle::Resizeable))
					{
						const bool isLess = width < m_ResizedColumn->GetMinWidth();
						if (isLess)
						{
							width = m_ResizedColumn->GetMinWidth();
						}

						DataViewWidgetEvent ctrlEvent(m_View->m_Widget);
						if (!SendWidgetEvent(ctrlEvent, DataViewWidgetEvent::EvtColumnResize, m_ResizedColumn, MakeWidthRect(width)).Allowed || isLess)
						{
							*result = TRUE;
						}
						else
						{
							wxHeaderCtrlEvent event = NewHeaderEvent(wxEVT_HEADER_RESIZING, m_ResizedColumn);
							event.SetWidth(ctrlEvent.GetSize().GetWidth());
							SendHeaderEvent(event);
						}
					}
					else
					{
						width = m_ResizedColumn->GetWidth();
						*result = TRUE;
					}
					return true;
				}
				return false;
			}

			// Reordering
			case (int)HDN_BEGINDRAG:
			{
				if (!m_DraggedColumn)
				{
					auto column = GetColumn();
					if (column && column->GetStyle().Contains(DV::ColumnStyle::Moveable))
					{
						m_DraggedColumn = column;

						wxHeaderCtrlEvent event = NewHeaderEvent(wxEVT_HEADER_BEGIN_REORDER, m_DraggedColumn);
						SendHeaderEvent(event);

						return true;
					}
				}
				return false;
			}
			case (int)HDN_ENDDRAG:
			{
				const int order = header->pitem->iOrder;
				if (m_DraggedColumn && order != -1)
				{
					auto column = m_DraggedColumn;
					m_DraggedColumn = nullptr;
					m_UpdateColumns = true;

					wxHeaderCtrlEvent event = NewHeaderEvent(wxEVT_HEADER_END_REORDER, column);
					event.SetNewOrder(order);
					SendHeaderEvent(event);

					return true;
				}
				break;
			}
			case (int)NM_RELEASEDCAPTURE:
			{
				if (m_DraggedColumn)
				{
					wxHeaderCtrlEvent event = NewHeaderEvent(wxEVT_HEADER_DRAGGING_CANCELLED, m_DraggedColumn);
					m_DraggedColumn = nullptr;
					SendHeaderEvent(event);

					return true;
				}
				return false;
			}

			// Checkbox
			case (int)HDN_ITEMSTATEICONCLICK:
			{
				if (auto column = GetColumn())
				{
					// Send an event, if it wasn't processed, toggle check state ourselves.
					// In any case update native column state after the event handler returns.
					const bool isChecked = column->IsChecked();
					if (!SendWidgetEvent(DataViewWidgetEvent::EvtColumnToggle, column).Processed)
					{
						column->SetChecked(!isChecked);
					}
					UpdateColumn(*column);
					return true;
				}
				return false;
			}

			// Dropdown
			case (int)HDN_DROPDOWN:
			{
				if (auto column = GetColumn())
				{
					SendWidgetEvent(DataViewWidgetEvent::EvtColumnDropdown, column, GetDropdownRect(header->iItem));
					return true;
				}
				return false;
			}
		};
		return false;
	}
	void HeaderCtrl::OnInternalIdle()
	{
		if (m_UpdateColumns)
		{
			DoUpdate();
			m_UpdateColumns = false;
		}

		wxControl::OnInternalIdle();
		WindowRefreshScheduler::OnInternalIdle();
	}

	void HeaderCtrl::DoUpdate()
	{
		if (!IsShown())
		{
			return;
		}

		if (m_HeaderCtrlHandle)
		{
			// First delete all old columns
			const size_t oldItemsCount = Header_GetItemCount(reinterpret_cast<HWND>(m_HeaderCtrlHandle));
			for (size_t i = 0; i < oldItemsCount; i++)
			{
				Header_DeleteItem(reinterpret_cast<HWND>(m_HeaderCtrlHandle), 0);
			}

			// Clear image list
			if (m_ImageList)
			{
				m_ImageList->Clear();
			}

			// And add the new ones
			size_t index = 0;
			bool hasResizableColumns = false;
			bool hasMoveableColumns = false;

			for (const auto* column: m_View->GetColumnsInPhysicalDisplayOrder())
			{
				HDITEMW item = {};
				DoMakeItem(item, *column);
				Header_InsertItem(reinterpret_cast<HWND>(m_HeaderCtrlHandle), index, &item);
				index++;

				if (!hasResizableColumns)
				{
					hasResizableColumns = column->GetStyle().Contains(DV::ColumnStyle::Resizeable);
				}
				if (!hasMoveableColumns)
				{
					hasMoveableColumns = column->GetStyle().Contains(DV::ColumnStyle::Moveable);
				}
			}

			// Update styles based on column properties
			SystemWindow(m_HeaderCtrlHandle).ModWindowStyle(GWL_STYLE, [&](FlagSet<intptr_t> style)
			{
				style.Mod(HDS_NOSIZING, !hasResizableColumns);
				style.Mod(HDS_DRAGDROP, hasMoveableColumns);

				return style;
			});
		}
	}
	void HeaderCtrl::UpdateColumn(const DV::Column& column)
	{
		DoUpdate();
	}
	void HeaderCtrl::UpdateColumnCount()
	{
		DoUpdate();
	}

	Rect HeaderCtrl::GetDropdownRect(size_t index) const
	{
		RECT rect = {};
		Header_GetItemDropDownRect(reinterpret_cast<HWND>(GetHeaderCtrlHandle()), index, &rect);
		return Utility::FromWindowsRect(rect);
	}
	Rect HeaderCtrl::GetDropdownRect(const DV::Column& column) const
	{
		return GetDropdownRect(column.GetPhysicalDisplayIndex());
	}

	HeaderCtrl::HeaderCtrl(View& parent)
		:m_View(&parent)
	{
		Bind(wxEVT_CREATE, &HeaderCtrl::OnCreate, this);
		Bind(wxEVT_DESTROY, &HeaderCtrl::OnDestroy, this);

		if ([&]()
		{
			if (!CreateControl(&parent, wxID_NONE, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxCLIP_CHILDREN|wxTAB_TRAVERSAL, wxDefaultValidator, "HeaderCtrl"))
			{
				return false;
			}
			if (!MSWCreateControl(WC_HEADER, "", wxDefaultPosition, wxDefaultSize))
			{
				return false;
			}

			// Special hack for margins when using comctl32.dll v6 or later: the
			// default margin is too big and results in label truncation when the
			// column width is just about right to show it together with the sort
			// indicator, so reduce it to a smaller value (in principle we could even
			// use 0 here but this starts to look ugly)
			Header_SetBitmapMargin(GetHandle(), System::GetMetric(SystemSizeMetric::Edge, &parent).GetWidth());
			return true;
		}())
		{
			// See comment in 'HeaderCtrl::SetBackgroundColour' for details
			// about why double-buffering needs to be disabled.
			SetDoubleBuffered(false);

			// Events
			Bind(wxEVT_HEADER_CLICK, &HeaderCtrl::OnClick, this);
			Bind(wxEVT_HEADER_RIGHT_CLICK, &HeaderCtrl::OnRClick, this);
			Bind(wxEVT_HEADER_SEPARATOR_DCLICK, &HeaderCtrl::OnSeparatorDClick, this);
			Bind(wxEVT_LEFT_UP, &HeaderCtrl::OnWindowClick, this);
			Bind(wxEVT_RIGHT_UP, &HeaderCtrl::OnWindowClick, this);

			Bind(wxEVT_HEADER_RESIZING, &HeaderCtrl::OnResize, this);
			Bind(wxEVT_HEADER_END_RESIZE, &HeaderCtrl::OnResizeEnd, this);
			Bind(wxEVT_HEADER_END_REORDER, &HeaderCtrl::OnReorderEnd, this);
		}
	}
	HeaderCtrl::~HeaderCtrl()
	{
		m_HeaderCtrlHandle = nullptr;
	}

	MainWindow* HeaderCtrl::GetMainWindow() const
	{
		return m_View ? m_View->GetMainWindow() : nullptr;
	}

	void HeaderCtrl::ToggleSortByColumn(size_t index)
	{
		if (m_View->GetStyle().Contains(DV::WidgetStyle::MultiColumnSort))
		{
			return;
		}
		else if (auto column = m_View->GetColumnAt(index))
		{
			ToggleSortByColumn(*column);
		}
	}
	void HeaderCtrl::ToggleSortByColumn(DV::Column& column)
	{
		if (m_View->GetStyle().Contains(DV::WidgetStyle::MultiColumnSort) && column.GetStyle().Contains(DV::ColumnStyle::Sortable))
		{
			column.ToggleSortOrder();
			SendWidgetEvent(DataViewWidgetEvent::EvtColumnSorted, &column);
		}
	}

	bool HeaderCtrl::SetBackgroundColour(const wxColour& color)
	{
		// Skip setting the background color altogether to prevent 'wxHeaderCtrl' from trying to
		// owner-draw background because it causes flicker on resizing columns when double-buffering
		// is not enabled, but enabling double-buffering have some negative consequences on drag image.
		return false;
	}
}
