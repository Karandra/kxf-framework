#include "kxf-pch.h"
#include "MainWindow.h"
#include "HeaderCtrl.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/UI/Events/WidgetMouseEvent.h"
#include <wx/generic/private/widthcalc.h>
#include <wx/popupwin.h>
#include <wx/minifram.h>
#include <wx/rawbmp.h>
#include <wx/sysopt.h>

namespace
{
	enum
	{
		// Cell padding on the left/right
		PADDING_RIGHTLEFT = 3,

		// Expander space margin
		EXPANDER_MARGIN = 2,
		EXPANDER_OFFSET = 2,

		RULER_WIDTH = 1
	};
}

namespace kxf::WXUI::DataView
{
	class MaxWidthCalculator final: public wxMaxWidthCalculatorBase
	{
		private:
			const MainWindow& m_MainWindow;
			const DV::Column& m_Column;

			std::shared_ptr<IGraphicsRenderer> m_Renderer;
			std::shared_ptr<IGraphicsContext> m_Context;

			bool m_IsExpanderColumn = false;
			int m_ExpanderSize = 0;

		protected:
			void UpdateWithRow(int row) override
			{
				if (!m_Renderer || row < 0 || static_cast<size_t>(row) > m_MainWindow.m_ItemsCount)
				{
					return;
				}

				if (auto node = m_MainWindow.GetNodeByRow(row))
				{
					int indent = 2 * PADDING_RIGHTLEFT;
					if (m_IsExpanderColumn)
					{
						indent += m_MainWindow.m_Indent * node->GetIndentLevel() + m_ExpanderSize;
					}

					auto renderer = node->GetCellRenderer(m_Column);
					renderer.BeginCellRendering(*node, m_Column, *m_Context);
					renderer.SetupCellDisplayValue();
					renderer.SetupCellAttributes(m_MainWindow.GetCellStateForRow(row));

					Rect cellRect = m_MainWindow.GetItemRect(*node, &m_Column);
					auto&& [desiredSize, contentRect] = renderer.DrawCellContent(cellRect.GetSize(), {});
					UpdateWithWidth(desiredSize.GetWidth() + indent);
				}
			}

		public:
			MaxWidthCalculator(const MainWindow& mainWindow, const DV::Column& column, int expanderSize)
				:wxMaxWidthCalculatorBase(column.GetDisplayIndex()), m_MainWindow(mainWindow), m_Column(column), m_ExpanderSize(expanderSize)
			{
				if (column.IsVisible())
				{
					m_Renderer = mainWindow.GetRenderer();
					m_Context = m_Renderer->CreateLegacyMeasuringContext(mainWindow.GetView());

					m_IsExpanderColumn = m_MainWindow.m_View->GetExpanderColumnOrFirstOne() == &m_Column && !m_MainWindow.IsListLike();
				}
			}
	};
}

namespace kxf::WXUI::DataView
{
	// Events
	void MainWindow::OnChar(wxKeyEvent& event)
	{
		// Propagate the char event upwards
		wxKeyEvent eventForParent(event);
		eventForParent.SetEventObject(m_View);
		if (m_View->ProcessWindowEvent(eventForParent))
		{
			return;
		}
		if (m_View->HandleAsNavigationKey(event))
		{
			return;
		}

		// No item then nothing to do
		if (!HasCurrentRow())
		{
			event.Skip();
			return;
		}

		switch (event.GetKeyCode())
		{
			case WXK_RETURN:
			{
				if (event.HasModifiers())
				{
					event.Skip();
					break;
				}
				else
				{
					// Enter activates the item, i.e. sends 'DataViewWidgetEvent::EvtItemActivated' to it.
					// Only if that event is not handled do we activate column renderer (which
					// is normally done by Space) or even inline editing.
					auto evt = MakeEvent();
					evt.SetNode(GetNodeByRow(m_CurrentRow));

					if (ProcessEvent(evt, DataViewWidgetEvent::EvtItemActivated))
					{
						break;
					}

					// Else fall through to WXK_SPACE handling
					[[fallthrough]];
				}
			}
			case WXK_SPACE:
			{
				if (event.HasModifiers())
				{
					event.Skip();
					break;
				}
				else
				{
					auto node = GetNodeByRow(m_CurrentRow);

					// Activate the current activatable column. If not column is focused (typically
					// because the user has full row selected), try to find the first activatable
					// column (this would typically be a checkbox and we don't want to force the user
					// to set focus on the checkbox column).
					auto* activatableColumn = FindInteractibleColumn(*node, InteractibleCell::Activator);
					if (activatableColumn)
					{
						const Rect cellRect = node->GetCellRect(*activatableColumn);

						auto renderer = node->GetCellRenderer(*activatableColumn);
						renderer.BeginCellSetup(*node, *activatableColumn);
						renderer.SetupCellAttributes(GetCellStateForRow(m_CurrentRow));
						renderer.OnActivateCell(*node, cellRect, nullptr);

						break;
					}
					break;
				}
			}
			case WXK_F2:
			{
				if (event.HasModifiers())
				{
					event.Skip();
					break;
				}
				else
				{
					if (!m_SelectionStore.IsEmpty())
					{
						// Mimic Windows 7 behavior: edit the item that has focus
						// if it is selected and the first selected item if focus
						// is out of selection.
						DV::Row selectedRow;
						if (m_SelectionStore.IsSelected(*m_CurrentRow))
						{
							selectedRow = m_CurrentRow;
						}
						else
						{
							// Focused item is not selected.
							wxSelectionStore::IterationState cookie;
							selectedRow = m_SelectionStore.GetFirstSelectedItem(cookie);
						}
						auto node = GetNodeByRow(selectedRow);

						// Edit the current column. If no column is focused (typically because the user has full row selected),
						// try to find the first editable column.
						if (auto editableColumn = FindInteractibleColumn(*node, InteractibleCell::Editor))
						{
							node->EditCell(*editableColumn);
						}
					}
				}
				break;
			}
			case WXK_UP:
			{
				OnVerticalNavigation(event, -1);
				break;
			}
			case WXK_DOWN:
			{
				OnVerticalNavigation(event, +1);
				break;
			}
			case WXK_LEFT:
			{
				OnLeftKey(event);
				break;
			}
			case WXK_RIGHT:
			{
				OnRightKey(event);
				break;
			}
			case WXK_END:
			{
				OnVerticalNavigation(event, +(int)GetRowCount());
				break;
			}
			case WXK_HOME:
			{
				OnVerticalNavigation(event, -(int)GetRowCount());
				break;
			}
			case WXK_PAGEUP:
			{
				OnVerticalNavigation(event, -(int)(GetCountPerPage() - 1));
				break;
			}
			case WXK_PAGEDOWN:
			{
				OnVerticalNavigation(event, +(int)(GetCountPerPage() - 1));
				break;
			}
			default:
			{
				event.Skip();
			}
		};
	}
	void MainWindow::OnCharHook(wxKeyEvent& event)
	{
		if (m_CurrentEditor)
		{
			// Handle any keys special for the in-place editor and return without calling Skip() below.
			switch (event.GetKeyCode())
			{
				case WXK_ESCAPE:
				{
					CancelEdit();
					return;
				}
				case WXK_RETURN:
				{
					// Shift-Enter is not special neither.
					if (event.ShiftDown())
					{
						break;
					}
					[[fallthrough]];
				}
				case WXK_TAB:
				{
					// Ctrl/Alt-Tab or Enter could be used for something else, so don't handle them here.
					if (event.HasModifiers())
					{
						break;
					}

					EndEdit();
					return;
				}
			};
		}
		else if (m_UseCellFocus)
		{
			if (event.GetKeyCode() == WXK_TAB && !event.HasModifiers())
			{
				event.ShiftDown() ? OnLeftKey(event) : OnRightKey(event);
			}
		}
		event.Skip();
	}
	void MainWindow::OnVerticalNavigation(const wxKeyEvent& event, int delta)
	{
		// If there is no selection, we cannot move it anywhere
		if (!HasCurrentRow() || IsEmpty())
		{
			return;
		}

		// Let's keep the new row inside the allowed range
		intptr_t newRow = static_cast<intptr_t>(*m_CurrentRow) + delta;
		if (newRow < 0)
		{
			newRow = 0;
		}

		size_t rowCount = GetRowCount();
		if (newRow >= (intptr_t)rowCount)
		{
			newRow = rowCount - 1;
		}

		DV::Row oldCurrent = m_CurrentRow;
		DV::Row newCurrent = newRow;
		if (newCurrent == oldCurrent)
		{
			return;
		}

		// In single selection we just ignore Shift as we can't select several items anyhow
		if (event.ShiftDown() && !IsSingleSelection())
		{
			RefreshRow(oldCurrent);
			ChangeCurrentRow(newCurrent);

			// Select all the items between the old and the new one
			if (*oldCurrent > *newCurrent)
			{
				newCurrent = oldCurrent;
				oldCurrent = m_CurrentRow;
			}
			SelectRows(oldCurrent, newCurrent);

			wxSelectionStore::IterationState cookie;
			auto firstSel = m_SelectionStore.GetFirstSelectedItem(cookie);
			if (firstSel != wxSelectionStore::NO_SELECTION)
			{
				SendSelectionChangedEvent(GetNodeByRow(firstSel), m_CurrentColumn);
			}
		}
		else
		{
			RefreshRow(oldCurrent);

			// All previously selected items are unselected unless ctrl is held
			if (!event.ControlDown())
			{
				UnselectAllRows();
			}
			ChangeCurrentRow(newCurrent);

			if (!event.ControlDown())
			{
				SelectRow(m_CurrentRow, true);
				SendSelectionChangedEvent(GetNodeByRow(m_CurrentRow), m_CurrentColumn);
			}
			else
			{
				RefreshRow(m_CurrentRow);
			}
		}
		EnsureVisible(m_CurrentRow);
	}
	void MainWindow::OnLeftKey(wxKeyEvent& event)
	{
		if (IsListLike())
		{
			TryAdvanceCurrentColumn(nullptr, event, false);
		}
		else
		{
			auto node = GetNodeByRow(m_CurrentRow);
			if (!node)
			{
				return;
			}
			if (TryAdvanceCurrentColumn(node, event, false))
			{
				return;
			}

			const bool dontCollapseNodes = event.GetKeyCode() == WXK_TAB;
			if (dontCollapseNodes)
			{
				m_CurrentColumn = nullptr;

				// Allow focus change
				event.Skip();
				return;
			}

			// Because TryAdvanceCurrentColumn() return false, we are at the first
			// column or using whole-row selection. In this situation, we can use
			// the standard TreeView handling of the left key.
			if (node->HasChildren() && node->IsExpanded())
			{
				Collapse(m_CurrentRow);
			}
			else
			{
				// If the node is already closed, we move the selection to its parent
				if (auto parentNode = node->GetParentNode())
				{
					if (auto parentRow = GetRowByNode(*parentNode))
					{
						SelectRow(m_CurrentRow, false);
						SelectRow(parentRow, true);
						ChangeCurrentRow(parentRow);
						EnsureVisible(parentRow);
						SendSelectionChangedEvent(parentNode, m_CurrentColumn);
					}
				}
			}
		}
	}
	void MainWindow::OnRightKey(wxKeyEvent& event)
	{
		if (IsListLike())
		{
			TryAdvanceCurrentColumn(nullptr, event, true);
		}
		else if (auto node = GetNodeByRow(m_CurrentRow))
		{
			if (node->HasChildren() && m_CurrentColumn && m_CurrentColumn->IsExpander())
			{
				if (!node->IsExpanded())
				{
					Expand(m_CurrentRow);
				}
				else
				{
					// If the node is already open, we move the selection to the first child
					SelectRow(m_CurrentRow, false);
					SelectRow(m_CurrentRow + 1, true);
					ChangeCurrentRow(m_CurrentRow + 1);
					EnsureVisible(m_CurrentRow + 1);
					SendSelectionChangedEvent(GetNodeByRow(m_CurrentRow + 1), m_CurrentColumn);
				}
				return;
			}
			TryAdvanceCurrentColumn(node, event, true);
		}
	}

	void MainWindow::OnMouse(wxMouseEvent& event)
	{
		auto ResetHotTrackedExpander = [this]()
		{
			if (m_TreeNodeUnderMouse)
			{
				auto row = GetRowByNode(*m_TreeNodeUnderMouse);
				m_TreeNodeUnderMouse = nullptr;
				RefreshRow(row);
			}
		};

		const wxEventType eventType = event.GetEventType();
		if (eventType == wxEVT_LEAVE_WINDOW || eventType == wxEVT_ENTER_WINDOW)
		{
			ResetHotTrackedExpander();

			event.Skip();
			return;
		}

		const auto headerOffset = GetHeaderOffset();
		const bool isMouseInsideWindow = IsMouseInWindow({event.GetX(), event.GetY()});
		if (eventType == wxEVT_MOUSEWHEEL)
		{
			if (isMouseInsideWindow)
			{
				// Adjust Y mouse coordinate when vertical scrolling happened.
				// If we don't do this, row a few lines above or below will be tracked
				// and not the one, that should be tracked.
				if (event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
				{
					const bool shouldScrollHorizontal = event.ShiftDown();
					const int scrollPos = m_View->GetScrollPos(wxVERTICAL);

					if (shouldScrollHorizontal || (scrollPos > 0 && scrollPos + m_View->GetScrollPageSize(wxVERTICAL) < m_View->GetScrollLines(wxVERTICAL)))
					{
						int rateX = 0;
						int rateY = 0;
						m_View->GetScrollPixelsPerUnit(&rateX, &rateY);

						// Invert modifier if we scrolling up
						const bool isScrollDown = event.GetWheelRotation() < 0;
						if (!isScrollDown)
						{
							rateY = -rateY;
						}

						if (shouldScrollHorizontal)
						{
							// TODO: Implement horizontal scrolling for 'Shift + Wheel' combo
							//Point pos = {m_View->GetScrollPos(wxVERTICAL), m_View->GetScrollPos(wxHORIZONTAL)};
							//m_View->Scroll(pos.GetX() + rateX, pos.GetY());
						}
						else
						{
							event.SetY(event.GetY() + event.GetLinesPerAction() * rateY);
						}
					}
				}
			}
			event.Skip();
		}

		if (event.ButtonDown())
		{
			// Not skipping button down events would prevent the system from
			// setting focus to this window as most (all?) of them do by default,
			// so skip it to enable default handling.
			event.Skip();
		}

		int x = event.GetX();
		int y = event.GetY();
		m_View->CalcUnscrolledPosition(x, y, &x, &y);
		y -= headerOffset;

		DV::Column* currentColumn = nullptr;

		int xpos = 0;
		const size_t columnsCount = m_View->GetColumnCount();
		for (size_t i = 0; i < columnsCount; i++)
		{
			DV::Column* column = m_View->GetColumnDisplayedAt(i);

			int width = 0;
			if (column->IsExposed(width))
			{
				if (x < xpos + width)
				{
					currentColumn = column;
					break;
				}
				xpos += width;
			}
		}

		const auto currentRow = GetRowAt(y);
		auto const currentNode = GetNodeByRow(currentRow);

		// Hot track
		if (isMouseInsideWindow)
		{
			const bool rowChnaged = (m_HotTrackRow != currentRow && currentNode) || (m_HotTrackRow && !currentNode);
			const bool columnChanged = m_HotTrackColumn != currentColumn;

			if (rowChnaged || columnChanged)
			{
				m_HotTrackColumn = currentColumn;

				// Refresh old hot-tracked row
				if (rowChnaged)
				{
					m_HotTrackRowEnabled = false;
					RefreshRow(m_HotTrackRow);
				}
				if (!currentNode)
				{
					ResetHotTrackedExpander();
				}

				// Update new row
				m_HotTrackRow = currentNode ? currentRow : DV::Row();
				m_HotTrackRowEnabled = m_HotTrackRow && m_HotTrackColumn;
				RefreshRow(m_HotTrackRow);

				auto hoverEvent = MakeEvent();
				hoverEvent.SetNode(currentNode);
				hoverEvent.SetColumn(m_HotTrackColumn);
				ProcessEvent(hoverEvent, DataViewWidgetEvent::EvtItemHovered);

				// Show tooltip
				RemoveTooltip();
				if (m_HotTrackRow && m_HotTrackColumn)
				{
					m_ToolTipTimer.StartOnce(System::GetMetric(SystemTimeMetric::DClick).GetMilliseconds());
				}
			}
		}

		// Handle right clicking here, before everything else as context menu events should be
		// sent even when we click outside of any item, unlike all the other ones.
		if (event.RightUp())
		{
			CancelEdit();

			auto evt = MakeEvent();
			evt.SetNode(currentNode);
			evt.SetColumn(currentColumn);

			ProcessEvent(evt, DataViewWidgetEvent::EvtItemContextMenu);
			return;
		}

		// Drag and drop
		/*
		if (event.Dragging() || ((m_DragCount > 0) && event.Leaving()))
		{
			if (m_DragCount == 0)
			{
				// We have to report the raw, physical coords as we want to be able to
				// call HitTest(event.m_pointDrag) from the user code to get the item being dragged.
				m_DragStart = Point(event.GetPosition());
			}

			m_DragCount++;
			if (m_DragCount < 3 && event.Leaving())
			{
				m_DragCount = 3;
			}
			else if (m_DragCount != 3)
			{
				return;
			}

			if (event.LeftIsDown())
			{
				m_View->CalcUnscrolledPosition(m_DragStart.GetX(), m_DragStart.GetY(), &m_DragStart.X(), &m_DragStart.Y());
				const Row draggedRow = GetRowAt(m_DragStart.GetY());
				Node* const draggedNode = GetNodeByRow(draggedRow);

				// Don't allow invalid items
				if (draggedNode)
				{
					RemoveTooltip();

					// Notify cell about drag
					DragDropEvent dragEvent(DragDropEvent::EvtItemDrag);
					CreateEventTemplate(dragEvent, draggedNode, currentColumn);
					if (!m_View->HandleWindowEvent(dragEvent) || !dragEvent.IsAllowed())
					{
						return;
					}

					if (wxDataObjectSimple* dragObject = dragEvent.GetDataObject())
					{
						DropSource dragSource(this, draggedRow);
						dragSource.SetData(*dragObject);

						m_DragSource = &dragSource;
						dragSource.DoDragDrop(dragEvent.GetDragFlags());
						m_DragSource = nullptr;
					}
				}
			}
			return;
		}
		else
		{
			m_DragCount = 0;
		}
		*/

		// Check if we clicked outside the item area.
		if (currentRow >= GetRowCount() || !currentNode || !currentColumn)
		{
			// Follow Windows convention here - clicking either left or right (but not middle) button clears the existing selection.
			if (m_View && (event.LeftDown() || event.RightDown()))
			{
				if (!m_SelectionStore.IsEmpty())
				{
					m_View->UnselectAll();
					SendSelectionChangedEvent(nullptr, currentColumn);
				}
			}

			event.Skip();
			return;
		}

		auto TestExpanderButton = [&](DV::Row row, int itemOffset = -1, const DV::Node* node = nullptr)
		{
			if (itemOffset < 0)
			{
				itemOffset = m_Indent * node->GetIndentLevel();
			}

			Rect rect(xpos + itemOffset, GetRowStart(row) + (GetRowHeight(row) - m_UniformRowHeight) / 2, m_UniformRowHeight, m_UniformRowHeight);
			return rect.Contains(x, y);
		};

		// Test whether the mouse is hovering over the expander (a.k.a tree "+" button)
		// and also determine the offset of the real cell start, skipping the indentation and the expander itself.
		auto expander = m_View->GetExpanderColumnOrFirstOne();
		bool isHoverOverExpander = false;
		int itemOffset = 0;
		if (expander == currentColumn && !IsListLike())
		{
			itemOffset = m_Indent * currentNode->GetIndentLevel();

			// We make the rectangle we are looking in a bit bigger than the actual
			// visual expander so the user can hit that little thing reliably.
			if (currentNode->HasChildren() && TestExpanderButton(currentRow, itemOffset))
			{
				// So the mouse is over the expander
				if (m_TreeNodeUnderMouse && m_TreeNodeUnderMouse != currentNode)
				{
					RefreshRow(GetRowByNode(*m_TreeNodeUnderMouse));
				}
				if (m_TreeNodeUnderMouse != currentNode)
				{
					RefreshRow(currentRow);
				}

				m_TreeNodeUnderMouse = currentNode;
				isHoverOverExpander = true;
			}

			// Account for the expander as well, even if this item doesn't have it,
			// its parent does so it still counts for the offset.
			itemOffset += m_UniformRowHeight;
		}

		if (!isHoverOverExpander && m_TreeNodeUnderMouse)
		{
			auto row = GetRowByNode(*m_TreeNodeUnderMouse);
			m_TreeNodeUnderMouse = nullptr;
			RefreshRow(row);
		}

		bool simulateClick = false;
		bool ignoreOtherColumns = expander != currentColumn && currentNode->HasChildren();
		if (event.ButtonDClick())
		{
			m_LastOnSame = false;
		}

		if (event.LeftDClick())
		{
			if (!isHoverOverExpander && (currentRow == m_RowLastClicked))
			{
				auto evt = MakeEvent();
				evt.SetNode(currentNode);
				evt.SetColumn(currentColumn);
				if (ProcessEvent(evt, DataViewWidgetEvent::EvtItemActivated))
				{
					// Item activation was handled from the user code.
					return;
				}
			}

			// Either it was a double click over the expander, or the second click
			// happened on another item than the first one or it was a bona fide
			// double click which was unhandled. In all these cases we continue
			// processing this event as a simple click, e.g. to select the item or
			// activate the renderer.
			simulateClick = true;
		}

		if (event.LeftUp() && !isHoverOverExpander)
		{
			if (m_RowSelectSingleOnUp)
			{
				// Select single line
				if (UnselectAllRows(m_RowSelectSingleOnUp))
				{
					SelectRow(m_RowSelectSingleOnUp, true);
					SendSelectionChangedEvent(GetNodeByRow(m_RowSelectSingleOnUp), currentColumn);
				}
				else if (m_View->m_Style.Contains(DV::WidgetStyle::CellFocus))
				{
					RefreshRow(currentRow);
				}
				// Else it was already selected, nothing to do.
			}

			m_LastOnSame = false;
			m_RowSelectSingleOnUp = {};
		}
		else if (!event.LeftUp())
		{
			// This is necessary, because after a DnD operation in from and to ourself, the up event is swallowed by the
			// DnD code. So on next non-up event (which means here and now) 'm_RowSelectSingleOnUp' should be reset.
			m_RowSelectSingleOnUp = {};
		}

		if (event.RightDown())
		{
			m_RowBeforeLastClicked = m_RowLastClicked;
			m_RowLastClicked = currentRow;

			// If the item is already selected, do not update the selection.
			// Multi-selections should not be cleared if a selected item is clicked.
			if (!IsRowSelected(currentRow))
			{
				UnselectAllRows();

				const auto oldCurrent = m_CurrentRow;
				ChangeCurrentRow(currentRow);
				SelectRow(m_CurrentRow, true);
				RefreshRow(oldCurrent);
				SendSelectionChangedEvent(GetNodeByRow(m_CurrentRow), currentColumn);
			}
		}
		//else if (event.MiddleDown())
		//{
		//}


		if ((event.LeftDown() || simulateClick) && isHoverOverExpander && !event.LeftDClick())
		{
			// 'isHoverOverExpander' being true tells us that our node must be valid and have children. So we don't need any extra checks.
			if (currentNode->IsExpanded())
			{
				Collapse(currentRow);
			}
			else
			{
				Expand(currentRow);
			}
		}
		else if ((event.LeftDown() || simulateClick) && !isHoverOverExpander)
		{
			m_RowBeforeLastClicked = m_RowLastClicked;
			m_RowLastClicked = currentRow;

			auto oldCurrentRow = m_CurrentRow;
			auto oldCurrentCol = m_CurrentColumn;
			bool oldWasSelected = IsRowSelected(m_CurrentRow);

			bool cmdModifierDown = event.CmdDown();
			if (IsSingleSelection() || !(cmdModifierDown || event.ShiftDown()))
			{
				if (IsSingleSelection() || !IsRowSelected(currentRow))
				{
					ChangeCurrentRow(currentRow);
					if (UnselectAllRows(currentRow))
					{
						SelectRow(m_CurrentRow, true);
						SendSelectionChangedEvent(GetNodeByRow(m_CurrentRow), currentColumn);
					}
				}
				else
				{
					// Multi selection & current is highlighted & no mod keys
					m_RowSelectSingleOnUp = currentRow;
					ChangeCurrentRow(currentRow); // change focus
				}
			}
			else
			{
				// Multi selection & either ctrl or shift is down
				if (cmdModifierDown)
				{
					ChangeCurrentRow(currentRow);
					ReverseRowSelection(m_CurrentRow);
					SendSelectionChangedEvent(GetNodeByRow(m_CurrentRow), currentColumn);
				}
				else if (event.ShiftDown())
				{
					ChangeCurrentRow(currentRow);

					auto lineFrom = oldCurrentRow;
					auto lineTo = currentRow;

					if (!lineFrom)
					{
						// If we hadn't had any current row before, treat this as a simple click and select the new row only.
						lineFrom = currentRow;
					}

					if (lineTo < lineFrom)
					{
						lineTo = lineFrom;
						lineFrom = m_CurrentRow;
					}

					SelectRows(lineFrom, lineTo);

					wxSelectionStore::IterationState cookie;
					auto firstSel = m_SelectionStore.GetFirstSelectedItem(cookie);
					if (firstSel != wxSelectionStore::NO_SELECTION)
					{
						SendSelectionChangedEvent(GetNodeByRow(firstSel), currentColumn);
					}
				}
				else
				{
					// !ctrl, !shift
					// Test in the enclosing if should make it impossible
					wxFAIL_MSG(wxT("how did we get here?"));
				}
			}

			if (m_CurrentRow != oldCurrentRow)
			{
				RefreshRow(oldCurrentRow);
			}
			if (oldCurrentCol != currentColumn && m_View->m_Style.Contains(DV::WidgetStyle::CellFocus))
			{
				RefreshRow(currentRow);
			}

			// Update selection here...
			m_CurrentColumn = currentColumn;
			m_IsCurrentColumnSetByKeyboard = false;

			// This flag is used to decide whether we should start editing the item
			// label. We do it if the user clicks twice (but not double clicks,
			// i.e. simulateClick is false) on the same item but not if the click
			// was used for something else already, e.g. selecting the item (so it
			// must have been already selected) or giving the focus to the control
			// (so it must have had focus already).
			m_LastOnSame = !simulateClick && ((currentColumn == oldCurrentCol) &&	(currentRow == oldCurrentRow)) && oldWasSelected && m_View->HasFocus();

			// Call OnActivateCell() after everything else as under GTK+
			if (IsCellInteractible(*currentNode, *currentColumn, InteractibleCell::Activator))
			{
				// Notify cell about click
				Rect cellRect(xpos + itemOffset, GetRowStart(currentRow), currentColumn->GetWidth() - itemOffset, GetRowHeight(currentRow));
				auto rowState = GetCellStateForRow(oldCurrentRow);

				// Note that SetupCellAttributes() should be called after GetRowStart()
				// call in 'cellRect' initialization above as GetRowStart() calls
				// SetupCellAttributes() for other items from inside it.
				auto renderer = currentNode->GetCellRenderer(*currentColumn);
				renderer.BeginCellSetup(*currentNode, *currentColumn);
				renderer.SetupCellAttributes(rowState);
				renderer.SetupCellDisplayValue();

				// Report position relative to the cell's custom area, i.e.
				// not the entire space as given by the control but the one
				// used by the renderer after calculation of alignment etc.
				//
				// Notice that this results in negative coordinates when clicking
				// in the upper left corner of a center-aligned cell which doesn't
				// fill its column entirely so this is somewhat surprising, but we
				// do it like this for compatibility with the native GTK+ version,
				// see #12270.

				// 'DrawCellContent' won't draw anything in this case but will calculate the required rect.
				auto [size, itemRect] = renderer.DrawCellContent(cellRect, rowState);

				wxMouseEvent mouseEventWX(event);
				mouseEventWX.m_x -= itemRect.GetX();
				mouseEventWX.m_y -= itemRect.GetY();
				m_View->CalcUnscrolledPosition(mouseEventWX.m_x, mouseEventWX.m_y, &mouseEventWX.m_x, &mouseEventWX.m_y);

				WidgetMouseEvent mouseEvent(m_View->m_Widget, mouseEventWX);
				renderer.OnActivateCell(*currentNode, cellRect, &mouseEvent);
			}
		}
	}
	void MainWindow::OnFocusSet(wxFocusEvent& event)
	{
		m_HasFocus = true;

		// Make the control usable from keyboard once it gets focus by ensuring
		// that it has a current row, if at all possible.
		if (!HasCurrentRow() && !IsEmpty())
		{
			ChangeCurrentRow(0);
		}
		if (HasCurrentRow())
		{
			m_View->ScheduleRefresh();
		}
		event.Skip();
	}
	void MainWindow::OnFocusLost(wxFocusEvent& event)
	{
		m_HasFocus = false;

		if (HasCurrentRow())
		{
			m_View->ScheduleRefresh();
		}
		event.Skip();
	}

	void MainWindow::OnTooltipEvent(wxTimerEvent& event)
	{
		RemoveTooltip();

		if (m_HotTrackColumn)
		{
			if (auto node = GetNodeByRow(m_HotTrackRow))
			{
				ShowToolTip(*node, *m_HotTrackColumn);
			}
		}
	}

	bool MainWindow::SendExpanderEvent(const EventID& type, DV::Node& item)
	{
		auto event = MakeEvent();
		event.SetNode(&item);

		return !ProcessEvent(event, type) || event.IsAllowed();
	}
	void MainWindow::SendSelectionChangedEvent(DV::Node* item, DV::Column* column)
	{
		if (item)
		{
			RefreshRow(GetRowByNode(*item));
		}

		auto event = MakeEvent();
		event.SetNode(item);
		event.SetColumn(column);

		ProcessEvent(event, DataViewWidgetEvent::EvtItemSelected);
	}
	bool MainWindow::SendEditingStartedEvent(DV::Node& item, DV::Column& column)
	{
		auto event = MakeEvent();
		event.SetNode(&item);
		event.SetColumn(&column);

		ProcessEvent(event, DataViewWidgetEvent::EvtItemEditStarted);
		return event.IsAllowed();
	}
	bool MainWindow::SendEditingDoneEvent(DV::Node& item, DV::Column& column, bool canceled, const Any& value)
	{
		auto event = MakeEvent();
		event.SetNode(&item);
		event.SetColumn(&column);
		event.SetEditCanceled(canceled);
		event.SetValue(value);

		ProcessEvent(event, DataViewWidgetEvent::EvtItemEditFinished);
		return event.IsAllowed();
	}

	// Drawing
	void MainWindow::OnPaint(wxPaintEvent& event)
	{
		auto renderer = GetRenderer();
		auto penRuleH = renderer->CreatePen(System::GetColor(SystemColor::Light3D), RULER_WIDTH);
		auto penRuleV = penRuleH;

		auto gc = renderer->CreateLegacyWindowPaintContext(*m_View);
		gc->SetAntialiasMode(AntialiasMode::None);
		gc->SetTextAntialiasMode(AntialiasMode::Default);
		gc->SetInterpolationQuality(InterpolationQuality::None);

		const auto headerOffset = GetHeaderOffset();
		const auto clientRect = GetClientRect();
		const auto transparentPen = renderer->CreatePen(Drawing::GetStockColor(StockColor::Transparent));
		const auto transparentBrush = renderer->CreateSolidBrush(Drawing::GetStockColor(StockColor::Transparent));
		const auto backgroundBrush = renderer->CreateSolidBrush(m_View->GetBackgroundColour());
		gc->Clear(*backgroundBrush);

		// Draw the top border
		if (auto header = m_View->m_HeaderArea; header && header->IsShown())
		{
			auto brush = renderer->CreateSolidBrush(penRuleH->GetColor());

			auto rect = Rect(header->GetRect());
			rect.Height() += header->FromDIP(m_View->m_HeaderBorder);
			gc->DrawRectangle(rect, *brush);
		}

		gc->SetPen(transparentPen);
		gc->SetBrush(backgroundBrush);
		gc->OffsetForScrollableArea(*m_View);
		gc->TransformTranslate(0, headerOffset);

		const size_t columnCount = m_View->GetColumnCount();
		if (IsEmpty() || columnCount == 0)
		{
			auto label = m_View->m_Widget.GetWidgetText();
			if (!label.IsEmpty())
			{
				const int y = m_View->GetCharHeight() * 2;
				const Rect rect = {clientRect.GetX(), y, clientRect.GetWidth(), clientRect.GetHeight() - y};

				auto brush = renderer->CreateSolidBrush(m_View->GetForegroundColour().MakeDisabled());
				auto font = renderer->CreateFont(m_View->GetFont());
				gc->DrawLabel(label, rect, *font, *brush, Alignment::CenterHorizontal);
			}

			// We assume that we have at least one column below and painting an empty control is unnecessary anyhow
			return;
		}

		Rect updateRect = Rect(m_View->GetUpdateRegion().GetBox());
		m_View->CalcUnscrolledPosition(updateRect.GetX(), updateRect.GetY(), &updateRect.X(), &updateRect.Y());

		// Undo header offset from update rect
		updateRect.MoveTopBy(-headerOffset);

		// Compute which rows needs to be redrawn
		const DV::Row rowStart = GetRowAt(std::max(0, updateRect.GetY()));
		const size_t rowCount = std::min(*GetRowAt(std::max(0, updateRect.GetBottom())) - *rowStart + 1, GetRowCount() - *rowStart);
		const DV::Row rowEnd = rowStart + rowCount;

		// Send the event to the control itself
		{
			DataViewWidgetEvent cacheEvent(m_View->m_Widget);
			cacheEvent.SetCacheHints(rowStart, rowEnd - 1);
			m_View->m_Widget.ProcessEvent(cacheEvent, DataViewWidgetEvent::EvtViewCacheHint);
		}

		// Compute which columns needs to be redrawn. Calc start of X coordinate.
		size_t coulumnIndexStart = 0;
		int xCoordStart = 0;
		for (coulumnIndexStart = 0; coulumnIndexStart < columnCount; coulumnIndexStart++)
		{
			const DV::Column* column = m_View->GetColumnDisplayedAt(coulumnIndexStart);

			int width = 0;
			if (column->IsExposed(width))
			{
				if (xCoordStart + width >= updateRect.GetX())
				{
					break;
				}
				xCoordStart += width;
			}
		}

		// Calc end of X coordinate and visible columns count
		size_t visibleColumnsCount = 0;
		size_t coulmnIndexEnd = coulumnIndexStart;
		int xCoordEnd = xCoordStart;
		const int fullRowWidth = GetRowWidth();

		for (; coulmnIndexEnd < columnCount; coulmnIndexEnd++)
		{
			const DV::Column* column = m_View->GetColumnDisplayedAt(coulmnIndexEnd);

			int width = 0;
			if (column->IsExposed(width))
			{
				visibleColumnsCount++;

				if (xCoordEnd > updateRect.GetRight())
				{
					// If we drawing only part of the control, draw it one pixel wider, to hide not drawn regions.
					if (xCoordEnd + width < fullRowWidth)
					{
						xCoordEnd++;
					}
					break;
				}
				xCoordEnd += width;
			}
		}

		const bool isListLike = IsListLike();
		const bool isCellFocusEnabled = m_View->m_Style.Contains(DV::WidgetStyle::CellFocus);
		const bool fullRowSelectionEnabled = m_View->m_Style.Contains(DV::WidgetStyle::FullRowSelection);
		const bool alternatingRowColorsEnabled = m_View->m_Style.Contains(DV::WidgetStyle::AlternatingRowColors);
		const bool verticalRulesEnabled = m_View->m_Style.Contains(DV::WidgetStyle::VerticalRules);
		const bool horizontalRulesEnabled = m_View->m_Style.Contains(DV::WidgetStyle::HorizontalRules);

		// Redraw all cells for all rows which must be repainted and all columns
		auto& nativeRenderer = IRendererNative::Get();

		Color altRowColor = m_View->m_AlternateRowColor;
		std::shared_ptr<IGraphicsPen> altRowPen;
		std::shared_ptr<IGraphicsSolidBrush> altRowBrush;

		const DV::Column* const expanderColumn = m_View->GetExpanderColumnOrFirstOne();
		for (DV::Row currentRow = rowStart; currentRow < rowEnd; ++currentRow)
		{
			const DV::Node* const node = GetNodeByRow(currentRow);
			if (!node)
			{
				continue;
			}

			// Setup some values first
			const Rect cellInitialRect = {xCoordStart, GetRowStart(currentRow), 0, GetRowHeight(currentRow)};
			Rect cellRect = cellInitialRect;
			const DV::CellState cellState = GetCellStateForRow(currentRow);

			bool isCategoryRow = false;
			int categoryRowOffset = -1;

			int expanderIndent = 0;
			Rect expanderRect;

			auto GetRowRect = [&](bool offsetByExpander)
			{
				Rect rowRect = cellInitialRect;
				rowRect.SetWidth(xCoordEnd - xCoordStart);
				if (offsetByExpander && expanderColumn->IsDisplayedFirst())
				{
					rowRect.X() += expanderIndent;
					rowRect.Width() -= expanderIndent;
				}
				return rowRect;
			};

			// Draw background of alternate rows specially if required
			if (alternatingRowColorsEnabled)
			{
				if (!altRowColor)
				{
					// Determine the alternate rows color automatically from the background color.
					const Color bgColor = m_View->GetBackgroundColour();

					// Depending on the background, alternate row color will be 3% more dark or 50% brighter.
					constexpr auto middlePoint = Drawing::GetStockColor(StockColor::Gray).GetRGBA();
					Angle alpha = bgColor.GetRGBA() > middlePoint ? Angle::FromNormalized(-0.03f) : Angle::FromNormalized(+0.5f);
					altRowColor = bgColor.ChangeLightness(alpha);
				}
				if (!altRowPen || !altRowBrush)
				{
					altRowPen = renderer->CreatePen(altRowColor);
					altRowBrush = renderer->CreateSolidBrush(altRowColor);
				}

				// We only need to draw the visible part, so limit the rectangle to it.
				//const int x = m_View->CalcUnscrolledPosition(Point(0, 0)).x;
				if (*currentRow % 2)
				{
					gc->DrawRectangle(GetRowRect(false), *altRowBrush, *altRowPen);
				}
			}

			for (size_t currentColumnIndex = coulumnIndexStart; currentColumnIndex < coulmnIndexEnd; currentColumnIndex++)
			{
				DV::Column* column = m_View->GetColumnDisplayedAt(currentColumnIndex);
				if (!column->IsExposed(cellRect.Width()))
				{
					continue;
				}

				auto cellRenderer = node->GetCellRenderer(*column);
				cellRenderer.BeginCellRendering(*node, *column, *gc);
				auto& cellAttributes = cellRenderer.SetupCellAttributes(cellState);

				// Calculate expander button rect and its indent
				const bool isCategoryRow = cellAttributes.Options().ContainsOption(DV::CellStyle::Category);
				if (column == expanderColumn && !isListLike)
				{
					// Calculate the indent first
					const int indentOffset = m_Indent * node->GetIndentLevel();
					expanderIndent = std::min(indentOffset + m_UniformRowHeight, expanderColumn->GetWidth()) + EXPANDER_MARGIN;

					if (node->HasChildren())
					{
						auto ClacExpanderRect = [&expanderRect, &cellRect, indentOffset](const Size& size, int marginLeft = 0, int offsetTop = 0)
						{
							// We reserve 'm_UniformRowHeight' of horizontal space for the expander but leave EXPANDER_MARGIN around the expander itself
							expanderRect.SetX(cellRect.GetX() + indentOffset + marginLeft);
							expanderRect.SetY(cellRect.GetY() + (cellRect.GetHeight() - size.GetHeight()) / 2 + marginLeft - offsetTop);
							expanderRect.SetWidth(size.GetWidth());
							expanderRect.SetHeight(size.GetHeight());
						};

						if (isCategoryRow)
						{
							ClacExpanderRect(nativeRenderer.GetCollapseButtonSize(m_View) - Size(EXPANDER_MARGIN, EXPANDER_MARGIN).Scale(2), EXPANDER_MARGIN);
						}
						else
						{
							ClacExpanderRect(Size(m_UniformRowHeight, m_UniformRowHeight), EXPANDER_MARGIN, EXPANDER_OFFSET);
						}
					}
				}

				// Calc focus rect
				Rect focusCellRect = [&]()
				{
					Rect result = cellRect;
					result.Width() -= 1;
					result.Height() -= 1;

					const bool first = column->IsDisplayedFirst();
					if (column == expanderColumn && first)
					{
						result.X() += expanderIndent;
						result.Width() -= expanderIndent;

						if (!verticalRulesEnabled)
						{
							result.Width() += 1;
						}
					}
					if (!first)
					{
						result.X() -= 1;
						if (column->IsDisplayedLast())
						{
							result.Width() += 1;
						}
						else
						{
							result.Width() += verticalRulesEnabled ? 1 : 2;
						}
					}

					return result.Deflate(Size(m_View->FromDIP(wxSize(1, 1))));
				}();

				// Adjust cell rectangle
				Rect adjustedCellRect = cellRect;
				adjustedCellRect.Deflate(PADDING_RIGHTLEFT, 0);

				if (column == expanderColumn)
				{
					adjustedCellRect.X() += expanderIndent;
					adjustedCellRect.Width() -= expanderIndent;
				}

				// Draw vertical rules but don't draw the rule for last column is we have only one column
				if (verticalRulesEnabled && visibleColumnsCount > 1)
				{
					// Draw vertical rules in column's last pixel, so they will align with header control dividers
					const int x = cellRect.GetX() + cellRect.GetWidth() - 1;
					int yAdd = 1;
					if (currentRow + 1 == rowEnd)
					{
						yAdd += clientRect.GetHeight();
					}
					gc->DrawLine(PointF(x, cellRect.GetTop()), PointF(x, cellRect.GetBottom() + yAdd), *penRuleV);
				}

				// Draw horizontal rules
				if (horizontalRulesEnabled)
				{
					gc->DrawLine(PointF(xCoordStart, cellInitialRect.GetY()), PointF(xCoordEnd + clientRect.GetWidth(), cellInitialRect.GetY()), *penRuleV);
				}

				// Draw the cell
				if (!isCategoryRow || currentColumnIndex == 0)
				{
					cellRenderer.SetupCellDisplayValue();
					cellRenderer.DrawCellBackground(cellRect, cellState);

					// Clip to current cell but a few pixels smaller
					GraphicsAction::Clip cellClip(*gc, focusCellRect);
					cellRenderer.DrawCellContent(adjustedCellRect, cellState);

					#if 0
					// Measure category line offset
					if (isCategoryRow && categoryRowOffset < 0)
					{
						categoryRowOffset = rendererScope->GetCellSize().GetWidth() + GetCharWidth();
						if (column == expanderColumn)
						{
							categoryRowOffset += expanderIndent;
						}
					}
					#endif
				}

				// Draw expander
				if (column == expanderColumn && !expanderRect.IsEmpty())
				{
					FlagSet<NativeWidgetFlag> flags;
					flags.Add(NativeWidgetFlag::Current, m_TreeNodeUnderMouse == node);
					flags.Add(NativeWidgetFlag::Expanded, node->IsExpanded());
					flags.Add(NativeWidgetFlag::Flat, m_View->m_Style.Contains(DV::WidgetStyle::Flat));

					// We can skip on clipping if we know that we have enough space
					GraphicsAction::Clip cellClip(*gc);
					if (cellRect.GetWidth() <= expanderRect.GetWidth())
					{
						Rect clipRect = cellRect;
						clipRect.Width() -= m_View->FromDIP(EXPANDER_MARGIN);

						cellClip.Add(clipRect);
					}

					if (isCategoryRow)
					{
						auto actualExpanderRect = Rect(expanderRect.GetPosition(), nativeRenderer.GetCollapseButtonSize(m_View)).CenterIn(expanderRect);
						nativeRenderer.DrawCollapseButton(m_View, *gc, actualExpanderRect, flags);
					}
					else
					{
						auto actualExpanderRect = Rect(expanderRect.GetPosition(), nativeRenderer.GetExpanderButtonSize(m_View)).CenterIn(expanderRect);
						nativeRenderer.DrawExpanderButton(m_View, *gc, actualExpanderRect, flags);
					}
				}

				// Draw cell focus
				if (m_HasFocus && isCellFocusEnabled && cellState.IsCurrent() && cellState.IsSelected() && column == m_CurrentColumn)
				{
					// Focus rect looks ugly in it's narrower 3px
					if (focusCellRect.GetWidth() > 3 && focusCellRect.GetHeight() > 3)
					{
						nativeRenderer.DrawItemFocusRect(m_View, *gc, focusCellRect, NativeWidgetFlag::Selected);
					}
				}

				// Draw drop hint
				if (cellState.IsDropTarget())
				{
					Rect rowRect = GetRowRect(!fullRowSelectionEnabled);
					nativeRenderer.DrawItemFocusRect(m_View, *gc, rowRect.Deflate(Size(m_View->FromDIP(wxSize(1, 1)))), NativeWidgetFlag::Selected);
				}

				// Move coordinates to next column
				cellRect.X() += cellRect.GetWidth();
			}

			// Draw selection and hot-track indicator after background and cell content
			if (cellState.IsSelected() || cellState.IsHotTracked())
			{
				nativeRenderer.DrawItemSelectionRect(m_View, *gc, GetRowRect(!fullRowSelectionEnabled), cellState.ToNativeWidgetFlags(m_View->m_Widget));
			}

			// This needs more work
			#if 0
			if (isCategoryRow)
			{
				if (categoryRowOffset < 0)
				{
					categoryRowOffset = 0;
				}

				Point pos1(cellInitialRect.GetX() + categoryRowOffset, cellRect.GetY() + cellRect.GetHeight() / 2);
				Point pos2(cellInitialRect.GetX() + xCoordEnd - xCoordStart - categoryRowOffset, pos1.GetY());
				gc->DrawLine(pos1, pos2);
			}
			#endif
		}
	}
	DV::CellState MainWindow::GetCellStateForRow(DV::Row row) const
	{
		DV::CellState state;
		if (row)
		{
			if (m_CurrentRow == row)
			{
				state.SetCurrent();
			}
			if (IsRowSelected(row))
			{
				state.SetItemSelected();
			}
			if (m_HotTrackRowEnabled && row == m_HotTrackRow && m_HotTrackColumn)
			{
				state.SetHotTracked();
			}
			if (m_DropHint && row == m_DropHintRow)
			{
				state.SetDropTarget();
			}
		}
		return state;
	}

	void MainWindow::UpdateDisplay()
	{
		m_Dirty = true;
	}
	void MainWindow::RefreshDisplay()
	{
		m_RedrawNeeded = true;
	}
	void MainWindow::RecalculateDisplay()
	{
		auto rowWidth = GetRowWidth();
		if (m_Model)
		{
			m_View->SetVirtualSize(rowWidth, GetRowStart(GetRowCount()) + GetHeaderOffset());
		}
		else
		{
			// When we have no model set, use just total column widths for the virtual size
			m_View->SetVirtualSize(rowWidth, m_View->m_virtualSize.GetY() + GetHeaderOffset());
		}

		m_View->SetScrollRate(std::min(m_View->GetCharWidth() * 2, m_UniformRowHeight), m_UniformRowHeight);
		m_View->ScheduleRefresh();
	}

	// Tooltip
	bool MainWindow::ShowToolTip(const DV::Node& node, DV::Column& column)
	{
		// Get tooltip
		auto renderer = node.GetCellRenderer(column);
		renderer.BeginCellSetup(node, column);
		renderer.SetupCellDisplayValue();
		DV::ToolTip tooltip = node.GetCellToolTip(column);

		if (tooltip)
		{
			// See if we need to display the tooltip at all
			bool shouldShow = true;
			if (tooltip.ShouldDisplayOnlyIfClipped())
			{
				// Setup renderer to get cell size
				auto& clipTestColumn = tooltip.SelectClipTestColumn(column);

				auto clipTestRenderer = node.GetCellRenderer(clipTestColumn);
				clipTestRenderer.BeginCellSetup(node, const_cast<DV::Column&>(clipTestColumn));
				clipTestRenderer.SetupCellDisplayValue();

				// Test for clipping
				shouldShow = false;
				const Rect cellRect = clipTestColumn.GetRect();
				auto CompareWidth = [cellSize = clipTestRenderer.GetEffectiveCellSize()](int width)
				{
					return width - (2 * PADDING_RIGHTLEFT) <= cellSize.GetWidth();
				};

				// If the column is too small to display its content
				shouldShow = shouldShow || CompareWidth(cellRect.GetWidth());

				// If the column scrolled outside of the visible area
				shouldShow = shouldShow || CompareWidth(m_View->GetClientSize().GetWidth() - cellRect.GetX());
			}

			// Show it
			if (shouldShow)
			{
				return tooltip.Show(node, column);
			}
		}
		return false;
	}
	void MainWindow::RemoveTooltip()
	{
		m_ToolTipTimer.Stop();
		m_ToolTip.Dismiss();
		m_View->SetToolTip({});
	}

	// Columns
	void MainWindow::OnDeleteColumn(DV::Column& column)
	{
		if (&column == m_HotTrackColumn)
		{
			m_HotTrackColumn = nullptr;
		}
		if (&column == m_CurrentColumn)
		{
			m_CurrentColumn = nullptr;
		}
	}
	void MainWindow::OnColumnCountChanged()
	{
		m_UseCellFocus = m_View->GetColumnCount() != 0;
		UpdateDisplay();
	}
	bool MainWindow::IsCellInteractible(const DV::Node& node, const DV::Column& column, InteractibleCell action) const
	{
		switch (action)
		{
			case InteractibleCell::Activator:
			{
				return node.IsCellActivatable(column);
			}
			case InteractibleCell::Editor:
			{
				return node.IsCellEditable(column);
			}
		};
		return false;
	}
	DV::Column* MainWindow::FindInteractibleColumn(const DV::Node& node, InteractibleCell action)
	{
		// Edit the current column editable in 'mode'. If no column is focused
		// (typically because the user has full row selected), try to find the
		// first editable column (this would typically be a checkbox for
		// wxDATAVIEW_CELL_ACTIVATABLE and we don't want to force the user to set
		// focus on the checkbox column; or on the only editable text column).

		auto candidate = m_CurrentColumn;
		if (candidate && !IsCellInteractible(node, *candidate, action) && !m_IsCurrentColumnSetByKeyboard)
		{
			// If current column was set by mouse to something not editable
			// and the user pressed Space/F2 to edit it, treat the
			// situation as if there was whole-row focus, because that's what is
			// visually indicated and the mouse click could very well be targeted
			// on the row rather than on an individual cell.
			//
			// But if it was done by keyboard, respect that even if the column
			// isn't editable, because focus is visually on that column and editing
			// something else would be surprising.
			candidate = nullptr;
		}

		if (!candidate)
		{
			const size_t columnCount = m_View->GetColumnCount();
			for (size_t columnIndex = 0; columnIndex < columnCount; columnIndex++)
			{
				auto column = m_View->GetColumnDisplayedAt(columnIndex);
				if (column->IsVisible() && IsCellInteractible(node, *column, action))
				{
					candidate = column;
					break;
				}
			}
		}

		if (candidate && IsCellInteractible(node, *candidate, action))
		{
			return candidate;
		}
		return nullptr;
	}
	int MainWindow::CalcBestColumnWidth(const DV::Column& column) const
	{
		if (column.GetBestWidth() >= 0)
		{
			return column.GetBestWidth();
		}

		const size_t rowCount = GetRowCount();
		MaxWidthCalculator calculator(*this, column, m_UniformRowHeight);
		calculator.UpdateWithWidth(column.GetMinWidth());

		if (m_View->GetHeaderCtrl())
		{
			calculator.UpdateWithWidth(column.CalcTitleWidth());
		}

		const Point origin = Point(m_View->CalcUnscrolledPosition(Point(0, 0)));
		calculator.ComputeBestColumnWidth(rowCount, *GetRowAt(origin.GetY()), *GetRowAt(origin.GetY() + m_View->GetClientSize().GetY()));

		int maxWidth = calculator.GetMaxWidth();
		if (maxWidth > 0)
		{
			maxWidth += 2 * PADDING_RIGHTLEFT;
		}
		return maxWidth;
	}
	bool MainWindow::FitLastColumn(bool update)
	{
		if (!m_View->m_Style.Contains(DV::WidgetStyle::FitLastColumn))
		{
			return false;
		}

		size_t columnCount = m_View->GetVisibleColumnCount();
		if (columnCount != 0)
		{
			auto lastVisibleColumn = m_View->GetColumnPhysicallyDisplayedAt(columnCount - 1);
			if (lastVisibleColumn)
			{
				auto SetColumnWidth = [lastVisibleColumn, update](int width)
				{
					if (update)
					{
						lastVisibleColumn->SetWidth(width);
					}
					else
					{
						lastVisibleColumn->m_Width = width;
					}
				};

				const int clientWidth = m_View->GetClientSize().GetWidth();
				const int virtualWidth = GetRowWidth();
				const int lastColumnLeft = virtualWidth - lastVisibleColumn->GetWidth();

				if (lastColumnLeft < clientWidth)
				{
					const bool fitToClient = m_View->m_Style.Contains(DV::WidgetStyle::FitLastColumnToClient);
					const int desiredWidth = std::max(clientWidth - lastColumnLeft, lastVisibleColumn->GetMinWidth());

					if (desiredWidth < lastVisibleColumn->GetBestWidth() && !fitToClient)
					{
						SetColumnWidth(lastVisibleColumn->GetWidth());
						m_View->SetVirtualSize(virtualWidth, m_View->m_virtualSize.GetY());
						return true;
					}
					SetColumnWidth(desiredWidth);

					// All columns fit on screen, so we don't need horizontal scrolling.
					// To prevent flickering scrollbar when resizing the window to be
					// narrower, force-set the virtual width to 0 here. It will eventually
					// be corrected at idle time.
					m_View->SetVirtualSize(0, m_View->m_virtualSize.GetY());
					m_View->RefreshRect(Rect(lastColumnLeft, 0, clientWidth - lastColumnLeft, m_View->GetSize().GetY()));

					return true;
				}
				else
				{
					// Don't bother, the columns won't fit anyway
					SetColumnWidth(lastVisibleColumn->GetWidth());
					m_View->SetVirtualSize(virtualWidth, m_View->m_virtualSize.GetY());

					return true;
				}
			}
		}
		return false;
	}

	// Items
	size_t MainWindow::RecalculateItemCount() const
	{
		if (m_TreeRoot)
		{
			return m_TreeRoot.GetSubTreeCount();
		}
		return 0;
	}
	void MainWindow::InvalidateItemCount()
	{
		m_ItemsCount = INVALID_COUNT;
		m_TreeNodeUnderMouse = nullptr;
		m_ToolTipTimer.Stop();
		CancelEdit();
	}
	void MainWindow::OnCellChanged(DV::Node& node, DV::Column* column)
	{
		// In principle, we could skip the call to 'OnSortChildren' if the modified
		// column is not the sort column, but in real-world applications it's fully
		// possible and likely that custom compare uses not only the selected model
		// column but also falls back to other values for comparison. To ensure consistency
		// it is better to treat a value change as if it was an item change.

		node.SortChildren(m_View->GetSortMode());
		if (!node.IsRootNode())
		{
			if (column)
			{
				column->SetBestWidth(-1);
			}
			else
			{
				m_View->InvalidateColumnsBestWidth();
			}

			// Update the displayed value(s).
			RefreshRow(GetRowByNode(node));

			// Send event
			auto event = MakeEvent();
			event.SetNode(&node);
			event.SetColumn(column);
			ProcessEvent(event, DataViewWidgetEvent::EvtItemValueChanged);
		}
	}
	void MainWindow::OnNodeAdded(DV::Node& node)
	{
		InvalidateItemCount();

		m_View->InvalidateColumnsBestWidth();
		UpdateDisplay();
	}
	void MainWindow::OnNodeRemoved(DV::Node& item, intptr_t removedCount)
	{
		InvalidateItemCount();
		m_View->InvalidateColumnsBestWidth();

		// Update selection by removing this node and its entire children tree from the selection.
		if (!m_SelectionStore.IsEmpty())
		{
			m_SelectionStore.OnItemsDeleted(*item.GetRow(), removedCount);
		}

		// Change the current row to the last row if the current exceed the max row number
		if (m_CurrentRow >= GetRowCount())
		{
			ChangeCurrentRow(m_ItemsCount - 1);
		}

		m_TreeNodeUnderMouse = nullptr;
		UpdateDisplay();
	}
	void MainWindow::OnItemsCleared()
	{
		InvalidateItemCount();
		m_SelectionStore.Clear();
		m_CurrentRow = {};
		m_HotTrackRow = {};
		m_CurrentColumn = nullptr;
		m_HotTrackColumn = nullptr;
		m_TreeNodeUnderMouse = nullptr;

		m_View->InvalidateColumnsBestWidth();
		UpdateDisplay();
	}
	void MainWindow::OnShouldResort()
	{
		if (m_TreeRoot)
		{
			m_TreeRoot.SortChildren(m_View->GetSortMode());
		}
		UpdateDisplay();
	}

	bool MainWindow::IsListLike() const
	{
		if (m_TreeRoot)
		{
			bool isList = true;
			for (const DV::Node& node: m_TreeRoot.m_Children)
			{
				if (node.HasChildren())
				{
					isList = false;
					break;
				}
			}
			return isList;
		}
		return false;
	}

	// Misc
	void MainWindow::OnInternalIdle()
	{
		if (m_HotTrackRow && !IsMouseInWindow())
		{
			m_HotTrackRowEnabled = false;
			RefreshRow(m_HotTrackRow);

			m_HotTrackRow = {};
			m_HotTrackColumn = nullptr;

			RemoveTooltip();
		}

		if (m_Dirty)
		{
			RecalculateDisplay();
			FitLastColumn();

			m_Dirty = false;
			m_RedrawNeeded = false;
		}
		if (m_RedrawNeeded)
		{
			FitLastColumn(false);
			if (auto header = m_View->GetHeaderCtrl())
			{
				header->DoUpdate();
			}
			RecalculateDisplay();

			m_RedrawNeeded = false;
		}
	}

	MainWindow::MainWindow(View* parent)
		:m_View(parent)
	{
		// Setup drawing
		m_UniformRowHeight = GetDefaultRowHeight();
		m_Indent = System::GetMetric(SystemSizeMetric::IconSmall).GetWidth();

		// Tooltip
		m_ToolTip.Create(m_View);
		m_ToolTipTimer.SetOwner(m_View, wxID_ANY);
		m_View->Bind(wxEVT_TIMER, &MainWindow::OnTooltipEvent, this);

		// Bind events
		m_View->Bind(wxEVT_PAINT, &MainWindow::OnPaint, this);
		m_View->Bind(wxEVT_SET_FOCUS, &MainWindow::OnFocusSet, this);
		m_View->Bind(wxEVT_KILL_FOCUS, &MainWindow::OnFocusLost, this);
		m_View->Bind(wxEVT_CHAR_HOOK, &MainWindow::OnCharHook, this);
		m_View->Bind(wxEVT_CHAR, &MainWindow::OnChar, this);

		m_View->Bind(wxEVT_LEFT_DOWN, &MainWindow::OnMouse, this);
		m_View->Bind(wxEVT_LEFT_UP, &MainWindow::OnMouse, this);
		m_View->Bind(wxEVT_LEFT_DCLICK, &MainWindow::OnMouse, this);

		m_View->Bind(wxEVT_RIGHT_DOWN, &MainWindow::OnMouse, this);
		m_View->Bind(wxEVT_RIGHT_UP, &MainWindow::OnMouse, this);
		m_View->Bind(wxEVT_RIGHT_DCLICK, &MainWindow::OnMouse, this);

		m_View->Bind(wxEVT_MOTION, &MainWindow::OnMouse, this);
		m_View->Bind(wxEVT_ENTER_WINDOW, &MainWindow::OnMouse, this);
		m_View->Bind(wxEVT_LEAVE_WINDOW, &MainWindow::OnMouse, this);
		m_View->Bind(wxEVT_MOUSEWHEEL, &MainWindow::OnMouse, this);

		// Do update
		UpdateDisplay();
	}
	MainWindow::~MainWindow()
	{
		RemoveTooltip();
		AssignModel(nullptr);
	}

	void MainWindow::AssignModel(std::shared_ptr<IDataViewModel> model)
	{
		// Keep the old model around unil we've done cleaning the state
		std::swap(m_Model, model);

		m_TreeRoot.Initalize(*this);
		m_TreeRoot.RefreshChildren();
		ItemsChanged();
	}
	void MainWindow::ItemsChanged()
	{
		m_ItemsCount = RecalculateItemCount();
		m_View->InvalidateColumnsBestWidth();
		m_View->ScheduleRefresh();
	}

	// Refreshing
	void MainWindow::RefreshRows(DV::Row from, DV::Row to)
	{
		auto rowsRect = GetRowsRect(from, to);
		m_View->CalcScrolledPosition(rowsRect.GetX(), rowsRect.GetY(), &rowsRect.X(), &rowsRect.Y());

		auto clientRect = Rect(m_View->GetClientRect());
		auto intersectRect = clientRect.Intersect(rowsRect);
		if (!intersectRect.IsEmpty())
		{
			intersectRect.MoveTopBy(GetHeaderOffset());
			m_View->RefreshRect(intersectRect);
		}
	}
	void MainWindow::RefreshRowsAfter(DV::Row firstRow)
	{
		Size clientSize = Size(m_View->GetClientSize());
		int start = GetRowStart(firstRow);
		m_View->CalcScrolledPosition(start, 0, &start, nullptr);

		if (start <= clientSize.GetHeight())
		{
			Rect rect(0, start, clientSize.GetWidth(), clientSize.GetHeight() - start);
			rect.MoveTopBy(GetHeaderOffset());

			m_View->RefreshRect(rect);
		}
	}
	void MainWindow::RefreshColumn(const DV::Column& column)
	{
		Size clientSize = Size(m_View->GetClientSize());

		// Find X coordinate of this column
		int left = 0;
		for (size_t i = 0; i < m_View->GetColumnCount(); i++)
		{
			if (auto currentColumn = m_View->GetColumnDisplayedAt(i))
			{
				if (currentColumn == &column)
				{
					break;
				}
				left += currentColumn->GetWidth();
			}
		}

		Rect rect = {left, 0, column.GetWidth(), clientSize.GetHeight()};
		rect.MoveTopBy(GetHeaderOffset());

		m_View->RefreshRect(rect);
	}

	// Item rect
	int MainWindow::GetHeaderOffset() const
	{
		if (auto header = m_View->m_HeaderArea)
		{
			return header->FromDIP(m_View->m_HeaderHeight + m_View->m_HeaderBorder);
		}
		return 0;
	}
	Rect MainWindow::GetClientRect() const
	{
		const auto headerOffset = GetHeaderOffset();

		auto rect = Rect(m_View->GetClientRect());
		rect.MoveTopBy(headerOffset);
		rect.MoveBottomBy(-headerOffset);

		return rect;
	}

	Rect MainWindow::GetRowRect(DV::Row row) const
	{
		return GetRowsRect(row, row);
	}
	Rect MainWindow::GetRowsRect(DV::Row rowFrom, DV::Row rowTo) const
	{
		if (rowFrom > rowTo)
		{
			std::swap(rowFrom, rowTo);
		}

		Rect rect;
		rect.X() = 0;
		rect.Y() = GetRowStart(rowFrom);

		// Don't calculate exact width of the row, because 'GetRowWidth' is
		// expensive to call, and controls with rows not spanning entire width rare.
		// It is more efficient to e.g. repaint empty parts of the window needlessly.
		rect.Width() = std::numeric_limits<decltype(rect.GetWidth())>::max();
		if (rowFrom == rowTo)
		{
			rect.Height() = GetRowHeight(rowFrom);
		}
		else
		{
			rect.Height() = GetRowStart(rowTo) - rect.GetY() + GetRowHeight(rowTo);
		}
		return rect;
	}
	int MainWindow::GetRowStart(DV::Row row) const
	{
		if (m_View->m_Style.Contains(DV::WidgetStyle::VariableRowHeight))
		{
			const size_t columnCount = m_View->GetColumnCount();
			int start = 0;

			for (size_t currentRow = 0; currentRow < *row; currentRow++)
			{
				if (const auto node = GetNodeByRow(currentRow))
				{
					start += GetVariableRowHeight(*node);
				}
				else
				{
					break;
				}
			}
			return start;
		}
		return *row * m_UniformRowHeight;
	}
	int MainWindow::GetRowHeight(DV::Row row) const
	{
		if (m_View->m_Style.Contains(DV::WidgetStyle::VariableRowHeight))
		{
			if (const auto node = GetNodeByRow(row))
			{
				return GetVariableRowHeight(*node);
			}
		}
		return m_UniformRowHeight;
	}
	int MainWindow::GetVariableRowHeight(const DV::Node& node) const
	{
		if (m_View->m_Style.Contains(DV::WidgetStyle::VariableRowHeight))
		{
			if (auto column = m_CurrentColumn ? m_CurrentColumn : m_View->GetExpanderColumnOrFirstOne())
			{
				return std::max(node.GetCellAttributes(*column, {}).Options().GetRowHeight(), m_UniformRowHeight);
			}
		}
		return m_UniformRowHeight;
	}
	int MainWindow::GetVariableRowHeight(DV::Row row) const
	{
		const auto node = GetNodeByRow(row);
		return node ? GetVariableRowHeight(*node) : m_UniformRowHeight;
	}
	int MainWindow::GetRowWidth() const
	{
		int totalWidth = 0;
		for (size_t i = 0; i < m_View->GetColumnCount(); i++)
		{
			int width = 0;
			if (m_View->GetColumnAt(i)->IsExposed(width))
			{
				totalWidth += width;
			}
		}
		return totalWidth;
	}
	DV::Row MainWindow::GetRowAt(int yCoord) const
	{
		if (m_View->m_Style.Contains(DV::WidgetStyle::VariableRowHeight))
		{
			DV::Row row = 0;
			int yPos = 0;
			while (true)
			{
				const auto node = GetNodeByRow(row);
				if (!node)
				{
					// Not really correct but anyway
					return row + ((yCoord - yPos) / m_UniformRowHeight);
				}

				yPos += GetVariableRowHeight(*node);
				if (yCoord < yPos)
				{
					return row;
				}
				++row;
			}
		}
		else
		{
			return yCoord / m_UniformRowHeight;
		}
	}

	void MainWindow::SetUniformRowHeight(int height)
	{
		m_UniformRowHeight = height > 0 ? height : GetDefaultRowHeight();
	}
	int MainWindow::GetDefaultRowHeight(UniformHeight type) const
	{
		const Size iconSpacing =
		{
			System::GetMetric(SystemSizeMetric::Border, m_View).GetWidth() * 6,
			System::GetMetric(SystemSizeMetric::IconSmall, m_View).GetHeight()
		};

		int resultHeight = 0;
		switch (type)
		{
			case UniformHeight::Default:
			{
				int userHeight = wxSystemOptions::GetOptionInt("Kx::UI::DataView::DefaultRowHeight");
				if (userHeight > 0)
				{
					return userHeight;
				}
				else
				{
					resultHeight = std::max(iconSpacing.GetHeight() + iconSpacing.GetWidth(), m_View->GetCharHeight() + iconSpacing.GetWidth());
				}
				break;
			}
			case UniformHeight::ListView:
			{
				resultHeight = std::max(m_View->FromDIP(17), m_View->GetCharHeight() + iconSpacing.GetWidth());
				break;
			}
			case UniformHeight::Explorer:
			{
				resultHeight = std::max(m_View->FromDIP(22), m_View->GetCharHeight() + iconSpacing.GetWidth());
				break;
			}
		};
		return resultHeight >= iconSpacing.GetHeight() ? resultHeight : iconSpacing.GetHeight();
	}

	// Drag and Drop
	/*
	GDIBitmap MainWindow::CreateItemBitmap(DV::Row row, int& indent)
	{
		int width = GetRowWidth();
		int height = GetRowHeight(row);

		indent = 0;
		if (!IsListLike())
		{
			Node* node = GetNodeByRow(row);
			indent = m_Indent * node->GetItemIndent();
			indent = indent + m_UniformRowHeight;
		}
		width -= indent;
		Rect itemRect = Rect(0, 0, width, height);

		auto texture = m_GraphicsRenderer->CreateTexture(SizeF(width, height), Drawing::GetStockColor(StockColor::Transparent));
		auto gc = m_GraphicsRenderer->CreateContext(texture);
		{
			gc->SetFont(m_GraphicsRenderer->CreateFont(GetFont()));
			gc->SetFontBrush(m_GraphicsRenderer->CreateSolidBrush(m_View->GetForegroundColour()));
			gc->Clear(*m_GraphicsRenderer->CreateSolidBrush(m_View->GetBackgroundColour()));

			// Draw selection
			IRendererNative::Get().DrawItemSelectionRect(this, *gc, itemRect, NativeWidgetFlag::Current|NativeWidgetFlag::Selected|NativeWidgetFlag::Focused);

			// Draw cells
			int x = 0;
			Column* expander = m_View->GetExpanderColumnOrFirstOne();
			for (size_t columnIndex = 0; columnIndex < m_View->GetColumnCount(); columnIndex++)
			{
				Column* column = m_View->GetColumnDisplayedAt(columnIndex);
				if (column->IsExposed(width))
				{
					if (column == expander)
					{
						width -= indent;
					}

					CellState cellState = GetCellStateForRow(row);
					Node* node = GetNodeByRow(row);
					Renderer& renderer = node->GetCellRenderer(*column);
					renderer.BeginCellRendering(*node, *column, *gc);

					Rect cellRect(x, 0, width, height);

					renderer.SetupCellDisplayValue();
					renderer.SetupCellAttributes(cellState);
					renderer.DrawCellBackground(cellRect, cellState);

					cellRect.Deflate(PADDING_RIGHTLEFT, 0);
					renderer.DrawCellContent(cellRect, cellState);

					renderer.EndCellRendering();
					x += width;
				}
			}
		}

		return texture->ToBitmapImage().ToGDIBitmap();
	}

	bool MainWindow::EnableDND(std::unique_ptr<wxDataObjectSimple> dataObject, DNDOpType type, bool isPreferredDrop)
	{
		using Result = DnDInfo::Result;

		if (dataObject)
		{
			const auto result = m_DragDropInfo.EnableOperation(*dataObject, type, isPreferredDrop);
			if (result != Result::None && result != Result::OperationRemoved)
			{
				if (m_DragDropDataObject == nullptr)
				{
					m_DragDropDataObject = new wxDataObjectComposite();
					m_DropTarget = new DropTarget(m_DragDropDataObject, this);
					SetDropTarget(m_DropTarget);
				}

				if (m_DragDropDataObject->GetObject(dataObject->GetFormat()) == nullptr)
				{
					m_DragDropDataObject->Add(dataObject.release(), isPreferredDrop);
				}
			}
			return result != DnDInfo::Result::None;
		}
		return false;
	}
	bool MainWindow::DisableDND(const wxDataFormat& format)
	{
		using Result = DnDInfo::Result;

		const Result value = m_DragDropInfo.DisableOperations(format);
		return value != Result::OperationRemoved || value == Result::None;
	}

	std::tuple<DV::Row, DV::Node*> MainWindow::DragDropHitTest(const Point& pos) const
	{
		// Get row
		Point unscrolledPos;
		m_View->CalcUnscrolledPosition(pos.GetX(), pos.GetY(), &unscrolledPos.X(), &unscrolledPos.Y());
		Row row = GetRowAt(unscrolledPos.GetY());

		// Get item
		Node* node = nullptr;
		if (row < GetRowCount() && pos.GetY() <= GetRowWidth())
		{
			node = GetNodeByRow(row);
		}

		return {row, node};
	}
	void MainWindow::RemoveDropHint()
	{
		if (m_DropHint)
		{
			m_DropHint = false;
			RefreshRow(m_DropHintRow);
			m_DropHintRow = {};
		}
	}
	wxDragResult MainWindow::OnDragOver(const wxDataObjectSimple& dataObject, const Point& pos, wxDragResult dragResult)
	{
		auto [row, node] = DragDropHitTest(pos);
		if (row != 0 && row < GetRowCount())
		{
			const Row firstVisible = GetFirstVisibleRow();
			const Row lastVisible = GetLastVisibleRow();
			if (row == firstVisible || row == firstVisible + 1)
			{
				ScrollTo(row - 1);
			}
			else if (row == lastVisible || row == lastVisible - 1)
			{
				ScrollTo(firstVisible + 1);
			}
		}

		DragDropEvent event(DragDropEvent::EvtItemDropPossible);
		CreateEventTemplate(event, node);
		event.SetPosition(pos);
		event.SetDropEffect(dragResult);
		event.SetDataObject(const_cast<wxDataObjectSimple*>(&dataObject));

		if (!m_View->HandleWindowEvent(event) || !event.IsAllowed())
		{
			RemoveDropHint();
			return wxDragNone;
		}

		if (node)
		{
			if (m_DropHint && (row != m_DropHintRow))
			{
				RefreshRow(m_DropHintRow);
			}
			m_DropHint = true;
			m_DropHintRow = row;
			RefreshRow(row);
		}
		else
		{
			RemoveDropHint();
		}
		return dragResult;
	}
	wxDragResult MainWindow::OnDropData(wxDataObjectSimple& dataObject, const Point& pos, wxDragResult dragResult)
	{
		auto [row, node] = DragDropHitTest(pos);

		DragDropEvent event(DragDropEvent::EvtItemDrop);
		CreateEventTemplate(event, node);
		event.SetPosition(pos);
		event.SetDropEffect(dragResult);
		event.SetDataObject(&dataObject);

		if (!m_View->HandleWindowEvent(event) || !event.IsAllowed())
		{
			return wxDragNone;
		}
		return dragResult;
	}
	bool MainWindow::TestDropPossible(const wxDataObjectSimple& dataObject, const Point& pos)
	{
		RemoveDropHint();
		auto [row, node] = DragDropHitTest(pos);

		DragDropEvent event(DragDropEvent::EvtItemDropPossible);
		CreateEventTemplate(event, node);
		event.SetPosition(pos);
		event.SetDataObject(const_cast<wxDataObjectSimple*>(&dataObject));

		if (!m_View->HandleWindowEvent(event) || !event.IsAllowed())
		{
			return false;
		}
		return true;
	}

	wxDragResult MainWindow::OnDragDropEnter(const wxDataObjectSimple& dataObject, const Point& pos, wxDragResult dragResult)
	{
		return dragResult;
	}
	void MainWindow::OnDragDropLeave()
	{
		RemoveDropHint();
	}
	*/

	// Scrolling
	void MainWindow::ScrollWidget(int dx, int dy, const Rect& rect)
	{
		// Immediately refresh the visible client area
		m_View->RefreshRect(rect);

		// And schedule a single refresh after we're done with the scrolling
		// to process any pending actions, like mouse position.
		RefreshDisplay();
	}
	void MainWindow::ScrollTo(DV::Row row, size_t column)
	{
		Point pos;
		m_View->GetScrollPixelsPerUnit(&pos.X(), &pos.Y());
		Point scrollPos(-1, GetRowStart(row) / pos.GetY());

		if (column != INVALID_COLUMN)
		{
			Rect rect = Rect(m_View->GetClientRect());

			Point unscrolledPos;
			m_View->CalcUnscrolledPosition(rect.GetX(), rect.GetY(), &unscrolledPos.X(), &unscrolledPos.Y());

			int columnWidth = 0;
			int x_start = 0;
			for (size_t colnum = 0; colnum < column; colnum++)
			{
				if (m_View->GetColumnDisplayedAt(colnum)->IsExposed(columnWidth))
				{
					x_start += columnWidth;
				}
			}

			int x_end = x_start + columnWidth;
			int xe = unscrolledPos.GetX() + rect.GetWidth();
			if (x_end > xe && scrollPos.GetX() != 0)
			{
				scrollPos.X() = (unscrolledPos.GetX() + x_end - xe) / scrollPos.GetX();
			}
			if (x_start < unscrolledPos.GetX() && scrollPos.GetX())
			{
				scrollPos.X() = x_start / scrollPos.GetX();
			}
		}
		m_View->Scroll(scrollPos);
	}
	void MainWindow::EnsureVisible(DV::Row row, size_t column)
	{
		if (row > GetRowCount())
		{
			row = GetRowCount();
		}

		size_t first = *GetFirstVisibleRow();
		size_t last = *GetLastVisibleRow();
		if (row < first)
		{
			ScrollTo(row, column);
		}
		else if (row > last)
		{
			ScrollTo((intptr_t)*row - (intptr_t)last + (intptr_t)first, column);
		}
		else
		{
			ScrollTo(first, column);
		}
	}

	// Current row and column
	void MainWindow::ChangeCurrentRow(DV::Row row)
	{
		m_CurrentRow = row;
		// Send event ?
	}
	bool MainWindow::TryAdvanceCurrentColumn(DV::Node* node, wxKeyEvent& event, bool moveForward)
	{
		const size_t columnCount = m_View->GetColumnCount();
		const size_t visibleColumnsCount = m_View->GetVisibleColumnCount();
		const bool wrapAround = event.GetKeyCode() == WXK_TAB;
		const bool currentColumnIsExpander = m_CurrentColumn && m_CurrentColumn->IsExpander();

		// Nothing to do
		if (columnCount == 0 || !m_UseCellFocus)
		{
			return false;
		}

		if (node && currentColumnIsExpander)
		{
			if (moveForward)
			{
				if (node->HasChildren() && !node->IsExpanded())
				{
					return false;
				}
			}
			else if (node->GetParentNode())
			{
				return false;
			}
		}

		if (!m_CurrentColumn || !m_IsCurrentColumnSetByKeyboard)
		{
			if (moveForward)
			{
				m_CurrentColumn = m_View->GetColumnDisplayedAt(0);
				m_IsCurrentColumnSetByKeyboard = true;
				RefreshRow(m_CurrentRow);
				return true;
			}
		}
		if (!m_CurrentColumn)
		{
			return false;
		}

		size_t nextColumn = std::clamp<intptr_t>((intptr_t)m_CurrentColumn->GetDisplayIndex() + (moveForward ? +1 : -1), 0, visibleColumnsCount);
		if (nextColumn == static_cast<intptr_t>(visibleColumnsCount))
		{
			if (!wrapAround)
			{
				return false;
			}

			if (GetCurrentRow() < GetRowCount() - 1)
			{
				// Go to the first column of the next row:
				nextColumn = 0;
				OnVerticalNavigation(wxKeyEvent(), +1);
			}
			else
			{
				// allow focus change
				event.Skip();
				return false;
			}
		}

		if (nextColumn == 0 && wrapAround)
		{
			if (GetCurrentRow())
			{
				// Go to the last column of the previous row
				nextColumn = visibleColumnsCount - 1;
				OnVerticalNavigation(wxKeyEvent(), -1);
			}
			else
			{
				// Allow focus change
				event.Skip();
				return false;
			}
		}

		m_IsCurrentColumnSetByKeyboard = true;
		m_CurrentColumn = m_View->GetColumnDisplayedAt(nextColumn);

		EnsureVisible(m_CurrentRow, m_CurrentColumn->GetIndex());
		RefreshRow(m_CurrentRow);
		return true;
	}

	DV::Node* MainWindow::GetHotTrackItem() const
	{
		if (m_HotTrackRow)
		{
			return GetNodeByRow(m_HotTrackRow);
		}
		return nullptr;
	}
	DV::Column* MainWindow::GetHotTrackColumn() const
	{
		return m_HotTrackColumn;
	}

	// Selection
	bool MainWindow::UnselectAllRows(DV::Row exceptThisRow)
	{
		if (!IsSelectionEmpty())
		{
			for (auto row = GetFirstVisibleRow(); row <= GetLastVisibleRow(); ++row)
			{
				if (m_SelectionStore.IsSelected(*row) && row != exceptThisRow)
				{
					RefreshRow(row);
				}
			}

			if (exceptThisRow)
			{
				bool wasSelected = m_SelectionStore.IsSelected(*exceptThisRow);
				ClearSelection();
				if (wasSelected)
				{
					m_SelectionStore.SelectItem(*exceptThisRow);

					// The special item is still selected
					return false;
				}
			}
			else
			{
				ClearSelection();
			}
		}

		// There are no selected items left
		return true;
	}
	void MainWindow::ReverseRowSelection(DV::Row row)
	{
		m_SelectionStore.SelectItem(*row, !IsRowSelected(row));
		RefreshRow(row);
	}
	void MainWindow::SelectRow(DV::Row row, bool select)
	{
		if (m_SelectionStore.SelectItem(*row, select))
		{
			RefreshRow(row);
		}
	}
	void MainWindow::SelectRows(DV::Row from, DV::Row to)
	{
		if (from > to)
		{
			std::swap(from, to);
		}
		for (auto row = from; row <= to; ++row)
		{
			m_SelectionStore.SelectItem(*row, true);
		}
		RefreshRows(from, to);
	}

	// View
	size_t MainWindow::GetRowCount() const
	{
		if (m_ItemsCount == INVALID_COUNT)
		{
			MainWindow* self = const_cast<MainWindow*>(this);
			self->UpdateItemCount(RecalculateItemCount());
			self->UpdateDisplay();
		}
		return m_ItemsCount;
	}
	size_t MainWindow::GetCountPerPage() const
	{
		return GetClientRect().GetHeight() / m_UniformRowHeight;
	}
	DV::Row MainWindow::GetFirstVisibleRow() const
	{
		Point pos;
		m_View->CalcUnscrolledPosition(pos.GetX(), pos.GetY(), &pos.X(), &pos.Y());

		return GetRowAt(pos.GetY());
	}
	DV::Row MainWindow::GetLastVisibleRow() const
	{
		Size size = Size(m_View->GetClientSize());
		m_View->CalcUnscrolledPosition(size.GetWidth(), size.GetHeight(), &size.Width(), &size.Height());

		// We should deal with the pixel here.
		size_t row = *GetRowAt(size.GetHeight()) - 1;
		return std::min(GetRowCount() - 1, row);
	}

	void MainWindow::HitTest(const Point& pos, DV::Node** nodeOut, DV::Column** columnOut)
	{
		Point unscrolledPos = Point::UnspecifiedPosition();
		if (nodeOut)
		{
			m_View->CalcUnscrolledPosition(pos.GetX(), pos.GetY(), &unscrolledPos.X(), &unscrolledPos.Y());
			*nodeOut = GetNodeByRow(GetRowAt(unscrolledPos.GetY()));
		}

		if (columnOut)
		{
			int x_start = 0;
			const size_t columnCount = m_View->GetColumnCount();

			if (!unscrolledPos.IsFullySpecified())
			{
				m_View->CalcUnscrolledPosition(pos.GetX(), pos.GetY(), &unscrolledPos.X(), &unscrolledPos.Y());
			}
			for (size_t colnumIndex = 0; colnumIndex < columnCount; colnumIndex++)
			{
				auto column = m_View->GetColumnDisplayedAt(colnumIndex);

				int width = 0;
				if (column->IsExposed(width))
				{
					if (x_start + width >= unscrolledPos.GetX())
					{
						*columnOut = column;
						return;
					}
					x_start += width;
				}
			}
		}
	}
	Rect MainWindow::GetItemRect(const DV::Node& item, const DV::Column* column) const
	{
		int xpos = 0;
		int width = 0;
		size_t columnCount = m_View->GetColumnCount();

		// If column is null the loop will compute the combined width of all columns.
		// Otherwise, it will compute the x position of the column we are looking for.
		for (size_t i = 0; i < columnCount; i++)
		{
			auto currentColumn = m_View->GetColumnDisplayedAt(i);
			if (currentColumn == column)
			{
				break;
			}

			int widthCurrent = 0;
			if (currentColumn->IsExposed(widthCurrent))
			{
				xpos += widthCurrent;
				width += widthCurrent;
			}
		}

		if (column)
		{
			// If we have a column, we can get its width directly.
			width = 0;
			column->IsExposed(width);
		}
		else
		{
			// If we have no column, we reset the x position back to zero.
			xpos = 0;
		}

		auto row = GetRowByNode(item);
		if (!row)
		{
			// This means the row is currently not visible at all.
			return {};
		}

		// We have to take an expander column into account and compute its indentation
		// to get the correct x position where the actual text is.
		int indent = 0;
		if ((!column || m_View->GetExpanderColumnOrFirstOne() == column) && !IsListLike())
		{
			auto node = GetNodeByRow(row);
			indent = m_Indent * node->GetIndentLevel();

			// Use 'm_UniformLineHeight' as the width of the expander
			indent += m_UniformRowHeight;
		}

		Rect itemRect(xpos + indent, GetRowStart(row), width - indent, GetRowHeight(row));
		m_View->CalcScrolledPosition(itemRect.GetX(), itemRect.GetY(), &itemRect.X(), &itemRect.Y());
		return itemRect;
	}

	// Rows
	void MainWindow::Expand(DV::Row row)
	{
		if (!IsListLike())
		{
			if (auto node = GetNodeByRow(row))
			{
				Expand(*node, row);
			}
		}
	}
	void MainWindow::Expand(DV::Node& node, DV::Row row)
	{
		if (!node.IsExpanded())
		{
			if (!node.OnNodeExpand() || !SendExpanderEvent(DataViewWidgetEvent::EvtItemExpanding, node))
			{
				// Vetoed by the event handler
				return;
			}

			const size_t rowsAdded = node.ToggleNodeExpandState();
			if (!row)
			{
				row = GetRowByNode(node);
			}

			// Shift all stored indices after this row by the number of newly added rows.
			m_SelectionStore.OnItemsInserted(*row + 1, rowsAdded);
			if (m_CurrentRow > row)
			{
				ChangeCurrentRow(m_CurrentRow + rowsAdded);
			}

			if (m_ItemsCount != INVALID_COUNT)
			{
				m_ItemsCount += rowsAdded;
			}

			// Expanding this item means the previously cached column widths could
			// have become invalid as new items are now visible.
			m_View->InvalidateColumnsBestWidth();
			UpdateDisplay();

			// Send the expanded event
			SendExpanderEvent(DataViewWidgetEvent::EvtItemExpanded, node);
		}
	}

	void MainWindow::Collapse(DV::Row row)
	{
		if (!IsListLike())
		{
			if (auto node = GetNodeByRow(row))
			{
				Collapse(*node, row);
			}
		}
	}
	void MainWindow::Collapse(DV::Node& node, DV::Row row)
	{
		if (node.IsExpanded())
		{
			if (!node.OnNodeCollapse() || !SendExpanderEvent(DataViewWidgetEvent::EvtItemCollapsing, node))
			{
				// Vetoed by the event handler
				return;
			}

			if (!row)
			{
				row = GetRowByNode(node);
			}

			const size_t rowsRemoved = node.GetSubTreeCount();
			if (m_SelectionStore.OnItemsDeleted(*row + 1, rowsRemoved))
			{
				RefreshRow(row);

				if (!m_CurrentColumn)
				{
					m_CurrentColumn = m_View->GetExpanderColumnOrFirstOne();
				}
				SendSelectionChangedEvent(GetNodeByRow(row), m_CurrentColumn);
			}
			node.ToggleNodeExpandState();

			// Adjust the current row if necessary.
			if (m_CurrentRow > row)
			{
				// If the current row was among the collapsed items, make the
				// parent itself current.
				if (m_CurrentRow <= row + rowsRemoved)
				{
					ChangeCurrentRow(row);
				}
				else
				{
					// Otherwise just update the index.
					ChangeCurrentRow(m_CurrentRow - rowsRemoved);
				}
			}

			if (m_ItemsCount != INVALID_COUNT)
			{
				m_ItemsCount -= rowsRemoved;
			}

			m_View->InvalidateColumnsBestWidth();
			UpdateDisplay();

			SendExpanderEvent(DataViewWidgetEvent::EvtItemCollapsed, node);
		}
	}

	void MainWindow::ToggleExpand(DV::Row row)
	{
		if (!IsListLike())
		{
			if (auto node = GetNodeByRow(row))
			{
				if (node->IsExpanded())
				{
					node->CollapseNode();
				}
				else
				{
					node->ExpandNode();
				}
			}
		}
	}
	bool MainWindow::IsExpanded(DV::Row row) const
	{
		if (!IsListLike())
		{
			if (auto node = GetNodeByRow(row))
			{
				return node->IsExpanded();
			}
		}
		return false;
	}
	bool MainWindow::HasChildren(DV::Row row) const
	{
		if (!IsListLike())
		{
			auto node = GetNodeByRow(row);
			return node && node->HasChildren();
		}
		return false;
	}

	DV::Node* MainWindow::GetNodeByRow(DV::Row row) const
	{
		if (m_Model && row)
		{
			DV::RowToNodeOperation operation(*row);
			if (operation.Walk(m_TreeRoot))
			{
				return const_cast<DV::Node*>(operation.GetResult());
			}
		}
		return nullptr;
	}
	DV::Row MainWindow::GetRowByNode(const DV::Node& node) const
	{
		if (m_Model && !node.IsRootNode())
		{
			size_t row = 0;
			auto currentNode = &node;
			while (currentNode && !currentNode->IsRootNode())
			{
				// Add current node sub row index
				row += currentNode->GetSubTreeIndex() + 1;

				// If this node has parent, add subtree count from all previous siblings
				if (auto parentNode = currentNode->GetParentNode())
				{
					for (const DV::Node& childNode: parentNode->m_Children)
					{
						if (&childNode == currentNode)
						{
							break;
						}
						row += childNode.GetSubTreeCount();
					}
					currentNode = parentNode;
				}
			}

			// If we reached the root node, consider the search successful
			if (currentNode && currentNode->IsRootNode())
			{
				// Rows are zero-based, but we calculated it as one-based.
				return row - 1;
			}
		}
		return {};
	}

	bool MainWindow::BeginEdit(DV::Node& node, DV::Column& column)
	{
		// Cancel any previous editing
		CancelEdit();

		if (m_CurrentEditor = node.GetCellEditor(column))
		{
			m_View->SetFocus();
			node.EnsureCellVisible(column);

			auto itemRect = GetItemRect(node, &column);
			if (m_CurrentEditor.BeginEdit(node, column, itemRect))
			{
				return true;
			}
		}

		m_CurrentEditor = {};
		return false;
	}
	void MainWindow::EndEdit()
	{
		m_CurrentEditor.EndEdit();
		m_CurrentEditor = {};
	}
	void MainWindow::CancelEdit()
	{
		m_CurrentEditor.CancelEdit();
		m_CurrentEditor = {};
	}
}
