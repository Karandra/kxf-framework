#include "kxf-pch.h"
#include "View.h"
#include "HeaderCtrl.h"
#include "HeaderCtrl2.h"
#include "MainWindow.h"
#include "Column.h"
#include "Renderer.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIWindowContext.h"

namespace
{
	int FromDIPX(const wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(value, wxDefaultCoord)).GetWidth();
	}
	int FromDIPY(const wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(wxDefaultCoord, value)).GetHeight();
	}
}

namespace kxf::UI::DataView
{
	wxIMPLEMENT_ABSTRACT_CLASS(View, wxWindow)

	bool View::MSWHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		const bool ret = ViewBase::MSWHandleMessage(result, msg, wParam, lParam);
		switch (msg)
		{
			case WM_GETDLGCODE:
			{
				// We need to process arrows ourselves for scrolling
				*result |= DLGC_WANTARROWS;
				break;
			}
		};
		return ret;
	}

	void View::InvalidateColumnsBestWidth()
	{
		for (auto& column: m_Columns)
		{
			column->InvalidateBestWidth();
		}
	}
	void View::UpdateColumnsWidth()
	{
		m_ColumnsDirty = false;

		if (m_HeaderArea)
		{
			if (m_HeaderArea->CanUpdateSingleColumn())
			{
				for (auto& column: m_Columns)
				{
					// Note that we have to have an explicit 'dirty' flag here instead of
					// checking if the width == 0, as is done in 'MainWindow::CalcBestColumnWidth'.

					// Testing width == 0 wouldn't work correctly if some code called
					// 'GetWidth' after column width invalidation but before
					// 'View::UpdateColumnsWidth' was called at idle time. This
					// would result in the header's column width getting out of sync with
					// the control itself.

					if (column->IsDirty())
					{
						m_HeaderArea->UpdateColumn(*column);
						column->MarkDirty(false);
					}
				}
			}
			else
			{
				m_HeaderArea->DoUpdate();
			}
		}
	}

	void View::DoEnable(bool value)
	{
		ViewBase::DoEnable(value);
		Refresh();
	}
	void View::DoInsertColumn(std::unique_ptr<Column> column, size_t index)
	{
		// Correct insertion position
		if (m_Columns.empty())
		{
			index = 0;
		}
		else if (index > m_Columns.size())
		{
			index = m_Columns.size();
		}

		// Set column data
		column->AssignIndex(index);
		column->AssignDisplayIndex(index);
		column->SetView(this);

		// Insert
		m_Columns.emplace(m_Columns.begin() + index, std::move(column));
		OnColumnCountChanged();
	}
	void View::ResetAllSortColumns()
	{
		for (const auto& column: m_Columns)
		{
			column->ResetSorting();
		}
	}
	void View::OnInternalIdle()
	{
		ViewBase::OnInternalIdle();

		if (m_ColumnsDirty)
		{
			UpdateColumnsWidth();
		}
	}
	void View::DoEnableSystemTheme(bool enable, wxWindow* window)
	{
		m_UsingSystemTheme = enable;

		wxSystemThemedControlBase::DoEnableSystemTheme(enable, window);
		wxSystemThemedControlBase::DoEnableSystemTheme(enable, m_ClientArea);
		if (m_HeaderArea)
		{
			wxSystemThemedControlBase::DoEnableSystemTheme(enable, m_HeaderArea);
		}
	}

	void View::OnSize(wxSizeEvent& event)
	{
		if (m_ClientArea && GetColumnCount() != 0)
		{
			m_ClientArea->FitLastColumn();
		}

		// We need to override OnSize so that our scrolled window
		// a) does call Layout() to use sizers for positioning the controls but
		// b) does not query the sizer for their size and use that for setting
		// the scrollable area as set that ourselves by calling SetScrollbar() further down.

		AdjustScrollbars();
		Layout();

		// We must redraw the headers if their height changed. Normally this
		// shouldn't happen as the control shouldn't let itself be resized beneath
		// its minimal height but avoid the display artifacts that appear if it
		// does happen, e.g. because there is really not enough vertical space.
		if (m_HeaderArea && m_HeaderAreaSI->IsShown())
		{
			if (m_HeaderArea->GetSize().GetY() <= m_HeaderArea->GetBestSize().GetY())
			{
				m_HeaderArea->ScheduleRefresh();
			}
		}
	}
	void View::OnPaint(wxPaintEvent& event)
	{
		IGraphicsRenderer& renderer = *m_ClientArea->m_GraphicsRenderer;

		auto gc = renderer.CreateLegacyWindowPaintContext(*this);
		gc->SetAntialiasMode(AntialiasMode::None);
		gc->SetInterpolationQuality(InterpolationQuality::NearestNeighbor);

		auto brush = renderer.CreateSolidBrush(m_BorderColor ? m_BorderColor : Color(GetBackgroundColour()));
		gc->Clear(*brush);

		if (m_HeaderAreaSpacerSI && m_HeaderAreaSpacerSI->IsShown())
		{
			brush = renderer.CreateSolidBrush(m_ClientArea->m_PenRuleH->GetColor());

			RectF rect = Rect(m_HeaderAreaSpacerSI->GetRect());
			rect.SetWidth(GetClientSize().GetWidth());
			gc->DrawRectangle(rect, *brush, *m_ClientArea->m_PenRuleH);
		}
	}
	wxSize View::GetSizeAvailableForScrollTarget(const wxSize& size)
	{
		Size newSize = Size(size);
		if (m_HeaderArea)
		{
			newSize.Height() -= m_HeaderArea->GetSize().GetHeight();
		}
		return newSize;
	}

	Column::RefVector View::DoGetColumnsInDisplayOrder(bool physicalOrder) const
	{
		std::vector<Column*> displayOrder;
		displayOrder.reserve(m_Columns.size());

		for (const auto& column: m_Columns)
		{
			if (!physicalOrder || column->IsVisible())
			{
				displayOrder.push_back(column.get());
			}
		}
		std::sort(displayOrder.begin(), displayOrder.end(), [](Column* left, Column* right)
		{
			return left->GetDisplayIndex() < right->GetDisplayIndex();
		});
		return displayOrder;
	}

	void View::MoveColumn(Column& movedColumn, size_t newIndex)
	{
		// Do *not* reorder 'm_Columns' elements here, they should always be in the order in which columns
		// were added, we only display the columns in different order.
		if (movedColumn.GetDisplayIndex() != newIndex)
		{
			const size_t oldDisplayIndex = movedColumn.GetDisplayIndex();
			Column& otherColumn = *GetColumnDisplayedAt(newIndex);

			if (oldDisplayIndex < newIndex)
			{
				// Column moved to the left
				for (auto& column: m_Columns)
				{
					size_t displayIndex = column->GetDisplayIndex();
					if (displayIndex <= newIndex && displayIndex > oldDisplayIndex)
					{
						column->AssignDisplayIndex(displayIndex - 1);
					}
				}
			}
			else
			{
				// Column moved to the right
				for (auto& column: m_Columns)
				{
					size_t displayIndex = column->GetDisplayIndex();
					if (displayIndex >= newIndex && displayIndex < oldDisplayIndex)
					{
						column->AssignDisplayIndex(displayIndex + 1);
					}
				}
			}

			// Set the new display position
			movedColumn.AssignDisplayIndex(newIndex);

			// Notify the header control
			OnColumnCountChanged();
		}
	}
	void View::MoveColumnToPhysicalIndex(Column& movedColumn, size_t newIndex)
	{
		if (Column* column = GetColumnPhysicallyDisplayedAt(newIndex))
		{
			MoveColumn(movedColumn, column->GetDisplayIndex());
		}
	}

	void View::OnColumnChange(Column& column)
	{
		if (m_HeaderArea)
		{
			m_HeaderArea->UpdateColumn(column);
		}
		m_ClientArea->UpdateDisplay();
	}
	void View::OnColumnCountChanged()
	{
		if (m_HeaderArea)
		{
			m_HeaderArea->UpdateColumnCount();
		}
		m_ClientArea->OnColumnCountChanged();
	}

	bool View::Create(wxWindow* parent, wxWindowID id, const Point& pos, const Size& size, FlagSet<CtrlStyle> style, const String& name)
	{
		m_Styles = *style|(WindowStyle::ScrollHorizontal|WindowStyle::ScrollVertical);
		SetWindowExStyle(m_ExtraStyles);

		if (ViewBase::Create(parent, id, pos, size, m_Styles.ToInt(), GetClassInfo()->GetClassName()))
		{
			m_ClientArea = new MainWindow(this, wxID_NONE);

			// We use the cursor keys for moving the selection, not scrolling, so call
			// this method to ensure wxScrollHelperEvtHandler doesn't catch all
			// keyboard events forwarded to us from 'MainWindow'.
			DisableKeyboardScrolling();

			if (!m_Styles.Contains(CtrlStyle::NoHeader))
			{
				m_HeaderArea = new HeaderCtrl(this);
			}
			SetTargetWindow(m_ClientArea);

			m_Sizer = new wxBoxSizer(wxVERTICAL);
			if (m_HeaderArea)
			{
				m_HeaderAreaSI = m_Sizer->Add(m_HeaderArea, 0, wxEXPAND);
				m_HeaderAreaSI->SetMinSize(m_HeaderArea->FromDIP(wxSize(wxDefaultCoord, 25)));

				m_HeaderAreaSpacerSI = m_Sizer->AddSpacer(1);
			}
			m_ClientAreaSI = m_Sizer->Add(m_ClientArea, 1, wxEXPAND|wxLEFT);
			SetSizer(m_Sizer);

			EnableSystemTheme();
			SetBackgroundStyle(wxBG_STYLE_PAINT);
			Bind(wxEVT_SIZE, &View::OnSize, this);
			Bind(wxEVT_PAINT, &View::OnPaint, this);
			return true;
		}
		return false;
	}
	View::~View()
	{
	}

	// Styles
	long View::GetWindowStyleFlag() const
	{
		return m_Styles.ToInt();
	}
	void View::SetWindowStyleFlag(long styles)
	{
		m_Styles.FromInt(styles);
		if (m_HeaderAreaSI)
		{
			bool shouldShow = !m_Styles.Contains(CtrlStyle::NoHeader);
			if (shouldShow && !m_HeaderAreaSI->IsShown())
			{
				m_HeaderArea->DoUpdate();
			}
			m_HeaderAreaSI->Show(shouldShow);
			m_HeaderAreaSpacerSI->Show(shouldShow);
		}

		ViewBase::SetWindowStyleFlag(styles);
	}
	void View::SetExtraStyle(long styles)
	{
		m_ExtraStyles.FromInt(styles);
		ViewBase::SetExtraStyle(styles);
	}

	// Model
	Model* View::GetModel()
	{
		return m_ClientArea->GetModel();
	}
	void View::SetModel(Model& model)
	{
		m_ClientArea->SetModel(model);
	}
	void View::AssignModel(std::unique_ptr<Model> model)
	{
		m_ClientArea->AssignModel(std::move(model));
	}

	RootNode& View::GetRootNode()
	{
		return m_ClientArea->GetRootNode();
	}
	const RootNode& View::GetRootNode() const
	{
		return m_ClientArea->GetRootNode();
	}
	void View::ItemsChanged()
	{
		m_ClientArea->ItemsChanged();
	}

	// Columns
	Renderer& View::AppendColumn(std::unique_ptr<Column> column)
	{
		Renderer& renderer = column->GetRenderer();
		DoInsertColumn(std::move(column), GetColumnCount());

		return renderer;
	}
	Renderer& View::PrependColumn(std::unique_ptr<Column> column)
	{
		Renderer& renderer = column->GetRenderer();
		DoInsertColumn(std::move(column), 0);

		return renderer;
	}
	Renderer& View::InsertColumn(size_t index, std::unique_ptr<Column> column)
	{
		Renderer& renderer = column->GetRenderer();
		DoInsertColumn(std::move(column), index);

		return renderer;
	}

	size_t View::GetVisibleColumnCount() const
	{
		size_t count = 0;
		for (const auto& column: m_Columns)
		{
			if (column->IsVisible())
			{
				count++;
			}
		}
		return count;
	}

	Column* View::GetColumn(size_t position) const
	{
		if (position < m_Columns.size())
		{
			return m_Columns[position].get();
		}
		return nullptr;
	}
	Column* View::GetColumnByID(ColumnID columnID) const
	{
		for (const auto& column: m_Columns)
		{
			if (column->GetID() == columnID)
			{
				return column.get();
			}
		}
		return nullptr;
	}
	Column* View::GetColumnDisplayedAt(size_t displayIndex) const
	{
		// Columns can't be reordered if there is no header window which allows to do this
		if (HasHeaderCtrl())
		{
			for (const auto& column: m_Columns)
			{
				if (column->GetDisplayIndex() == displayIndex)
				{
					return column.get();
				}
			}
		}
		return GetColumn(displayIndex);
	}
	Column* View::GetColumnPhysicallyDisplayedAt(size_t displayIndex) const
	{
		// Columns can't be reordered if there is no header window which allows to do this
		if (HasHeaderCtrl())
		{
			Column::RefVector displayed = GetColumnsInPhysicalDisplayOrder();
			if (displayIndex < displayed.size())
			{
				return displayed[displayIndex];
			}
			return nullptr;
		}
		return GetColumn(displayIndex);
	}

	bool View::DeleteColumn(Column& column)
	{
		size_t nextColumn = INVALID_COLUMN;
		size_t displayIndex = INVALID_COLUMN;
		for (auto it = m_Columns.begin(); it != m_Columns.end(); ++it)
		{
			if (it->get() == &column)
			{
				nextColumn = column.GetIndex();
				displayIndex = column.GetDisplayIndex();

				if (&column == m_ExpanderColumn)
				{
					m_ExpanderColumn = nullptr;
				}

				m_ClientArea->OnDeleteColumn(column);
				m_Columns.erase(it);
				break;
			}
		}

		// Column was removed, update display indexes
		if (nextColumn != INVALID_COLUMN)
		{
			for (size_t i = nextColumn; i < m_Columns.size(); i++)
			{
				Column& currentColumn = *m_Columns[i];

				// Move actual index
				currentColumn.AssignIndex(currentColumn.GetIndex() - 1);

				// Move display index
				size_t index = currentColumn.GetDisplayIndex();
				if (index > displayIndex)
				{
					currentColumn.AssignDisplayIndex(index - 1);
				}
			}

			OnColumnCountChanged();
			return true;
		}
		return false;
	}
	bool View::ClearColumns()
	{
		for (auto& column: m_Columns)
		{
			m_ClientArea->OnDeleteColumn(*column);
		}

		SetExpanderColumn(nullptr);
		m_Columns.clear();
		OnColumnCountChanged();

		return true;
	}

	Column* View::GetCurrentColumn() const
	{
		return m_ClientArea->GetCurrentColumn();
	}
	Column* View::GetExpanderColumnOrFirstOne()
	{
		if (!m_ExpanderColumn)
		{
			// TODO-RTL: Last column for RTL support
			m_ExpanderColumn = GetColumnDisplayedAt(0);
			SetExpanderColumn(m_ExpanderColumn);
		}
		return m_ExpanderColumn;
	}
	Column* View::GetExpanderColumn() const
	{
		return m_ExpanderColumn;
	}
	void View::SetExpanderColumn(Column *column)
	{
		m_ExpanderColumn = column;
		if (column)
		{
			column->InvalidateBestWidth();
		}
		m_ClientArea->UpdateDisplay();
	}

	bool View::IsMultiColumnSortUsed() const
	{
		size_t sortedCount = 0;
		for (const auto& column: m_Columns)
		{
			if (column->IsSorted())
			{
				sortedCount++;
			}
		}
		return sortedCount > 1;
	}
	Column* View::GetSortingColumn() const
	{
		for (const auto& column: m_Columns)
		{
			if (column->IsSorted())
			{
				return column.get();
			}
		}
		return nullptr;
	}
	Column::RefVector View::GetSortingColumns() const
	{
		Column::RefVector sortingColumns;
		for (const auto& column: m_Columns)
		{
			if (column->IsSorted())
			{
				sortingColumns.emplace_back(column.get());
			}
		}
		return sortingColumns;
	}

	bool View::AllowMultiColumnSort(bool allow)
	{
		if (m_AllowMultiColumnSort == allow)
		{
			return true;
		}
		m_AllowMultiColumnSort = allow;

		// If disabling, must disable any multiple sort that are active
		if (!allow)
		{
			ResetAllSortColumns();
			m_ClientArea->OnShouldResort();
		}
		return true;
	}
	void View::ToggleSortByColumn(size_t index)
	{
		m_HeaderArea->ToggleSortByColumn(index);
	}

	int View::GetIndent() const
	{
		return m_ClientArea->m_Indent;
	}
	void View::SetIndent(int indent)
	{
		m_ClientArea->m_Indent = indent;
		m_ClientArea->UpdateDisplay();
	}

	Node* View::GetCurrentItem() const
	{
		if (m_Styles.Contains(CtrlStyle::MultipleSelection))
		{
			return m_ClientArea->GetNodeByRow(m_ClientArea->GetCurrentRow());
		}
		else
		{
			return GetSelection();
		}
	}
	Node* View::GetHotTrackedItem() const
	{
		return m_ClientArea->GetHotTrackItem();
	}
	Column* View::GetHotTrackedColumn() const
	{
		return m_ClientArea->GetHotTrackColumn();
	}

	size_t View::GetSelectedCount() const
	{
		return m_ClientArea->GetSelections().GetSelectedCount();
	}
	Node* View::GetSelection() const
	{
		const wxSelectionStore& selectionStore = m_ClientArea->GetSelections();
		if (selectionStore.GetSelectedCount() == 1)
		{
			wxSelectionStore::IterationState cookie = 0;
			return m_ClientArea->GetNodeByRow(selectionStore.GetFirstSelectedItem(cookie));
		}
		return nullptr;
	}
	size_t View::GetSelections(std::function<bool(Node&)> func) const
	{
		const wxSelectionStore& selectionStore = m_ClientArea->GetSelections();

		size_t count = 0;
		wxSelectionStore::IterationState cookie;
		for (auto row = selectionStore.GetFirstSelectedItem(cookie); row != wxSelectionStore::NO_SELECTION; row = selectionStore.GetNextSelectedItem(cookie))
		{
			if (Node* item = m_ClientArea->GetNodeByRow(row))
			{
				count++;
				if (!std::invoke(func, *item))
				{
					break;
				}
			}
		}
		return count;
	}
	void View::SetSelections(const std::vector<Node*>& selection)
	{
		m_ClientArea->ClearSelection();
		Node* lastParent = nullptr;

		for (Node* item: selection)
		{
			Node* parent = item->GetParentNode();
			if (parent)
			{
				if (parent != lastParent)
				{
					item->DoExpandNodeAncestors();
				}
			}

			lastParent = parent;
			if (Row row = m_ClientArea->GetRowByNode(*item))
			{
				m_ClientArea->SelectRow(row);
			}
		}
	}

	void View::GenerateSelectionEvent(Node& item, const Column* column)
	{
		m_ClientArea->SendSelectionChangedEvent(&item, const_cast<Column*>(column));
	}

	void View::SelectAll()
	{
		m_ClientArea->SelectAllRows();
	}
	void View::UnselectAll()
	{
		m_ClientArea->UnselectAllRows();
	}

	void View::HitTest(const Point& point, Node*& item, Column*& column) const
	{
		m_ClientArea->HitTest(point, item, column);
	}

	int View::GetUniformRowHeight() const
	{
		return m_ClientArea->GetUniformRowHeight();
	}
	void View::SetUniformRowHeight(int rowHeight)
	{
		m_ClientArea->SetUniformRowHeight(rowHeight);
	}
	int View::GetDefaultRowHeight(UniformHeight type) const
	{
		return m_ClientArea->GetDefaultRowHeight(type);
	}

	// Drag and drop
	bool View::EnableDND(std::unique_ptr<wxDataObjectSimple> dataObject, DNDOpType type, bool isPreferredDrop)
	{
		return m_ClientArea->EnableDND(std::move(dataObject), type, isPreferredDrop);
	}
	bool View::DisableDND(const wxDataFormat& format)
	{
		return m_ClientArea->DisableDND(format);
	}
	bool View::DisableDND(const wxDataObjectSimple& dataObject)
	{
		return m_ClientArea->DisableDND(dataObject.GetFormat());
	}

	void View::SetFocus()
	{
		if (m_ClientArea)
		{
			m_ClientArea->SetFocus();
		}
	}
	bool View::SetFont(const wxFont& font)
	{
		if (!ViewBase::SetFont(font))
		{
			return false;
		}

		if (m_HeaderArea)
		{
			m_HeaderArea->SetFont(font);
		}
		if (m_ClientArea)
		{
			m_ClientArea->SetFont(font);
			m_ClientArea->SetUniformRowHeight(m_ClientArea->GetDefaultRowHeight());
		}

		if (m_HeaderArea || m_ClientArea)
		{
			InvalidateColumnsBestWidth();
			Layout();
		}
		return true;
	}
	void View::InitDialog()
	{
		ViewBase::InitDialog();
	}

	// Window
	bool View::CreateColumnSelectionMenu(Menu& menu)
	{
		/*
		size_t count = GetColumnCount();
		for (size_t i = 0; i < count; i++)
		{
			Column* column = GetColumn(i);
			String title = column->GetTitle();
			if (title.IsEmpty())
			{
				title = Format("<{}>", i + 1);
			}

			MenuItem* menuItem = menu.AddItem(title, NullString, wxITEM_CHECK);
			menuItem->Check(column->IsVisible());
			menuItem->SetBitmap(column->m_Bitmap.ToWxBitmap());
			menuItem->SetClientData(column);
		}
		return menu.GetMenuItemCount() != 0;
		*/
		return false;
	}
	Column* View::OnColumnSelectionMenu(Menu& menu)
	{
		/*
		wxWindowID retID = menu.Show(this);
		if (retID != wxID_NONE)
		{
			MenuItem* menuItem = menu.FindItem(retID);
			Column* column = static_cast<Column*>(menuItem->GetClientData());
			column->AssignVisible(menuItem->IsChecked());

			if (column->IsVisible() && column->GetWidth() == 0)
			{
				column->AssignWidth(column->GetBestWidth());
			}

			ItemEvent event(ItemEvent::EvtColumnHeaderMenuItem);
			m_ClientArea->CreateEventTemplate(event, nullptr, column);
			ProcessWindowEvent(event);

			OnColumnChange(*column);
			return column;
		}
		return nullptr;
		*/
		return nullptr;
	}

	String View::GetEmptyControlLabel() const
	{
		return m_ClientArea->m_EmptyControlLabel;
	}
	void View::SetEmptyControlLabel(const String& value)
	{
		m_ClientArea->m_EmptyControlLabel = value;
		if (m_ClientArea->IsEmpty())
		{
			m_ClientArea->ScheduleRefresh();
		}
	}

	// Control visuals
	wxBorder View::GetDefaultBorder() const
	{
		return wxBORDER_THEME;
	}
	void View::SetBorderColor(const Color& color, int size)
	{
		m_BorderColor = color;
		int borderSize = m_BorderColor ? FromDIP(size) : 0;

		m_ClientAreaSI->SetBorder(borderSize);
		if (m_HeaderAreaSI)
		{
			m_HeaderAreaSI->SetBorder(borderSize);
		}
		Refresh();
	}

	bool View::SetForegroundColour(const wxColour& color)
	{
		const bool b1 = ViewBase::SetForegroundColour(color);
		const bool b2 = m_ClientArea ? m_ClientArea->SetForegroundColour(color) : true;
		const bool b3 = m_HeaderArea ? m_HeaderArea->SetForegroundColour(color) : true;

		return b1 && b2 && b3;
	}
	bool View::SetBackgroundColour(const wxColour& color)
	{
		const  bool b1 = ViewBase::SetBackgroundColour(color);
		const  bool b2 = m_ClientArea ? m_ClientArea->SetBackgroundColour(color) : true;
		const  bool b3 = m_HeaderArea ? m_HeaderArea->SetBackgroundColour(color) : true;

		return b1 && b2 && b3;
	}

	BitmapImage View::GetBackgroundBitmap() const
	{
		return m_ClientArea->GetBackgroundBitmap();
	}
	void View::SetBackgroundBitmap(const BitmapImage& bitmap, FlagSet<Alignment> align, bool fit)
	{
		m_ClientArea->SetBackgroundBitmap(bitmap, align, fit);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> View::GetActiveGraphicsRenderer() const
	{
		return m_ClientArea->m_GraphicsRenderer;
	}
	void View::SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer)
	{
		if (!renderer)
		{
			renderer = Drawing::GetDefaultRenderer();
		}
		if (m_ClientArea->m_GraphicsRenderer != renderer)
		{
			m_PendingGraphicsRenderer = std::move(renderer);
		}
	}
}
