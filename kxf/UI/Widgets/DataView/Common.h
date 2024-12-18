#pragma once
#include "../Common.h"

namespace kxf
{
	class IDataViewWidget;
	class IDataViewModel;
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
	class CellState;
	class CellAttributes;

	enum class WidgetStyle: uint32_t
	{
		None = 0,

		MultipleSelection = FlagSetValue<WidgetStyle>(0),
		FullRowSelection = FlagSetValue<WidgetStyle>(1),
		VerticalRules = FlagSetValue<WidgetStyle>(2),
		HorizontalRules = FlagSetValue<WidgetStyle>(3),
		AlternatingRowColors = FlagSetValue<WidgetStyle>(4),
		CellFocus = FlagSetValue<WidgetStyle>(5),
		VariableRowHeight = FlagSetValue<WidgetStyle>(6),
		MultiColumnSort = FlagSetValue<WidgetStyle>(7),
		NoHeader = FlagSetValue<WidgetStyle>(8),
		Flat = FlagSetValue<WidgetStyle>(9),
		FitLastColumn = FlagSetValue<WidgetStyle>(10),
		FitLastColumnToClient = FlagSetValue<WidgetStyle>(11),
	};
	enum class ColumnStyle: uint32_t
	{
		None = 0,

		Sortable = FlagSetValue<ColumnStyle>(0),
		Moveable = FlagSetValue<ColumnStyle>(1),
		Resizeable = FlagSetValue<ColumnStyle>(2),
		CheckBox = FlagSetValue<ColumnStyle>(3),
		Dropdown = FlagSetValue<ColumnStyle>(4)
	};
}

namespace kxf
{
	kxf_FlagSet_Declare(DataView::WidgetStyle);
	kxf_FlagSet_Declare(DataView::ColumnStyle);
}
