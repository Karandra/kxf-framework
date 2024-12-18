#pragma once
#include "Common.h"
#include "../IDataViewWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::DataView
{
	class Column;
}
namespace kxf::WXUI::DataView
{
	class View;
}

namespace kxf::Widgets
{
	class KXF_API DataView: public RTTI::Implementation<DataView, Private::BasicWxWidget<DataView, WXUI::DataView::View, IDataViewWidget>, IGraphicsRendererAwareWidget>
	{
		public:
			using Column = kxf::DataView::Column;

		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			DataView();
			~DataView();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IDataViewWidget
			std::shared_ptr<IDataViewModel> GetDataModel() const override;
			void SetDataModel(std::shared_ptr<IDataViewModel> dataModel) override;

			FlagSet<WidgetStyle> GetStyle() const override;
			void SetStyle(FlagSet<WidgetStyle> style) override;

			Color GetAlternateRowColor() const override;
			void SetAlternateRowColor(const Color& color) override;

			int GetUniformRowHeight() const override;
			void SetUniformRowHeight(int rowHeight) override;

			size_t GetColumnCount() const override;
			size_t GetVisibleColumnCount() const override;

			bool IsMultiColumnSortUsed() const override;
			DataView::Column* GetSortingColumn() const override;
			Enumerator<DataView::Column&> GetSortingColumns() const override;

			DataView::Column* GetCurrentColumn() const override;
			DataView::Column* GetHotTrackColumn() const override;
			DataView::Column* GetExpanderColumn() const override;
			void SetExpanderColumn(DataView::Column& column) override;

			Column* InsertColumn(size_t index, const String& title, WidgetID id = {}, FlagSet<ColumnStyle> style = ColumnStyle::Moveable|ColumnStyle::Resizeable) override;
			void RemoveColumn(DataView::Column& column) override;
			void ClearColumns() override;

			DataView::Column* GetColumnAt(size_t index) const override;
			DataView::Column* GetColumnDisplayedAt(size_t index) const override;
			DataView::Column* GetColumnPhysicallyDisplayedAt(size_t index) const override;
			Enumerator<DataView::Column&> EnumColumns(ColumnOrder order = ColumnOrder::Default) const override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
