#include "kxf-pch.h"
#include "IDataViewModel.h"
#include "Widgets/DataView/Node.h"
#include "Widgets/DataView/Column.h"

namespace kxf
{
	std::shared_ptr<IDataViewCellRenderer> IDataViewModel::GetCellRenderer(const DataView::Node& node, const DataView::Column& column) const
	{
		if (auto cellRenderer = node.GetItem()->GetCellRenderer(node, column))
		{
			return cellRenderer;
		}
		else
		{
			return column.GetCellRenderer();
		}
	}
	std::shared_ptr<IDataViewCellEditor> IDataViewModel::GetCellEditor(const DataView::Node& node, const DataView::Column& column) const
	{
		if (auto cellEditor = node.GetItem()->GetCellEditor(node, column))
		{
			return cellEditor;
		}
		else
		{
			return column.GetCellEditor();
		}
	}

	Any IDataViewModel::GetCellDisplayValue(const DataView::Node& node, const DataView::Column& column) const
	{
		return node.GetItem()->GetCellDisplayValue(node, column);
	}
	Any IDataViewModel::GetCellValue(const DataView::Node& node, const DataView::Column& column) const
	{
		return node.GetItem()->GetCellValue(node, column);
	}
	bool IDataViewModel::SetCellValue(DataView::Node& node, DataView::Column& column, Any value)
	{
		return node.GetItem()->SetCellValue(node, column, std::move(value));
	}

	DataView::ToolTip IDataViewModel::GetCellToolTip(const DataView::Node& node, const DataView::Column& column) const
	{
		return node.GetItem()->GetCellToolTip(node, column);
	}
	kxf::DataView::CellAttributes IDataViewModel::GetCellAttributes(const DataView::Node& node, const DataView::Column& column, const DataView::CellState& cellState) const
	{
		return node.GetItem()->GetCellAttributes(node, column, cellState);
	}
}
