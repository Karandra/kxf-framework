#include "kxf-pch.h"
#include "DataView.h"
#include "WXUI/DataView/View.h"
#include "WXUI/DataView/MainWindow.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Utility/Enumerator.h"
#include "kxf/Utility/Functional.h"

namespace kxf::Widgets
{
	// DataView
	DataView::DataView()
	{
		InitializeWxWidget();
	}
	DataView::~DataView() = default;

	// IWidget
	bool DataView::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IDataViewWidget
	FlagSet<IDataViewWidget::WidgetStyle> DataView::GetStyle() const
	{
		return Get()->GetStyle();
	}
	void DataView::SetStyle(FlagSet<WidgetStyle> style)
	{
		Get()->SetStyle(style);
	}

	DataView::Column* DataView::InsertColumn(size_t index, const String& title, WidgetID id, FlagSet<ColumnStyle> style)
	{
		auto column = std::make_unique<DataView::Column>(title, id, style);
		auto ref = column.get();
		if (Get()->InsertColumn(std::move(column), index) != npos)
		{
			return ref;
		}
		return nullptr;
	}
	void DataView::RemoveColumn(DataView::Column& column)
	{
		Get()->DeleteColumn(column);
	}
	void DataView::ClearColumns()
	{
		Get()->ClearColumns();
	}

	std::shared_ptr<IDataViewModel> DataView::GetDataModel() const
	{
		return Get()->m_ClientArea->m_Model;
	}
	void DataView::SetDataModel(std::shared_ptr<IDataViewModel> dataModel)
	{
		Get()->m_ClientArea->AssignModel(std::move(dataModel));
	}

	Color DataView::GetAlternateRowColor() const
	{
		return Get()->GetAlternateRowColor();
	}
	void DataView::SetAlternateRowColor(const Color& color)
	{
		Get()->SetAlternateRowColor(color);
	}

	int DataView::GetUniformRowHeight() const
	{
		return Get()->GetUniformRowHeight();
	}
	void DataView::SetUniformRowHeight(int rowHeight)
	{
		Get()->SetUniformRowHeight(rowHeight);
	}

	size_t DataView::GetColumnCount() const
	{
		return Get()->GetColumnCount();
	}
	size_t DataView::GetVisibleColumnCount() const
	{
		return Get()->GetVisibleColumnCount();
	}

	bool DataView::IsMultiColumnSortUsed() const
	{
		return Get()->IsMultiColumnSortUsed();
	}
	DataView::Column* DataView::GetSortingColumn() const
	{
		return Get()->GetSortingColumn();
	}
	Enumerator<DataView::Column&> DataView::GetSortingColumns() const
	{
		return Utility::EnumerateIterableContainer<DataView::Column&, Utility::ReferenceOf>(Get()->GetSortingColumns());
	}

	DataView::Column* DataView::GetCurrentColumn() const
	{
		return Get()->GetCurrentColumn();
	}
	DataView::Column* DataView::GetHotTrackColumn() const
	{
		return Get()->GetHotTrackedColumn();
	}
	DataView::Column* DataView::GetExpanderColumn() const
	{
		return Get()->GetExpanderColumn();
	}
	void DataView::SetExpanderColumn(DataView::Column& column)
	{
		Get()->SetExpanderColumn(column);
	}

	DataView::Column* DataView::GetColumnAt(size_t index) const
	{
		return Get()->GetColumnAt(index);
	}
	DataView::Column* DataView::GetColumnDisplayedAt(size_t index) const
	{
		return Get()->GetColumnDisplayedAt(index);
	}
	DataView::Column* DataView::GetColumnPhysicallyDisplayedAt(size_t index) const
	{
		return Get()->GetColumnPhysicallyDisplayedAt(index);
	}
	Enumerator<DataView::Column&> DataView::EnumColumns(ColumnOrder order) const
	{
		switch (order)
		{
			case ColumnOrder::Display:
			{
				return Utility::EnumerateIterableContainer<DataView::Column&, Utility::ReferenceOf>(Get()->GetColumnsInDisplayOrder());
			}
			case ColumnOrder::Physical:
			{
				return Utility::EnumerateIterableContainer<DataView::Column&, Utility::ReferenceOf>(Get()->GetColumnsInPhysicalDisplayOrder());
			}
			default:
			{
				return Utility::EnumerateIterableContainer<DataView::Column&, Utility::ReferenceOf, Utility::UnfancyPtr>(Get()->m_Columns);
			}
		};
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> DataView::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
