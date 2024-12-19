#include "kxf-pch.h"
#include "View.h"
#include "HeaderCtrl.h"
#include "MainWindow.h"
#include "kxf/System/SystemInformation.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"

namespace
{
	int FromDIPX(const wxWindow* window, int value) noexcept
	{
		return window->FromDIP(wxSize(value, wxDefaultCoord)).GetWidth();
	}
	int FromDIPY(const wxWindow* window, int value) noexcept
	{
		return window->FromDIP(wxSize(wxDefaultCoord, value)).GetHeight();
	}
}

namespace kxf::WXUI::DataView
{
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
			column->SetBestWidth(-1);
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
	bool View::HasColumn(const DV::Column& column) const
	{
		for (const auto& item: m_Columns)
		{
			if (item.get() == &column)
			{
				return true;
			}
		}
		return false;
	}

	void View::DoEnable(bool value)
	{
		ViewBase::DoEnable(value);
		ScheduleRefresh();
	}
	void View::ResetAllSortColumns()
	{
		for (auto& column: m_Columns)
		{
			column->SetSortOrder(SortOrder::None);
		}
	}
	void View::OnInternalIdle()
	{
		ViewBase::OnInternalIdle();
		m_ClientArea->OnInternalIdle();

		if (m_ColumnsDirty)
		{
			UpdateColumnsWidth();
		}
	}
	void View::DoEnableSystemTheme(bool enable, wxWindow* window)
	{
		m_UsingSystemTheme = enable;

		wxSystemThemedControlBase::DoEnableSystemTheme(enable, window);
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

		// We must redraw the headers if their height changed. Normally this
		// shouldn't happen as the control shouldn't let itself be resized beneath
		// its minimal height but avoid the display artifacts that appear if it
		// does happen, e.g. because there is really not enough vertical space.
		if (m_HeaderArea && m_HeaderArea->IsShown())
		{
			// The header size isn't controlled by sizers, so force it here.
			m_HeaderArea->SetSize(0, 0, event.GetSize().GetWidth(), FromDIPX(m_HeaderArea, m_HeaderHeight));

			if (m_HeaderArea->GetSize().GetY() <= m_HeaderArea->GetBestSize().GetY())
			{
				m_HeaderArea->ScheduleRefresh();
			}
		}
	}
	wxSize View::GetSizeAvailableForScrollTarget(const wxSize& size)
	{
		if (m_ClientArea)
		{
			return {size.GetWidth(), size.GetHeight() - m_ClientArea->GetHeaderOffset()};
		}
		return size;
	}

	std::vector<DV::Column*> View::DoGetColumnsInDisplayOrder(bool physicalOrder) const
	{
		std::vector<DV::Column*> displayOrder;
		displayOrder.reserve(m_Columns.size());

		for (auto& column: m_Columns)
		{
			if (!physicalOrder || column->IsVisible())
			{
				displayOrder.emplace_back(column.get());
			}
		}
		std::sort(displayOrder.begin(), displayOrder.end(), [](auto& left, auto& right)
		{
			return left->GetDisplayIndex() < right->GetDisplayIndex();
		});
		return displayOrder;
	}

	void View::MoveColumn(DV::Column& movedColumn, size_t newIndex)
	{
		// Do *not* reorder 'm_Columns' elements here, they should always be in the order in which columns
		// were added, we only display the columns in different order.
		if (movedColumn.GetDisplayIndex() != newIndex)
		{
			const size_t oldDisplayIndex = movedColumn.GetDisplayIndex();
			auto otherColumn = GetColumnDisplayedAt(newIndex);

			if (oldDisplayIndex < newIndex)
			{
				// Column moved to the left
				for (auto& column: m_Columns)
				{
					size_t displayIndex = column->GetDisplayIndex();
					if (displayIndex <= newIndex && displayIndex > oldDisplayIndex)
					{
						column->OnColumnAttached(*this, column->GetIndex(), displayIndex - 1);
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
						column->OnColumnAttached(*this, column->GetIndex(), displayIndex + 1);
					}
				}
			}

			// Set the new display position
			movedColumn.OnColumnAttached(*this, movedColumn.GetIndex(), newIndex);

			// Notify the header control
			OnColumnCountChanged();
		}
	}
	void View::MoveColumnToPhysicalIndex(DV::Column& movedColumn, size_t newIndex)
	{
		if (auto column = GetColumnPhysicallyDisplayedAt(newIndex))
		{
			MoveColumn(movedColumn, column->GetDisplayIndex());
		}
	}

	void View::OnColumnChange(DV::Column& column)
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

	View::View(IDataViewWidget& widget)
		:EvtHandlerWrapper(widget), m_Widget(widget)
	{
	}
	View::~View() = default;

	bool View::Create(wxWindow* parent,
					  const String& label,
					  const Point& pos,
					  const Size& size)
	{
		if (ViewBase::Create(parent, wxID_NONE, pos, size, wxVSCROLL|wxHSCROLL|wxWANTS_CHARS, "kxf::WXUI::DataViewWidget"))
		{
			m_HeaderHeight = 25;
			m_HeaderBorder = 1;

			m_ClientArea = std::make_unique<MainWindow>(this);
			if (!m_Style.Contains(WidgetStyle::NoHeader))
			{
				m_HeaderArea = new HeaderCtrl(*this);
			}

			EnableSystemTheme();
			SetBackgroundStyle(wxBG_STYLE_PAINT);
			SetBackgroundColour(System::GetColor(SystemColor::Window));
			Bind(wxEVT_SIZE, &View::OnSize, this);

			// We use the cursor keys for moving the selection, not scrolling, so call
			// this method to ensure wxScrollHelperEvtHandler doesn't catch all
			// keyboard events forwarded to us from 'MainWindow'.
			DisableKeyboardScrolling();

			return m_Widget.QueryInterface(m_RendererAware);
		}
		return false;
	}

	// Styles
	void View::SetStyle(FlagSet<WidgetStyle> style)
	{
		m_Style = style;
		ScheduleRefresh();

		if (m_HeaderArea)
		{
			bool shouldShow = !m_Style.Contains(WidgetStyle::NoHeader);
			if (shouldShow && !m_HeaderArea->IsShown())
			{
				m_HeaderArea->DoUpdate();
			}
			m_HeaderArea->Show(shouldShow);
		}

		// If disabling, must disable any multiple sort that are active
		if (!m_Style.Contains(WidgetStyle::MultiColumnSort))
		{
			ResetAllSortColumns();
			m_ClientArea->OnShouldResort();
		}
	}

	// Columns
	size_t View::InsertColumn(std::unique_ptr<DV::Column> column, size_t index)
	{
		if (column && !HasColumn(*column))
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

			// Insert
			column->OnColumnAttached(*this, index);
			m_Columns.emplace(m_Columns.begin() + index, std::move(column));
			OnColumnCountChanged();

			return index;
		}
		return IDataViewWidget::npos;
	}
	bool View::DeleteColumn(DV::Column& column)
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
				auto& currentColumn = *m_Columns[i];

				// Move actual index
				currentColumn.m_Index = currentColumn.GetIndex() - 1;

				// Move display index
				size_t index = currentColumn.GetDisplayIndex();
				if (index > displayIndex)
				{
					currentColumn.m_DisplayIndex = index - 1;
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

		m_ExpanderColumn = nullptr;
		m_Columns.clear();
		OnColumnCountChanged();

		return true;
	}

	DV::Column* View::GetColumnAt(size_t index) const
	{
		if (index < m_Columns.size())
		{
			return m_Columns[index].get();
		}
		return nullptr;
	}
	DV::Column* View::GetColumnByID(WidgetID columnID) const
	{
		if (!columnID.IsNone())
		{
			for (const auto& column: m_Columns)
			{
				if (column->GetID() == columnID)
				{
					return column.get();
				}
			}
		}
		return nullptr;
	}
	DV::Column* View::GetColumnDisplayedAt(size_t displayIndex) const
	{
		for (const auto& column: m_Columns)
		{
			if (column->GetDisplayIndex() == displayIndex)
			{
				return column.get();
			}
		}
		return nullptr;
	}
	DV::Column* View::GetColumnPhysicallyDisplayedAt(size_t displayIndex) const
	{
		// TODO: Figure out more optimal way to do this
		auto displayed = GetColumnsInPhysicalDisplayOrder();
		if (displayIndex < displayed.size())
		{
			return displayed[displayIndex];
		}
		return nullptr;
	}

	DV::Column* View::GetCurrentColumn() const
	{
		return m_ClientArea->GetCurrentColumn();
	}
	DV::Column* View::GetExpanderColumnOrFirstOne()
	{
		if (!m_ExpanderColumn)
		{
			// TODO-RTL: Last column for RTL support
			if (auto column = GetColumnDisplayedAt(0))
			{
				SetExpanderColumn(*column);
			}
		}
		return m_ExpanderColumn;
	}
	DV::Column* View::GetExpanderColumn() const
	{
		return m_ExpanderColumn;
	}
	void View::SetExpanderColumn(DV::Column& column)
	{
		if (HasColumn(column))
		{
			m_ExpanderColumn = &column;
			m_ExpanderColumn->SetBestWidth(-1);

			m_ClientArea->UpdateDisplay();
		}
	}

	bool View::IsMultiColumnSortUsed() const
	{
		size_t sortedCount = 0;
		for (const auto& column: m_Columns)
		{
			if (column->GetSortOrder() != SortOrder::None)
			{
				sortedCount++;
			}
		}
		return sortedCount > 1;
	}
	DV::Column* View::GetSortingColumn() const
	{
		for (auto& column: m_Columns)
		{
			if (column->GetSortOrder() != SortOrder::None)
			{
				return column.get();
			}
		}
		return nullptr;
	}
	std::vector<DV::Column*> View::GetSortingColumns() const
	{
		std::vector<DV::Column*> sortingColumns;
		for (auto& column: m_Columns)
		{
			if (column->GetSortOrder() != SortOrder::None)
			{
				sortingColumns.emplace_back(column.get());
			}
		}
		return sortingColumns;
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

	DV::Node* View::GetCurrentItem() const
	{
		if (m_Style.Contains(WidgetStyle::MultipleSelection))
		{
			return m_ClientArea->GetNodeByRow(m_ClientArea->GetCurrentRow());
		}
		else
		{
			return GetSelection();
		}
	}
	DV::Node* View::GetHotTrackedItem() const
	{
		return m_ClientArea->GetHotTrackItem();
	}
	DV::Column* View::GetHotTrackedColumn() const
	{
		return m_ClientArea->GetHotTrackColumn();
	}

	size_t View::GetSelectedCount() const
	{
		return m_ClientArea->GetSelections().GetSelectedCount();
	}
	DV::Node* View::GetSelection() const
	{
		const wxSelectionStore& selectionStore = m_ClientArea->GetSelections();
		if (selectionStore.GetSelectedCount() == 1)
		{
			wxSelectionStore::IterationState cookie = 0;
			return m_ClientArea->GetNodeByRow(selectionStore.GetFirstSelectedItem(cookie));
		}
		return nullptr;
	}
	size_t View::GetSelections(std::function<bool(DV::Node*)> func) const
	{
		const wxSelectionStore& selectionStore = m_ClientArea->GetSelections();

		size_t count = 0;
		wxSelectionStore::IterationState cookie;
		for (auto row = selectionStore.GetFirstSelectedItem(cookie); row != wxSelectionStore::NO_SELECTION; row = selectionStore.GetNextSelectedItem(cookie))
		{
			if (DV::Node* item = m_ClientArea->GetNodeByRow(row))
			{
				count++;
				if (!std::invoke(func, item))
				{
					break;
				}
			}
		}
		return count;
	}
	void View::SetSelections(const std::vector<DV::Node*>& selection)
	{
		m_ClientArea->ClearSelection();
		DV::Node* lastParent = nullptr;

		for (auto& node: selection)
		{
			DV::Node* parent = node->GetParentNode();
			if (lastParent && parent->IsSameAs(*lastParent))
			{
				node->DoExpandNodeAncestors();
			}

			lastParent = parent;
			if (DV::Row row = m_ClientArea->GetRowByNode(*node))
			{
				m_ClientArea->SelectRow(row);
			}
		}
	}

	void View::SelectAll()
	{
		m_ClientArea->SelectAllRows();
	}
	void View::UnselectAll()
	{
		m_ClientArea->UnselectAllRows();
	}

	void View::HitTest(const Point& point, DV::Node*& item, DV::Column*& column) const
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

	// Drag and drop
	bool View::EnableDND(std::unique_ptr<wxDataObjectSimple> dataObject, DNDOpType type, bool isPreferredDrop)
	{
		//return m_ClientArea->EnableDND(std::move(dataObject), type, isPreferredDrop);
		return false;
	}
	bool View::DisableDND(const wxDataFormat& format)
	{
		//return m_ClientArea->DisableDND(format);
		return true;
	}
	bool View::DisableDND(const wxDataObjectSimple& dataObject)
	{
		//return m_ClientArea->DisableDND(dataObject.GetFormat());
		return true;
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
			m_ClientArea->SetUniformRowHeight(m_ClientArea->GetDefaultRowHeight());
		}

		if (m_HeaderArea || m_ClientArea)
		{
			InvalidateColumnsBestWidth();
			Layout();
		}
		return true;
	}

	// Window
	#if 0
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
	#endif

	void View::DoSetVirtualSize(int x, int y)
	{
		if (x != m_virtualSize.GetX() || y != m_virtualSize.GetY())
		{
			ViewBase::DoSetVirtualSize(x, y);
		}
	}
	void View::ScrollWindow(int dx, int dy, const wxRect* rect)
	{
		if (dx != 0 || dy != 0)
		{
			auto updateRect = Rect(rect ? *rect : GetClientRect());
			if (m_HeaderArea)
			{
				m_HeaderArea->ScrollWidget(dx, dy, updateRect);
			}
			if (m_ClientArea)
			{
				m_ClientArea->ScrollWidget(dx, dy, updateRect);
			}

			// Don't call the base class function, since we don't want to physically scroll the window.
		}
	}

	// Control visuals
	void View::SetAlternateRowColor(const Color& color)
	{
		m_AlternateRowColor = color;
		ScheduleRefresh();
	}

	bool View::SetForegroundColour(const wxColour& color)
	{
		const bool b1 = ViewBase::SetForegroundColour(color);
		const bool b2 = m_HeaderArea ? m_HeaderArea->SetForegroundColour(color) : true;

		return b1 && b2;
	}
	bool View::SetBackgroundColour(const wxColour& color)
	{
		const  bool b1 = ViewBase::SetBackgroundColour(color);
		const  bool b2 = m_HeaderArea ? m_HeaderArea->SetBackgroundColour(color) : true;

		return b1 && b2;
	}
}
