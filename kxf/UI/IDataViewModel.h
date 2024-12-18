#pragma once
#include "Common.h"
#include "kxf/Core/Any.h"

namespace kxf
{
	class IDataViewItem;
	class IDataViewCellEditor;
	class IDataViewCellRenderer;
}
namespace kxf::DataView
{
	class Node;
	class Column;
	class CellEditor;
	class CellRenderer;

	class ToolTip;
	class SortMode;
	class CellState;
	class CellAttributes;
}

namespace kxf
{
	class KXF_API IDataViewModel: public RTTI::Interface<IDataViewModel>
	{
		kxf_RTTI_DeclareIID(IDataViewModel, {0x96657379, 0x220e, 0x4df1, {0xa8, 0x4d, 0x9d, 0x66, 0xd9, 0x4e, 0xa, 0xa3}});

		public:
			virtual void OnSortChildren(const DataView::SortMode& sortMode)
			{
			}

			virtual size_t GetChildrenCount() const = 0;
			virtual std::shared_ptr<IDataViewItem> GetChildItem(size_t index) const = 0;

		public:
			virtual std::shared_ptr<IDataViewCellRenderer> GetCellRenderer(const DataView::Node& node, const DataView::Column& column) const;
			virtual std::shared_ptr<IDataViewCellEditor> GetCellEditor(const DataView::Node& node, const DataView::Column& column) const;

			virtual Any GetCellDisplayValue(const DataView::Node& node, const DataView::Column& column) const;
			virtual Any GetCellValue(const DataView::Node& node, const DataView::Column& column) const;
			virtual bool SetCellValue(DataView::Node& node, DataView::Column& column, Any value);

			virtual DataView::ToolTip GetCellToolTip(const DataView::Node& node, const DataView::Column& column) const;
			virtual DataView::CellAttributes GetCellAttributes(const DataView::Node& node, const DataView::Column& column, const DataView::CellState& cellState) const;
	};
}
