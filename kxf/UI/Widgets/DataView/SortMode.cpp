#include "kxf-pch.h"
#include "SortMode.h"
#include "Column.h"
#include "../../IDataViewWidget.h"

namespace kxf::DataView
{
	SortMode::SortMode(const IDataViewWidget& widget) noexcept
	{
		if (widget.IsMultiColumnSortUsed())
		{
			m_SortWhat = SortWhat::MultiColumn;
		}
		else if (auto column = widget.GetSortingColumn())
		{
			m_SortWhat = SortWhat::SingleColumn;
			m_SortOrder = column->GetSortOrder();
		}
	}
	SortMode::SortMode(DataView::Column& column) noexcept
		:m_SortWhat(SortWhat::SingleColumn), m_SortOrder(column.GetSortOrder())
	{
	}
}
