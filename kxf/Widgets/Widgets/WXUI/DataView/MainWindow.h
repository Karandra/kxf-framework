#pragma once
#include "Common.h"
#include "View.h"
#include "Node.h"
#include "../../DataView/Row.h"
#include "../../DataView/Node.h"
#include "../../DataView/Column.h"
#include "../../DataView/SortMode.h"
#include "../../DataView/CellState.h"
#include "../../DataView/CellEditor.h"
#include "../../DataView/CellRenderer.h"
#include "../../DataView/CellAttributes.h"
#include "../../../Events/DataViewWidgetEvent.h"

#include "kxf/Widgets/Windows/ToolTipEx.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/EventSystem/Event.h"
#include <wx/selstore.h>

namespace kxf::DataView
{
	class Column;
	class ToolTip;
	class RootNode;
	class CellEditor;
}
namespace kxf::WXUI::DataView
{
	class HeaderCtrl;
	class MainWindow;
	class DropSource;
	class DropTarget;
}

namespace kxf::WXUI::DataView
{
	class KXF_API MainWindow
	{
		friend class Widgets::DataView;
		friend class DV::Node;
		friend class DV::RootNode;
		friend class DV::Column;
		friend class DV::ToolTip;
		friend class DV::CellEditor;
		friend class DV::CellRenderer;

		friend class View;
		friend class HeaderCtrl;
		friend class HeaderCtrl2;

		friend class DropSource;
		friend class DropTarget;
		friend class MaxWidthCalculator;

		public:
			enum: size_t
			{
				INVALID_ROW = std::numeric_limits<size_t>::max(),
				INVALID_COLUMN = INVALID_ROW,
				INVALID_COUNT = INVALID_ROW,
			};
			enum class InteractibleCell
			{
				Editor,
				Activator,
			};
			enum class UniformHeight
			{
				Default,
				ListView,
				Explorer
			};

		private:
			View* m_View = nullptr;

			int m_UniformRowHeight = 0;
			int m_Indent = 0;

			DV::Column* m_CurrentColumn = nullptr;
			DV::Row m_CurrentRow;
			wxSelectionStore m_SelectionStore;

			bool m_LastOnSame = false;
			bool m_HasFocus = false;
			bool m_UseCellFocus = false;
			bool m_IsCurrentColumnSetByKeyboard = false;
			bool m_IsMouseOverExpander = false;

			bool m_Dirty = true;
			bool m_RedrawNeeded = false;

			DV::Row m_HotTrackRow;
			bool m_HotTrackRowEnabled = false;
			DV::Column* m_HotTrackColumn = nullptr;

			// Tooltip
			UI::ToolTipEx m_ToolTip;
			wxTimer m_ToolTipTimer;

			// Drag and Drop
			/*
			DnDInfo m_DragDropInfo;
			wxDataObjectComposite* m_DragDropDataObject = nullptr;
			DropTarget* m_DropTarget = nullptr;
			DropSource* m_DragSource = nullptr;
			*/
			size_t m_DragCount = 0;

			Point m_DragStart;
			DV::Row m_DropHintRow;
			bool m_DropHint = false;

			// Double click logic
			DV::Row m_RowLastClicked;
			DV::Row m_RowBeforeLastClicked;
			DV::Row m_RowSelectSingleOnUp;

			// Make 'm_ItemsCount' = -1 will cause the class recalculate the real displaying number of rows.
			size_t m_ItemsCount = INVALID_COUNT;
			std::shared_ptr<IDataViewModel> m_Model;
			DV::RootNode m_TreeRoot;

			// This is the tree node under the cursor
			DV::Node* m_TreeNodeUnderMouse = nullptr;

			// Current editor
			DV::CellEditor m_CurrentEditor;

		protected:
			// Events
			void OnChar(wxKeyEvent& event);
			void OnCharHook(wxKeyEvent& event);
			void OnVerticalNavigation(const wxKeyEvent& event, int delta);
			void OnLeftKey(wxKeyEvent& event);
			void OnRightKey(wxKeyEvent& event);

			void OnMouse(wxMouseEvent& event);
			void OnFocusSet(wxFocusEvent& event);
			void OnFocusLost(wxFocusEvent& event);

			void OnTooltipEvent(wxTimerEvent& event);

			// Return false only if the event was vetoed by its handler.
			bool SendExpanderEvent(const EventID& type, DV::Node& item);
			void SendSelectionChangedEvent(DV::Node* item, DV::Column* column = nullptr);

			// Will return true if event allowed
			bool SendEditingStartedEvent(DV::Node& item, DV::Column& column);
			bool SendEditingDoneEvent(DV::Node& item, DV::Column& column, bool canceled, const Any& value);

			DataViewWidgetEvent MakeEvent()
			{
				return DataViewWidgetEvent(m_View->m_Widget);
			}
			bool ProcessEvent(IEvent& event, const EventID& eventID)
			{
				return m_View->m_Widget.ProcessEvent(event, eventID);
			}

			// Drawing
			void OnPaint(wxPaintEvent& event);
			DV::CellState GetCellStateForRow(DV::Row row) const;
			std::shared_ptr<IGraphicsRenderer> GetRenderer() const
			{
				return m_View->m_RendererAware->GetActiveGraphicsRenderer();
			}

			void UpdateDisplay();
			void RefreshDisplay();
			void RecalculateDisplay();

			// Tooltip
			bool ShowToolTip(const DV::Node& node, DV::Column& column);
			void RemoveTooltip();

			// Columns
			void OnDeleteColumn(DV::Column& column);
			void OnColumnCountChanged();
			bool IsCellInteractible(const DV::Node& node, const DV::Column& column, InteractibleCell action) const;
			DV::Column* FindInteractibleColumn(const DV::Node& node, InteractibleCell action);
			int CalcBestColumnWidth(const DV::Column& column) const;
			bool FitLastColumn(bool update = true);

			// Items
			void InvalidateItemCount();
			void UpdateItemCount(size_t count)
			{
				m_ItemsCount = count;
				m_SelectionStore.SetItemCount(count);
			}
			size_t RecalculateItemCount() const;

			void OnCellChanged(DV::Node& node, DV::Column* column);
			void OnNodeAdded(DV::Node& node);
			void OnNodeRemoved(DV::Node& node, intptr_t removedCount);
			void OnItemsCleared();
			void OnShouldResort();

			bool IsListLike() const;

			// Misc
			void OnInternalIdle();

		public:
			MainWindow(View* parent);
			~MainWindow();

		public:
			// Model and nodes
			void AssignModel(std::shared_ptr<IDataViewModel> model);

			const DV::RootNode& GetRootNode() const
			{
				return m_TreeRoot;
			}
			DV::RootNode& GetRootNode()
			{
				return m_TreeRoot;
			}
			void ItemsChanged();

			// Refreshing
			void RefreshRow(DV::Row row)
			{
				if (row)
				{
					RefreshRows(row, row);
				}
			}
			void RefreshRows(DV::Row from, DV::Row to);
			void RefreshRowsAfter(DV::Row firstRow);
			void RefreshColumn(const DV::Column& column);

			// Item rect
			int GetHeaderOffset() const;
			Rect GetClientRect() const;
			Rect GetRowRect(DV::Row row) const;
			Rect GetRowsRect(DV::Row rowFrom, DV::Row rowTo) const;

			int GetRowStart(DV::Row row) const;
			int GetRowHeight(DV::Row row) const;
			int GetVariableRowHeight(const DV::Node& node) const;
			int GetVariableRowHeight(DV::Row row) const;
			int GetRowWidth() const;
			DV::Row GetRowAt(int yCoord) const;

			int GetUniformRowHeight() const
			{
				return m_UniformRowHeight;
			}
			void SetUniformRowHeight(int height);
			int GetDefaultRowHeight(UniformHeight type = UniformHeight::Default) const;

			// Drag and Drop
			/*
			GDIBitmap CreateItemBitmap(DV::Row row, int& indent);
			bool EnableDND(std::unique_ptr<wxDataObjectSimple> dataObject, DNDOpType type, bool isPreferredDrop = false);
			bool DisableDND(const wxDataFormat& format);

			std::tuple<DV::Row, DV::Node*> DragDropHitTest(const Point& pos) const;
			void RemoveDropHint();
			wxDragResult OnDragOver(const wxDataObjectSimple& dataObject, const Point& pos, wxDragResult dragResult);
			wxDragResult OnDropData(wxDataObjectSimple& dataObject, const Point& pos, wxDragResult dragResult);
			bool TestDropPossible(const wxDataObjectSimple& dataObject, const Point& pos);

			wxDragResult OnDragDropEnter(const wxDataObjectSimple& format, const Point& pos, wxDragResult dragResult);
			void OnDragDropLeave();
			*/

			// Scrolling
			void ScrollWidget(int dx, int dy, const Rect& rect);
			void ScrollTo(DV::Row row, size_t column = INVALID_COLUMN);
			void EnsureVisible(DV::Row row, size_t column = INVALID_COLUMN);

			// Current row and column
			DV::Row GetCurrentRow() const
			{
				return m_CurrentRow;
			}
			bool HasCurrentRow() const
			{
				return static_cast<bool>(m_CurrentRow);
			}
			void ChangeCurrentRow(DV::Row row);
			bool TryAdvanceCurrentColumn(DV::Node* node, wxKeyEvent& event, bool moveForward);

			DV::Column* GetCurrentColumn() const
			{
				return m_CurrentColumn;
			}
			void ClearCurrentColumn()
			{
				m_CurrentColumn = nullptr;
			}

			DV::Node* GetHotTrackItem() const;
			DV::Column* GetHotTrackColumn() const;

			// Selection
			bool IsSingleSelection() const
			{
				return !IsMultipleSelection();
			}
			bool IsMultipleSelection() const
			{
				return m_View->m_Style.Contains(DV::WidgetStyle::MultipleSelection);
			}
			bool IsEmpty()
			{
				return GetRowCount() == 0;
			}

			const wxSelectionStore& GetSelections() const
			{
				return m_SelectionStore;
			}
			bool IsSelectionEmpty() const
			{
				return m_SelectionStore.IsEmpty();
			}

			// If a valid row is specified and it was previously selected, it is left selected and the function
			// returns false. Otherwise, i.e. if there is really no selection left in the control, it returns true.
			bool UnselectAllRows(DV::Row exceptThisRow = {});
			void ReverseRowSelection(DV::Row row);
			void ClearSelection()
			{
				m_SelectionStore.SelectRange(0, GetRowCount() - 1, false);
			}
			void SelectRow(DV::Row row, bool select = true);
			void SelectRows(DV::Row from, DV::Row to);
			void SelectAllRows()
			{
				m_SelectionStore.SelectRange(0, GetRowCount() - 1);
				m_View->Refresh();
			}
			bool IsRowSelected(DV::Row row) const
			{
				return m_SelectionStore.IsSelected(*row);
			}

			// View
			View* GetView() const
			{
				return m_View;
			}
			bool IsMouseInWindow() const
			{
				auto mouseState = wxGetMouseState();
				return IsMouseInWindow(Point(m_View->ScreenToClient(mouseState.GetPosition())));
			}
			bool IsMouseInWindow(const Point& position) const
			{
				return GetClientRect().Contains(position);
			}

			size_t GetRowCount() const;
			size_t GetCountPerPage() const;
			DV::Row GetFirstVisibleRow() const;
			DV::Row GetLastVisibleRow() const;

			void HitTest(const Point& pos, DV::Node** nodeOut = nullptr, DV::Column** columnOut = nullptr);
			void HitTest(const Point& pos, DV::Node*& node, DV::Column*& column)
			{
				return HitTest(pos, &node, &column);
			}
			Rect GetItemRect(const DV::Node& item, const DV::Column* column = nullptr) const;

			// Rows
			void Expand(DV::Row row);
			void Expand(DV::Node& node, DV::Row row = {});
			void Collapse(DV::Row row);
			void Collapse(DV::Node& node, DV::Row row = {});
			void ToggleExpand(DV::Row row);
			bool IsExpanded(DV::Row row) const;
			bool HasChildren(DV::Row row) const;

			DV::Node* GetNodeByRow(DV::Row row) const;
			DV::Row GetRowByNode(const DV::Node& item) const;

			// Editing
			bool BeginEdit(DV::Node& item, DV::Column& column);
			void EndEdit();
			void CancelEdit();
	};
}
