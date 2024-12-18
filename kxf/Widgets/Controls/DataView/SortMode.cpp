#include "kxf-pch.h"
#include "SortMode.h"
#include "Column.h"
#include "View.h"

namespace kxf::UI::DataView
{
	SortMode::SortMode(const View& view) noexcept
	{
		if (view.IsMultiColumnSortUsed())
		{
			m_SortWhat = SortWhat::MultiColumn;
			m_SortingColumn = nullptr;
		}
		else if (const Column* column = view.GetSortingColumn())
		{
			m_SortWhat = SortWhat::SingleColumn;
			m_SortingColumn = column;
		}
	}
	SortMode::SortMode(const Column& column) noexcept
		:m_SortWhat(SortWhat::SingleColumn), m_SortingColumn(&column)
	{
	}

	bool SortMode::IsSortedAscending() const
	{
		return m_SortingColumn && m_SortingColumn->IsSortedAscending();
	}
	bool SortMode::IsSortedDescending() const
	{
		return m_SortingColumn && m_SortingColumn->IsSortedDescending();
	}
}
