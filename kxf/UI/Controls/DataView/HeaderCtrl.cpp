#include "KxfPCH.h"
#include "HeaderCtrl.h"
#include "View.h"
#include "MainWindow.h"
#include "Column.h"
#include "kxf/UI/Common.h"
#include "kxf/System/SystemWindow.h" 
#include "kxf/System/SystemInformation.h"
#include "kxf/Drawing/IRendererNative.h"
#include "kxf/Utility/Drawing.h"

Kx_MakeWinUnicodeCallWrapper(SendMessage);
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

namespace kxf::UI::DataView
{
	void* HeaderCtrl::GetHeaderCtrlHandle() const
	{
		return m_HeaderCtrlHandle;
	}
	void HeaderCtrl::DoSetSize(int x, int y, int width, int height, int sizeFlags)
	{
		wxControl::DoSetSize(x + m_ScrollOffset, y, width - m_ScrollOffset, height, sizeFlags & wxSIZE_FORCE);
	}
	void HeaderCtrl::ScrollWidget(int dx)
	{
		// As the native control doesn't support offsetting its contents, we use a
		// hack here to make it appear correctly when the parent is scrolled.
		// Instead of scrolling or repainting we simply move the control window
		// itself. To be precise, offset it by the scroll increment to the left and
		// increment its width to still extend to the right boundary to compensate
		// for it (notice that dx is negative when scrolling to the right)

		m_ScrollOffset += dx;
		wxControl::DoSetSize(GetPosition().x + dx, -1, GetSize().x - dx, -1, wxSIZE_USE_EXISTING);
	}

	void HeaderCtrl::FinishEditing()
	{
		m_View->GetMainWindow()->EndEdit();
	}
	HeaderCtrl::EventResult HeaderCtrl::SendCtrlEvent(ItemEvent& event, const EventID& type, Column* column, std::optional<Rect> rect)
	{
		event.SetEventType(type.AsInt());
		if (rect)
		{
			event.SetRect(*rect);
		}
		m_View->GetMainWindow()->CreateEventTemplate(event, nullptr, column);

		return {m_View->ProcessWindowEvent(event), event.IsAllowed()};
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

		if (Column* column = m_View->GetColumn(event.GetColumn()))
		{
			if (SendCtrlEvent(ItemEvent::EvtColumnHeaderClick, column).Processed)
			{
				return;
			}

			// Default handling for the column click is to sort by this column or  toggle its sort order
			if (!column->IsSortable())
			{
				// No default handling for non-sortable columns
				return;
			}

			if (column->IsSorted())
			{
				// Already using this column for sorting, just change the order
				column->ToggleSortOrder();
			}
			else
			{
				// Not using this column for sorting yet.
				// We will sort by this column only now, so reset all the previously used ones.
				m_View->ResetAllSortColumns();
				column->SortAscending();
			}

			m_View->GetMainWindow()->OnShouldResort();
			m_View->OnColumnChange(*column);
			SendCtrlEvent(ItemEvent::EvtColumnSorted, column);
		}
	}
	void HeaderCtrl::OnRClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		if (Column* column = m_View->GetColumn(event.GetColumn()))
		{
			// Event wasn't processed somewhere, use default behavior
			if (!SendCtrlEvent(ItemEvent::EvtColumnHeaderRClick, column).Processed && m_View)
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
			SendCtrlEvent(event.GetEventType() == wxEVT_LEFT_UP ? ItemEvent::EvtColumnHeaderClick : ItemEvent::EvtColumnHeaderRClick, nullptr);
		}
		event.Skip();
	}
	void HeaderCtrl::OnSeparatorDClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		Column* column = m_View->GetColumn(event.GetColumn());
		if (column && column->IsSizeable())
		{
			column->FitContent();
		}
	}

	void HeaderCtrl::OnResize(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		if (Column* column = m_View->GetColumn(event.GetColumn()))
		{
			column->AssignWidth(event.GetWidth());
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

		Column* column = m_View->GetColumn(event.GetColumn());
		if (column)
		{
			m_View->MoveColumnToPhysicalIndex(*column, event.GetNewOrder());
		}
	}

	bool HeaderCtrl::UpdateColumnWidthToFit(size_t index)
	{
		if (Column* column = m_View->GetColumn(index))
		{
			return column->FitContent();
		}
		return false;
	}
	void HeaderCtrl::DoMakeItem(_HD_ITEMW& item, const Column& column)
	{
		item.mask |= HDI_FORMAT|HDI_TEXT|HDI_LPARAM;
		item.lParam = reinterpret_cast<LPARAM>(&column);

		// Title text
		item.pszText = column.m_Title.wc_str().unsafe_data();
		item.cchTextMax = column.m_Title.length();

		// Bitmap
		const GDIBitmap& bitmap = column.m_Bitmap;
		if (bitmap)
		{
			item.mask |= HDI_IMAGE;
			if (column.GetStyle().Contains(ColumnStyle::IconOnRight))
			{
				item.fmt |= HDF_BITMAP_ON_RIGHT;
			}

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
		if (column.IsSorted())
		{
			item.fmt |= column.IsSortedAscending() ? HDF_SORTUP : HDF_SORTDOWN;
		}

		// Resizing
		if (!column.IsSizeable())
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
		if (column.HasDropdown())
		{
			item.fmt |= HDF_SPLITBUTTON;
		}

		// Checkbox
		if (column.HasCheckBox())
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
				return GetMainWindow()->HitTestColumn(Point(m_View->ScreenToClient(::wxGetMousePosition())));
			}
			else
			{
				HDITEMW headerItem = {};
				headerItem.mask = HDI_LPARAM;
				Header_GetItem(reinterpret_cast<HWND>(GetHeaderCtrlHandle()), header->iItem, &headerItem);

				return reinterpret_cast<Column*>(headerItem.lParam);
			}
		};
		auto NewHeaderEvent = [this](wxEventType eventType, Column* column)
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
				if (Column* column = GetColumn())
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
				if (Column* column = GetColumn())
				{
					if (!SendCtrlEvent(ItemEvent::EvtColumnHeaderSeparatorDClick, column).Processed)
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
				if (Column* column = GetColumn(true))
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
					Column* column = GetColumn();
					if (column && column->IsSizeable())
					{
						m_ResizedColumn = column;

						if (SendCtrlEvent(ItemEvent::EvtColumnBeginResize, m_ResizedColumn).Allowed)
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

					ItemEvent ctrlEvent;
					if (!SendCtrlEvent(ctrlEvent, ItemEvent::EvtColumnEndResize, m_ResizedColumn, MakeWidthRect(width)).Allowed || isLess)
					{
						*result = TRUE;
					}
					else
					{
						wxHeaderCtrlEvent event = NewHeaderEvent(wxEVT_HEADER_END_RESIZE, m_ResizedColumn);
						event.SetWidth(ctrlEvent.GetWidth());
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
					if (m_ResizedColumn->IsSizeable())
					{
						const bool isLess = width < m_ResizedColumn->GetMinWidth();
						if (isLess)
						{
							width = m_ResizedColumn->GetMinWidth();
						}

						ItemEvent ctrlEvent;
						if (!SendCtrlEvent(ctrlEvent, ItemEvent::EvtColumnResize, m_ResizedColumn, MakeWidthRect(width)).Allowed || isLess)
						{
							*result = TRUE;
						}
						else
						{
							wxHeaderCtrlEvent event = NewHeaderEvent(wxEVT_HEADER_RESIZING, m_ResizedColumn);
							event.SetWidth(ctrlEvent.GetWidth());
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
					Column* column = GetColumn();
					if (column && column->IsMoveable())
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
					Column* column = m_DraggedColumn;
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
				if (Column* column = GetColumn())
				{
					// Send an event, if it wasn't processed, toggle check state ourselves.
					// In any case update native column state after the event handler returns.
					const bool isChecked = column->IsChecked();
					if (!SendCtrlEvent(ItemEvent::EvtColumnToggle, column).Processed)
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
				if (Column* column = GetColumn())
				{
					SendCtrlEvent(ItemEvent::EvtColumnDropdown, column, GetDropdownRect(header->iItem));
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

			for (const Column* column: m_View->GetColumnsInPhysicalDisplayOrder())
			{
				HDITEMW item = {};
				DoMakeItem(item, *column);
				Header_InsertItem(reinterpret_cast<HWND>(m_HeaderCtrlHandle), index, &item);
				index++;

				if (!hasResizableColumns)
				{
					hasResizableColumns = column->IsSizeable();
				}
				if (!hasMoveableColumns)
				{
					hasMoveableColumns = column->IsMoveable();
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
	void HeaderCtrl::UpdateColumn(const Column& column)
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
	Rect HeaderCtrl::GetDropdownRect(const Column& column) const
	{
		return GetDropdownRect(column.GetPhysicalDisplayIndex());
	}

	HeaderCtrl::HeaderCtrl(View* parent)
		:m_View(parent)
	{
		Bind(wxEVT_CREATE, &HeaderCtrl::OnCreate, this);
		Bind(wxEVT_DESTROY, &HeaderCtrl::OnDestroy, this);

		if ([this, parent]()
		{
			if (!CreateControl(parent, wxID_NONE, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxCLIP_CHILDREN|wxTAB_TRAVERSAL, wxDefaultValidator, "HeaderCtrl"))
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
			Header_SetBitmapMargin(GetHandle(), System::GetMetric(SystemSizeMetric::Edge, parent).GetWidth());
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

	size_t HeaderCtrl::GetColumnCount() const
	{
		return m_View->GetColumnCount();
	}
	Column* HeaderCtrl::GetColumnAt(size_t index) const
	{
		return m_View->GetColumn(index);
	}
	Column* HeaderCtrl::GetColumnDisplayedAt(size_t index) const
	{
		return m_View->GetColumnDisplayedAt(index);
	}

	void HeaderCtrl::ToggleSortByColumn(size_t index)
	{
		if (m_View->IsMultiColumnSortAllowed())
		{
			return;
		}
		else if (Column* column = m_View->GetColumn(index))
		{
			ToggleSortByColumn(*column);
		}
	}
	void HeaderCtrl::ToggleSortByColumn(Column& column)
	{
		if (m_View->IsMultiColumnSortAllowed() && column.IsSortable())
		{
			if (column.IsSorted())
			{
				column.ResetSorting();
			}
			else
			{
				column.SortAscending();
			}
			SendCtrlEvent(ItemEvent::EvtColumnSorted, &column);
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
