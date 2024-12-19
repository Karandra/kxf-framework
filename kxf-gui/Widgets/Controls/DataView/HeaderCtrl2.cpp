#include "kxf-pch.h"
#include "HeaderCtrl2.h"
#include "View.h"
#include "MainWindow.h"
#include "Column.h"
#include "kxf/Utility/Common.h"
#include <CommCtrl.h>

namespace
{
	using namespace kxf;
	using namespace kxf::UI;

	enum class MouseButton
	{
		Left = 0,
		Right = 1,
		Middle = 2,
	};
	wxEventTypeTag<wxHeaderCtrlEvent> GetClickEventType(bool isDoubleClick, MouseButton button)
	{
		switch (button)
		{
			case MouseButton::Left:
			{
				return isDoubleClick ? wxEVT_HEADER_DCLICK : wxEVT_HEADER_CLICK;
			}
			case MouseButton::Right:
			{
				return isDoubleClick ? wxEVT_HEADER_RIGHT_DCLICK : wxEVT_HEADER_RIGHT_CLICK;
			}
			case MouseButton::Middle:
			{
				return isDoubleClick ? wxEVT_HEADER_MIDDLE_DCLICK : wxEVT_HEADER_MIDDLE_CLICK;
			}
		};
		return wxEVT_NULL;
	}

	Rect MakeWidthRect(int value)
	{
		return Rect(0, 0, value, 0);
	};
}

namespace kxf::UI::DataView
{
	void HeaderCtrl2::OnPaint(wxPaintEvent& event)
	{
		wxAutoBufferedPaintDC dc(this);
		dc.Clear();

		// Account for the horizontal scrollbar offset in the parent window
		dc.SetDeviceOrigin(m_ScrollOffset, 0);

		wxRendererNative& nativeRenderer = wxRendererNative::Get();
		Size clientSize = Size(GetClientSize());
		const bool isEnabled = IsEnabled();

		int offsetX = 0;
		for (const Column* column: m_View->GetColumnsInPhysicalDisplayOrder())
		{
			const int width = column->GetWidth();

			// Sort arrow state
			wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE;
			if (column->IsSortable())
			{
				sortArrow = column->IsSortedAscending() ? wxHDR_SORT_ICON_UP : wxHDR_SORT_ICON_DOWN;
			}

			// Header state
			int state = wxCONTROL_NONE;
			if (isEnabled)
			{
				if (column == m_HoverColumn)
				{
					state = wxCONTROL_CURRENT;
				}
			}
			else
			{
				state = wxCONTROL_DISABLED;
			}

			wxHeaderButtonParams headerParameters;
			headerParameters.m_labelText = column->GetTitle();
			headerParameters.m_labelBitmap = column->GetBitmap().AsWXBitmap();
			headerParameters.m_labelAlignment = column->GetTitleAlignment();
			nativeRenderer.DrawHeaderButton(this, dc, Rect(offsetX, 0, width, clientSize.GetHeight()), state, sortArrow, &headerParameters);

			// Move to next column
			offsetX += width;
		}

		// Draw the area after columns
		if (offsetX < clientSize.GetWidth())
		{
			int state = isEnabled ? wxCONTROL_DIRTY : wxCONTROL_DISABLED;
			nativeRenderer.DrawHeaderButton(this, dc, Rect(offsetX, 0, clientSize.GetWidth() - offsetX, clientSize.GetHeight()), state);
		}
	}
	void HeaderCtrl2::OnMouse(wxMouseEvent& mevent)
	{
		// Do this in advance to allow simply returning if we're not interested,
		// we'll undo it if we do handle the event below.
		mevent.Skip();

		const bool wasSeparatorDClick = m_WasSeparatorDClick;
		m_WasSeparatorDClick = false;

		// Account for the control displacement
		const int xPhysical = mevent.GetX();

		// first deal with the [continuation of any] dragging operations in progress
		if (IsResizing())
		{
			if (mevent.LeftUp())
			{
				EndResizing(xPhysical);
			}
			else
			{
				// Update the live separator position
				StartOrContinueResizing(*m_ResizedColumn, xPhysical);
			}
			return;
		}

		if (IsReordering())
		{
			if (!mevent.LeftUp())
			{
				// Update the column position
				UpdateReorderingMarker(xPhysical);
				return;
			}

			// Finish reordering and continue to generate a click event below if we didn't really reorder anything
			if (EndReordering(xPhysical))
			{
				return;
			}
		}

		// Find if the event is over a column at all
		bool onSeparator = false;
		Column* column = mevent.Leaving() ? nullptr : GetColumnAtPoint(xPhysical, &onSeparator);

		// Update the highlighted column if it changed
		if (column != m_HoverColumn)
		{
			Column* hoverOld = m_HoverColumn;
			m_HoverColumn = column;

			if (hoverOld)
			{
				RefreshColumn(*hoverOld);
			}
			if (m_HoverColumn)
			{
				RefreshColumn(*m_HoverColumn);
			}
		}

		// Update mouse cursor as it moves around
		if (mevent.Moving())
		{
			SetCursor(onSeparator ? wxCursor(wxCURSOR_SIZEWE) : wxNullCursor);
			return;
		}

		// All the other events only make sense when they happen over a column
		if (!column)
		{
			return;
		}

		// Enter various dragging modes on left mouse press
		if (mevent.LeftDown())
		{
			if (onSeparator)
			{
				// Start resizing the column
				StartOrContinueResizing(*column, xPhysical);
			}
			else if (HasFlag(wxHD_ALLOW_REORDER) && column->IsMoveable())
			{
				// On column itself - both header and column must have the appropriate flags to allow dragging the column
				StartReordering(*column, xPhysical);
			}
			return;
		}

		// Determine the type of header event corresponding to click events
		wxEventType evtType = wxEVT_NULL;
		const bool click = mevent.ButtonUp();
		const bool dblclk = mevent.ButtonDClick();

		if (click || dblclk)
		{
			switch (mevent.GetButton())
			{
				case wxMOUSE_BTN_LEFT:
				{
					// treat left double clicks on separator specially
					if (onSeparator && dblclk)
					{
						evtType = wxEVT_HEADER_SEPARATOR_DCLICK;
						m_WasSeparatorDClick = true;
					}
					else if (!wasSeparatorDClick)
					{
						evtType = click ? wxEVT_HEADER_CLICK
							: wxEVT_HEADER_DCLICK;
					}
					break;
				}
				case wxMOUSE_BTN_RIGHT:
				{
					evtType = click ? wxEVT_HEADER_RIGHT_CLICK : wxEVT_HEADER_RIGHT_DCLICK;
					break;
				}
				case wxMOUSE_BTN_MIDDLE:
				{
					evtType = click ? wxEVT_HEADER_MIDDLE_CLICK : wxEVT_HEADER_MIDDLE_DCLICK;
					break;
				}
			};
		}

		if (evtType != wxEVT_NULL)
		{
			wxHeaderCtrlEvent event(evtType, GetId());
			event.SetEventObject(this);
			event.SetColumn(column->GetIndex());

			if (ProcessWindowEvent(event))
			{
				mevent.Skip(false);
			}
		}
	}
	void HeaderCtrl2::OnKeyDown(wxKeyEvent& event)
	{
		if (event.GetKeyCode() == WXK_ESCAPE)
		{
			if (IsDragging())
			{
				ReleaseMouse();
				CancelDragging();

				return;
			}
		}

		event.Skip();
	}
	void HeaderCtrl2::OnCaptureLost(wxMouseCaptureLostEvent& event)
	{
		if (IsDragging())
		{
			CancelDragging();
		}
	}

	void HeaderCtrl2::UpdateReorderingMarker(int xPhysical)
	{
		wxClientDC dc(this);

		wxDCOverlay dcover(m_DragOverlay, &dc);
		dcover.Clear();

		dc.SetPen(*wxBLUE);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);

		// Draw the phantom position of the column being dragged
		const int x = xPhysical - m_DragOffset;
		const int y = GetClientSize().GetY();
		dc.DrawRectangle(x, 0, m_DraggedColumn->GetWidth(), y);

		// And also a hint indicating where it is going to be inserted if it's dropped now
		if (Column* column = GetColumnClosestToPoint(xPhysical))
		{
			constexpr int DROP_MARKER_WIDTH = 4;

			dc.SetBrush(*wxBLUE);
			dc.DrawRectangle(column->GetRect().GetRight() - DROP_MARKER_WIDTH / 2, 0, DROP_MARKER_WIDTH, y);
		}
	}
	void HeaderCtrl2::ClearMarkers()
	{
		wxClientDC dc(this);

		wxDCOverlay dcover(m_DragOverlay, &dc);
		dcover.Clear();
	}

	void HeaderCtrl2::EndDragging()
	{
		// We currently only use markers for reordering, not for resizing
		if (IsReordering())
		{
			ClearMarkers();
			m_DragOverlay.Reset();
		}

		// Don't use the special dragging cursor any more
		SetCursor(wxNullCursor);
	}
	void HeaderCtrl2::CancelDragging()
	{
		EndDragging();

		Column*& column = IsResizing() ? m_ResizedColumn : m_DraggedColumn;
		if (column)
		{
			wxHeaderCtrlEvent event(wxEVT_HEADER_DRAGGING_CANCELLED, GetId());
			event.SetEventObject(this);
			event.SetColumn(column->GetIndex());
			ProcessWindowEvent(event);

			column = nullptr;
		}
	}

	void HeaderCtrl2::StartReordering(Column& col, int xPhysical)
	{
		wxHeaderCtrlEvent event(wxEVT_HEADER_BEGIN_REORDER, GetId());
		event.SetEventObject(this);
		event.SetColumn(col.GetIndex());

		if (ProcessWindowEvent(event) && !event.IsAllowed())
		{
			// Don't start dragging it, nothing to do otherwise
			return;
		}

		m_DragOffset = xPhysical - col.GetRect().GetX();
		m_DraggedColumn = &col;
		SetCursor(wxCursor(wxCURSOR_HAND));
		CaptureMouse();

		// Do not call UpdateReorderingMarker() here. We don't want to give feedback for reordering until the user starts
		// to really move the mouse as the user might want to just click on the column and not move it at all.
	}
	bool HeaderCtrl2::EndReordering(int xPhysical)
	{
		EndDragging();
		ReleaseMouse();

		const Column* colOld = m_DraggedColumn;
		const Column* colNew = GetColumnClosestToPoint(xPhysical);
		m_DraggedColumn = nullptr;

		if (colNew)
		{
			// Mouse drag must be longer than min distance m_dragOffset
			if (xPhysical - colOld->GetRect().GetX() == m_DragOffset)
			{
				return false;
			}

			if (colNew != colOld)
			{
				wxHeaderCtrlEvent event(wxEVT_HEADER_END_REORDER, GetId());
				event.SetEventObject(this);
				event.SetColumn(colOld->GetIndex());
				event.SetNewOrder(colNew->GetIndex());

				ProcessWindowEvent(event);
			}

			// Whether we moved the column or not, the user did move the mouse and so did try to do it so return true.
			return true;
		}
		return false;
	}

	int HeaderCtrl2::ConstrainByMinWidth(Column& col, int& xPhysical)
	{
		const int xStart = col.GetRect().GetX();

		// Notice that GetMinWidth() returns 0 if there is no minimal width so it
		// still makes sense to use it even in this case
		const int xMinEnd = xStart + col.GetMinWidth();

		if (xPhysical < xMinEnd)
		{
			xPhysical = xMinEnd;
		}
		return xPhysical - xStart;
	}
	void HeaderCtrl2::StartOrContinueResizing(Column& col, int xPhysical)
	{
		wxHeaderCtrlEvent event(IsResizing() ? wxEVT_HEADER_RESIZING : wxEVT_HEADER_BEGIN_RESIZE, GetId());
		event.SetEventObject(this);
		event.SetColumn(col.GetIndex());
		event.SetWidth(ConstrainByMinWidth(col, xPhysical));

		if (ProcessWindowEvent(event) && !event.IsAllowed())
		{
			if (IsResizing())
			{
				ReleaseMouse();
				CancelDragging();
			}
		}
		else
		{
			if (!IsResizing())
			{
				m_ResizedColumn = &col;
				SetCursor(wxCursor(wxCURSOR_SIZEWE));
				CaptureMouse();
			}
		}
	}
	void HeaderCtrl2::EndResizing(int xPhysical)
	{
		EndDragging();
		ReleaseMouse();

		if (m_ResizedColumn)
		{
			wxHeaderCtrlEvent event(wxEVT_HEADER_END_RESIZE, GetId());
			event.SetEventObject(this);
			event.SetColumn(m_ResizedColumn->GetIndex());
			event.SetWidth(ConstrainByMinWidth(*m_ResizedColumn, xPhysical));
			ProcessWindowEvent(event);

			m_ResizedColumn = nullptr;
		}
	}

	void HeaderCtrl2::FinishEditing()
	{
		m_View->GetMainWindow()->EndEdit();
	}
	HeaderCtrl2::EventResult HeaderCtrl2::SendCtrlEvent(ItemEvent& event, const EventID& type, Column* column, std::optional<Rect> rect)
	{
		event.SetEventType(type.AsInt());
		if (rect)
		{
			event.SetRect(*rect);
		}
		m_View->GetMainWindow()->CreateEventTemplate(event, nullptr, column);

		return {m_View->ProcessWindowEvent(event), event.IsAllowed()};
	}

	void HeaderCtrl2::OnClick(wxHeaderCtrlEvent& event)
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
	void HeaderCtrl2::OnRClick(wxHeaderCtrlEvent& event)
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
	void HeaderCtrl2::OnWindowClick(wxMouseEvent& event)
	{
		Point pos = Point(event.GetPosition());
		if (pos.GetX() > m_View->GetMainWindow()->GetRowWidth())
		{
			SendCtrlEvent(event.GetEventType() == wxEVT_LEFT_UP ? ItemEvent::EvtColumnHeaderClick : ItemEvent::EvtColumnHeaderRClick, nullptr);
		}
		event.Skip();
	}
	void HeaderCtrl2::OnSeparatorDClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		Column* column = m_View->GetColumn(event.GetColumn());
		if (column && column->IsSizeable())
		{
			column->FitContent();
		}
	}
	
	void HeaderCtrl2::OnResize(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		if (Column* column = m_View->GetColumn(event.GetColumn()))
		{
			column->AssignWidth(event.GetWidth());
			GetMainWindow()->RefreshDisplay();
		}
	}
	void HeaderCtrl2::OnResizeEnd(wxHeaderCtrlEvent& event)
	{
		GetMainWindow()->UpdateDisplay();
	}
	void HeaderCtrl2::OnReorderEnd(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		Column* column = m_View->GetColumn(event.GetColumn());
		if (column)
		{
			m_View->MoveColumnToPhysicalIndex(*column, event.GetNewOrder());
		}
	}

	wxSize HeaderCtrl2::DoGetBestSize() const
	{
		return FromDIP(wxSize(wxDefaultCoord, 25));
	}
	void HeaderCtrl2::OnInternalIdle()
	{
		wxControl::OnInternalIdle();
		WindowRefreshScheduler::OnInternalIdle();
	}

	Rect HeaderCtrl2::GetDropdownRect(const Column& column) const
	{
		return {};
	}

	bool HeaderCtrl2::Create(View* parent, long style)
	{
		m_View = parent;

		if (wxControl::Create(parent, wxID_NONE, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);
			SetBackgroundStyle(wxBG_STYLE_PAINT);

			// Ctrl events
			Bind(wxEVT_PAINT, &HeaderCtrl2::OnPaint, this);

			Bind(wxEVT_MOTION, &HeaderCtrl2::OnMouse, this);
			Bind(wxEVT_LEFT_UP, &HeaderCtrl2::OnMouse, this);
			Bind(wxEVT_LEFT_DCLICK, &HeaderCtrl2::OnMouse, this);

			Bind(wxEVT_MOUSE_CAPTURE_LOST, &HeaderCtrl2::OnCaptureLost, this);
			Bind(wxEVT_KEY_DOWN, &HeaderCtrl2::OnKeyDown, this);

			// Events
			Bind(wxEVT_HEADER_CLICK, &HeaderCtrl2::OnClick, this);
			Bind(wxEVT_HEADER_RIGHT_CLICK, &HeaderCtrl2::OnRClick, this);
			Bind(wxEVT_HEADER_SEPARATOR_DCLICK, &HeaderCtrl2::OnSeparatorDClick, this);
			Bind(wxEVT_LEFT_UP, &HeaderCtrl2::OnWindowClick, this);
			Bind(wxEVT_RIGHT_UP, &HeaderCtrl2::OnWindowClick, this);

			Bind(wxEVT_HEADER_RESIZING, &HeaderCtrl2::OnResize, this);
			Bind(wxEVT_HEADER_END_RESIZE, &HeaderCtrl2::OnResizeEnd, this);
			Bind(wxEVT_HEADER_END_REORDER, &HeaderCtrl2::OnReorderEnd, this);

			return true;
		}
		return false;
	}

	MainWindow* HeaderCtrl2::GetMainWindow() const
	{
		return m_View ? m_View->GetMainWindow() : nullptr;
	}

	size_t HeaderCtrl2::GetColumnCount() const
	{
		return m_View->GetVisibleColumnCount();
	}
	Column* HeaderCtrl2::GetColumnAt(size_t index) const
	{
		return m_View->GetColumn(index);
	}
	Column* HeaderCtrl2::GetColumnDisplayedAt(size_t index) const
	{
		return m_View->GetColumnPhysicallyDisplayedAt(index);
	}

	void HeaderCtrl2::ToggleSortByColumn(size_t index)
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
	void HeaderCtrl2::ToggleSortByColumn(Column& column)
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

	void HeaderCtrl2::RefreshColumn(Column& column)
	{
		Rect rect = column.GetRect();
		rect.SetHeight(GetClientSize().GetHeight());

		ScheduleRefreshRect(rect);
	}
	void HeaderCtrl2::RefreshColumnsAfter(Column& column)
	{
		Rect rect = column.GetRect();
		rect.SetWidth(GetClientSize().GetWidth() - rect.GetX());

		ScheduleRefreshRect(rect);
	}

	Column* HeaderCtrl2::GetColumnAtPoint(int xPhysical, bool* separator) const
	{
		int xOffset = 0;
		int xLogical = xPhysical - m_ScrollOffset;

		// TODO: Don't hardcode sensitivity
		const int separatorClickMargin = FromDIP(8);

		for (Column* column: m_View->GetColumnsInPhysicalDisplayOrder())
		{
			xOffset += column->GetWidth();

			// If the column is resizable, check if we're approximatively over the line separating it from the next column
			if (column->IsSizeable() && std::abs(xLogical - xOffset) < separatorClickMargin)
			{
				Utility::SetIfNotNull(separator, true);
				return column;
			}

			// If we're inside the column
			if (xLogical < xOffset)
			{
				Utility::SetIfNotNull(separator, false);
				return column;
			}
		}
		return nullptr;
	}
	Column* HeaderCtrl2::GetColumnClosestToPoint(int xPhysical) const
	{
		if (Column* columnAtPoint = GetColumnAtPoint(xPhysical))
		{
			return columnAtPoint;
		}
		else if (HasColumns())
		{
			// We're beyond the last column so return it
			return GetColumnDisplayedAt(GetColumnCount());
		}
		return nullptr;
	}

	void HeaderCtrl2::ScrollWindow(int dx, int dy, const wxRect* rect)
	{
		m_ScrollOffset = dx;
		wxControl::ScrollWindow(dx, dy, rect);
	}
}
