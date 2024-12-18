#pragma once
#include "Common.h"

namespace kxf
{
	class IDataViewWidget;
}

namespace kxf::DataView
{
	class KXF_API SortMode final
	{
		private:
			enum class SortWhat
			{
				None = -1,
				SingleColumn = 0,
				MultiColumn = 1
			};

		private:
			SortWhat m_SortWhat = SortWhat::None;
			SortOrder m_SortOrder = SortOrder::None;

		public:
			SortMode() noexcept = default;
			SortMode(const IDataViewWidget& widget) noexcept;
			SortMode(DataView::Column& column) noexcept;

		public:
			bool IsSortRequired() const noexcept
			{
				return m_SortWhat != SortWhat::None;
			}
			bool IsSingleColumn() const noexcept
			{
				return m_SortWhat == SortWhat::SingleColumn;
			}
			bool IsMultiColumn() const noexcept
			{
				return m_SortWhat == SortWhat::MultiColumn;
			}

			SortOrder GetSortOrder() const noexcept
			{
				return m_SortOrder;
			}

		public:
			bool operator==(const SortMode&) const noexcept = default;
	};
}
