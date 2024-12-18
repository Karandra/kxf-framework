#pragma once
#include "Common.h"

namespace kxf::UI::DataView
{
	class Column;
	class View;
}

namespace kxf::UI::DataView
{
	class KXF_API SortMode final
	{
		private:
			enum class SortWhat
			{
				None = -1,
				SingleColumn = 0,
				MultiColumn = 1,
			};

		private:
			const Column* m_SortingColumn = nullptr;
			SortWhat m_SortWhat = SortWhat::None;

		public:
			SortMode() noexcept = default;
			SortMode(const View& view) noexcept;
			SortMode(const Column& column) noexcept;

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

			const Column* GetSortingColumn() const noexcept
			{
				return m_SortingColumn;
			}
			bool IsSortedAscending() const;
			bool IsSortedDescending() const;

		public:
			bool operator==(const SortMode& other) const noexcept
			{
				return m_SortWhat == other.m_SortWhat && m_SortingColumn == other.m_SortingColumn;
			}
			bool operator!=(const SortMode& other) const noexcept
			{
				return !(*this == other);
			}

			explicit operator bool() const noexcept
			{
				return IsSortRequired();
			}
			bool operator!() const noexcept
			{
				return !IsSortRequired();
			}
	};
}
