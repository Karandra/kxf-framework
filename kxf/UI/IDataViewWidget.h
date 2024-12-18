#pragma once
#include "Common.h"
#include "IWidget.h"
#include "IDataViewModel.h"
#include "IDataViewItem.h"
#include "Widgets/DataView/Common.h"
#include "Widgets/DataView/Column.h"
#include "Widgets/DataView/Node.h"
#include "Widgets/DataView/CellRenderer.h"
#include "Widgets/DataView/Renderers/TextRenderer.h"
#include "Widgets/DataView/CellEditor.h"
#include "Widgets/DataView/Editors/TextBox.h"

namespace kxf
{
	class KXF_API IDataViewWidget: public RTTI::ExtendInterface<IDataViewWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(IDataViewWidget, {0x5ed1aeb0, 0x5b17, 0x4207, {0xb6, 0xa8, 0xd, 0x82, 0xba, 0xcb, 0xad, 0x73}});

		public:
			using WidgetStyle = DataView::WidgetStyle;
			using ColumnStyle = DataView::ColumnStyle;
			enum class ColumnOrder
			{
				Default,
				Display,
				Physical
			};

			static constexpr auto npos = std::numeric_limits<size_t>::max();

		public:
			virtual std::shared_ptr<IDataViewModel> GetDataModel() const = 0;
			virtual void SetDataModel(std::shared_ptr<IDataViewModel> dataModel) = 0;

			virtual FlagSet<WidgetStyle> GetStyle() const = 0;
			virtual void SetStyle(FlagSet<WidgetStyle> style) = 0;

			virtual Color GetAlternateRowColor() const = 0;
			virtual void SetAlternateRowColor(const Color& color) = 0;

			virtual int GetUniformRowHeight() const = 0;
			virtual void SetUniformRowHeight(int rowHeight) = 0;

			virtual size_t GetColumnCount() const = 0;
			virtual size_t GetVisibleColumnCount() const = 0;
			size_t GetHiddenColumnCount() const
			{
				return GetColumnCount() - GetVisibleColumnCount();
			}

			virtual bool IsMultiColumnSortUsed() const = 0;
			virtual DataView::Column* GetSortingColumn() const = 0;
			virtual Enumerator<DataView::Column&> GetSortingColumns() const = 0;

			virtual DataView::Column* GetCurrentColumn() const = 0;
			virtual DataView::Column* GetHotTrackColumn() const = 0;
			virtual DataView::Column* GetExpanderColumn() const = 0;
			virtual void SetExpanderColumn(DataView::Column& column) = 0;

			DataView::Column* AddColumn(const String& title, WidgetID id = {}, FlagSet<ColumnStyle> style = ColumnStyle::Moveable|ColumnStyle::Resizeable)
			{
				return InsertColumn(npos, title, id, style);
			}
			virtual DataView::Column* InsertColumn(size_t index, const String& title, WidgetID id = {}, FlagSet<ColumnStyle> style = ColumnStyle::Moveable|ColumnStyle::Resizeable) = 0;
			virtual void RemoveColumn(DataView::Column& column) = 0;
			void RemoveColumnAt(size_t index)
			{
				if (auto column = GetColumnAt(index))
				{
					RemoveColumn(*column);
				}
			}
			virtual void ClearColumns() = 0;

			virtual DataView::Column* GetColumnAt(size_t index) const = 0;
			virtual DataView::Column* GetColumnDisplayedAt(size_t index) const = 0;
			virtual DataView::Column* GetColumnPhysicallyDisplayedAt(size_t index) const = 0;
			virtual Enumerator<DataView::Column&> EnumColumns(ColumnOrder order = ColumnOrder::Default) const = 0;
	};
}
