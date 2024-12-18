#pragma once
#include "Common.h"
#include "View.h"
#include "Node.h"
#include "Row.h"
#include "Model.h"
#include "CellState.h"
#include "DragAndDrop.h"
#include "Renderers/NullRenderer.h"
#include "kxf-gui/Widgets/Windows/ToolTipEx.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
#include "kxf/EventSystem/Event.h"
#include <wx/selstore.h>

namespace kxf::UI::DataView
{
	class View;
	class Column;
	class Renderer;
	class Editor;
	class ToolTip;
	class HeaderCtrl;
	class ItemEvent;
	class VirtualListModel;

	class DropSource;
	class DropTarget;
}

namespace kxf::UI::DataView
{
	class KXF_API MainWindow: public WindowRefreshScheduler<wxWindow>
	{
		friend class Renderer;
		friend class Editor;
		friend class Column;
		friend class ItemEvent;
		friend class DragDropEvent;
		friend class View;
		friend class Node;
		friend class ToolTip;
		friend class HeaderCtrl;
		friend class HeaderCtrl2;
		friend class VirtualListModel;

		friend class DropSource;
		friend class DropTarget;
		friend class MaxWidthCalculator;

		public:
			enum: size_t
			{
				INVALID_ROW = std::numeric_limits<size_t>::max(),
				INVALID_COLUMN = std::numeric_limits<size_t>::max(),
				INVALID_COUNT = std::numeric_limits<size_t>::max(),
			};
			enum class InteractibleCell
			{
				Editor,
				Activator,
			};

		private:
			View* m_View = nullptr;

			int m_UniformRowHeight = 0;
			int m_Indent = 0;

			std::shared_ptr<IGraphicsRenderer> m_GraphicsRenderer;
			NullRenderer m_NullRenderer;
			Column* m_CurrentColumn = nullptr;
			Row m_CurrentRow;
			wxSelectionStore m_SelectionStore;

			bool m_LastOnSame = false;
			bool m_HasFocus = false;
			bool m_UseCellFocus = false;
			bool m_IsCurrentColumnSetByKeyboard = false;
			bool m_IsMouseOverExpander = false;

			bool m_Dirty = true;
			bool m_RedrawNeeded = false;

			Row m_HotTrackRow;
			bool m_HotTrackRowEnabled = false;
			Column* m_HotTrackColumn = nullptr;

			// Tooltip
			ToolTipEx m_ToolTip;
			wxTimer m_ToolTipTimer;

			// Drag and Drop
			DnDInfo m_DragDropInfo;
			wxDataObjectComposite* m_DragDropDataObject = nullptr;
			DropTarget* m_DropTarget = nullptr;
			DropSource* m_DragSource = nullptr;
			size_t m_DragCount = 0;

			Point m_DragStart;
			Row m_DropHintRow;
			bool m_DropHint = false;

			// Double click logic
			Row m_RowLastClicked;
			Row m_RowBeforeLastClicked;
			Row m_RowSelectSingleOnUp;

			// The pen used to draw horizontal/vertical rules
			std::shared_ptr<IGraphicsPen> m_PenRuleH;
			std::shared_ptr<IGraphicsPen> m_PenRuleV;

			// The pen used to draw the expander and the lines
			std::shared_ptr<IGraphicsPen> m_PenExpander;

			// Background bitmap
			BitmapImage m_BackgroundBitmap;
			FlagSet<Alignment> m_BackgroundBitmapAlignment = Alignment::Invalid;
			bool m_FitBackgroundBitmap = false;

			// Make 'm_ItemsCount' = -1 will cause the class recalculate the real displaying number of rows.
			size_t m_ItemsCount = INVALID_COUNT;
			std::shared_ptr<Model> m_Model;
			RootNode* m_TreeRoot = nullptr;

			// String to display when the control is empty
			String m_EmptyControlLabel;

			// This is the tree node under the cursor
			Node* m_TreeNodeUnderMouse = nullptr;

			// Current editor
			Editor* m_CurrentEditor = nullptr;

		protected:
			// Events
			void OnChar(wxKeyEvent& event);
			void OnCharHook(wxKeyEvent& event);
			void OnVerticalNavigation(const wxKeyEvent& event, int delta);
			void OnLeftKey(wxKeyEvent& event);
			void OnRightKey(wxKeyEvent& event);

			void OnMouse(wxMouseEvent& event);
			void OnSetFocus(wxFocusEvent& event);
			void OnKillFocus(wxFocusEvent& event);

			void OnTooltipEvent(wxTimerEvent& event);

			// Return false only if the event was vetoed by its handler.
			bool SendExpanderEvent(const EventID& type, Node& item);
			void SendSelectionChangedEvent(Node* item, Column* column = nullptr);

			// Will return true if event allowed
			bool SendEditingStartedEvent(Node& item, Editor* editor);
			bool SendEditingDoneEvent(Node& item, Editor* editor, bool canceled, const Any& value);

			// Drawing
			void OnPaint(wxPaintEvent& event);
			CellState GetCellStateForRow(Row row) const;

			void UpdateDisplay();
			void RefreshDisplay();
			void RecalculateDisplay();
			void DoSetVirtualSize(int x, int y) override;

			// Tooltip
			bool ShowToolTip(const Node& node, Column& column);
			void RemoveTooltip();

			// Columns
			void OnDeleteColumn(Column& column);
			void OnColumnCountChanged();
			bool IsCellInteractible(const Node& node, const Column& column, InteractibleCell action) const;
			Column* FindInteractibleColumn(const Node& node, InteractibleCell action);
			int CalcBestColumnWidth(Column& column) const;
			bool FitLastColumn(bool update = true);

			// Items
			void InvalidateItemCount();
			void UpdateItemCount(size_t count)
			{
				m_ItemsCount = count;
				m_SelectionStore.SetItemCount(count);
			}
			size_t RecalculateItemCount();

			void OnCellChanged(Node& node, Column* column);
			void OnNodeAdded(Node& node);
			void OnNodeRemoved(Node& node, intptr_t removedCount);
			void OnItemsCleared();
			void OnShouldResort();

			void DoAssignModel(std::shared_ptr<Model> model);
			bool IsListLike() const;

			// Misc
			void OnInternalIdle() override;

		public:
			MainWindow(View* parent, wxWindowID id);
			~MainWindow();

		public:
			void CreateEventTemplate(ItemEvent& event, Node* node = nullptr, Column* column = nullptr);

			// Model and nodes
			Model* GetModel()
			{
				return m_Model.get();
			}
			void SetModel(Model& model)
			{
				DoAssignModel(RTTI::assume_non_owned(model));
			}
			void AssignModel(std::unique_ptr<Model> model)
			{
				DoAssignModel(std::move(model));
			}

			const RootNode& GetRootNode() const
			{
				return *m_TreeRoot;
			}
			RootNode& GetRootNode()
			{
				return *m_TreeRoot;
			}
			void ItemsChanged();

			// View
			void SetView(View* owner)
			{
				m_View = owner;
			}
			View* GetView()
			{
				return m_View;
			}
			const View* GetView() const
			{
				return m_View;
			}

			Renderer& GetNullRenderer()
			{
				return m_NullRenderer;
			}
			const BitmapImage& GetBackgroundBitmap() const
			{
				return m_BackgroundBitmap;
			}
			void SetBackgroundBitmap(const BitmapImage& bitmap, FlagSet<Alignment> align = Alignment::Invalid, bool fit = false)
			{
				m_BackgroundBitmap = bitmap;
				m_BackgroundBitmapAlignment = align != Alignment::Invalid ? align : Alignment::None;
				m_FitBackgroundBitmap = fit;

				Refresh();
			}

			// Refreshing
			void RefreshRow(Row row)
			{
				RefreshRows(row, row);
			}
			void RefreshRows(Row from, Row to);
			void RefreshRowsAfter(Row firstRow);
			void RefreshColumn(const Column& column);

			// Item rect
			Rect GetRowRect(Row row) const;
			Rect GetRowsRect(Row rowFrom, Row rowTo) const;

			int GetRowStart(Row row) const;
			int GetRowHeight(Row row) const;
			int GetVariableRowHeight(const Node& node) const;
			int GetVariableRowHeight(Row row) const;
			int GetRowWidth() const;
			Row GetRowAt(int yCoord) const;

			int GetUniformRowHeight() const
			{
				return m_UniformRowHeight;
			}
			void SetUniformRowHeight(int height);
			int GetDefaultRowHeight(UniformHeight type = UniformHeight::Default) const;

			// Drag and Drop
			GDIBitmap CreateItemBitmap(Row row, int& indent);
			bool EnableDND(std::unique_ptr<wxDataObjectSimple> dataObject, DNDOpType type, bool isPreferredDrop = false);
			bool DisableDND(const wxDataFormat& format);

			std::tuple<Row, Node*> DragDropHitTest(const Point& pos) const;
			void RemoveDropHint();
			wxDragResult OnDragOver(const wxDataObjectSimple& dataObject, const Point& pos, wxDragResult dragResult);
			wxDragResult OnDropData(wxDataObjectSimple& dataObject, const Point& pos, wxDragResult dragResult);
			bool TestDropPossible(const wxDataObjectSimple& dataObject, const Point& pos);

			wxDragResult OnDragDropEnter(const wxDataObjectSimple& format, const Point& pos, wxDragResult dragResult);
			void OnDragDropLeave();

			// Scrolling
			void ScrollWindow(int dx, int dy, const wxRect* rect = nullptr) override;
			void ScrollTo(Row row, size_t column = INVALID_COLUMN);
			void EnsureVisible(Row row, size_t column = INVALID_COLUMN);

			// Current row and column
			Row GetCurrentRow() const
			{
				return m_CurrentRow;
			}
			bool HasCurrentRow()
			{
				return !m_CurrentRow.IsNull();
			}
			void ChangeCurrentRow(Row row);
			bool TryAdvanceCurrentColumn(Node* node, wxKeyEvent& event, bool moveForward);

			Column* GetCurrentColumn() const
			{
				return m_CurrentColumn;
			}
			void ClearCurrentColumn()
			{
				m_CurrentColumn = nullptr;
			}

			Node* GetHotTrackItem() const;
			Column* GetHotTrackColumn() const;

			// Selection
			bool IsSingleSelection() const
			{
				return !IsMultipleSelection();
			}
			bool IsMultipleSelection() const
			{
				return m_View->ContainsWindowStyle(CtrlStyle::MultipleSelection);
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
			bool UnselectAllRows(Row exceptThisRow = {});
			void ReverseRowSelection(Row row);
			void ClearSelection()
			{
				m_SelectionStore.SelectRange(0, GetRowCount() - 1, false);
			}
			void SelectRow(Row row, bool select = true);
			void SelectRows(Row from, Row to);
			void SelectRows(const Row::Vector& selection);
			void SelectAllRows()
			{
				m_SelectionStore.SelectRange(0, GetRowCount() - 1);
				Refresh();
			}
			bool IsRowSelected(Row row) const
			{
				return m_SelectionStore.IsSelected(*row);
			}

			// View
			IGraphicsRenderer& GetGraphicsRenderer()
			{
				return *m_GraphicsRenderer;
			}
			void SetRuleHPen(const Color& color, float width = 1.0f)
			{
				m_PenRuleH = m_GraphicsRenderer->CreatePen(color, width);
			}
			void SetRuleVPen(const Color& color, float width = 1.0f)
			{
				m_PenRuleV = m_GraphicsRenderer->CreatePen(color, width);
			}

			size_t GetRowCount() const;
			size_t GetCountPerPage() const;
			Row GetFirstVisibleRow() const;
			Row GetLastVisibleRow() const;

			void HitTest(const Point& pos, Node** nodeOut = nullptr, Column** columnOut = nullptr);
			void HitTest(const Point& pos, Node*& node, Column*& column)
			{
				return HitTest(pos, &node, &column);
			}
			Node* HitTestNode(const Point& pos)
			{
				Node* node = nullptr;
				HitTest(pos, &node);
				return node;
			}
			Column* HitTestColumn(const Point& pos)
			{
				Column* column = nullptr;
				HitTest(pos, nullptr, &column);
				return column;
			}
			Rect GetItemRect(const Node& item, const Column* column = nullptr);

			// Rows
			void Expand(Row row);
			void Expand(Node& node, Row row = {});
			void Collapse(Row row);
			void Collapse(Node& node, Row row = {});
			void ToggleExpand(Row row);
			bool IsExpanded(Row row) const;
			bool HasChildren(Row row) const;

			Node* GetNodeByRow(Row row) const;
			Row GetRowByNode(const Node& item) const;

			// Editing
			bool BeginEdit(Node& item, Column& column);
			void EndEdit();
			void CancelEdit();

		public:
			wxDECLARE_DYNAMIC_CLASS(MainWindow);
	};
}
