#pragma once
#include "Common.h"
#include "../../DataView/Row.h"
#include "../../DataView/Node.h"
#include "../../DataView/Column.h"
#include "../../DataView/SortMode.h"

#include "../../../IDataViewWidget.h"
#include "../../../IGraphicsRendererAwareWidget.h"
#include "kxf/Widgets/WindowWithStyles.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include "kxf/Widgets/Windows/ToolTipEx.h"
#include "kxf/Drawing/IRendererNative.h"
#include <wx/systhemectrl.h>
#include <wx/scrolwin.h>
#include <wx/dataobj.h>
#include <wx/sizer.h>

namespace kxf::Widgets
{
	class DataView;
}

namespace kxf::DataView
{
	class Column;
	class ToolTip;
	class CellEditor;
	class CellRenderer;
}
namespace kxf::WXUI::DataView
{
	class HeaderCtrl;
	class MainWindow;
	class Model;

	namespace DV = kxf::DataView;
}

namespace kxf::WXUI::DataView
{
	class KXF_API View: public EvtHandlerWrapper<View, UI::WindowRefreshScheduler<wxSystemThemedControl<wxScrolled<wxWindow>>>>
	{
		friend class Widgets::DataView;
		friend class DV::Column;
		friend class DV::ToolTip;
		friend class DV::CellEditor;
		friend class DV::CellRenderer;

		friend class HeaderCtrl;
		friend class HeaderCtrl2;
		friend class MainWindow;

		public:
			using WidgetStyle = kxf::DataView::WidgetStyle;
			enum: size_t
			{
				INVALID_ROW = std::numeric_limits<size_t>::max(),
				INVALID_COLUMN = std::numeric_limits<size_t>::max(),
				INVALID_COUNT = std::numeric_limits<size_t>::max(),
			};

			static constexpr auto npos = IDataViewWidget::npos;

		protected:
			using ViewBase = WindowRefreshScheduler<wxSystemThemedControl<wxScrolled<wxWindow>>>;

		private:
			IDataViewWidget& m_Widget;
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;

			std::unique_ptr<MainWindow> m_ClientArea;
			HeaderCtrl* m_HeaderArea = nullptr;
			DV::Column* m_ExpanderColumn;

			FlagSet<WidgetStyle> m_Style = WidgetStyle::VerticalRules|WidgetStyle::FullRowSelection|WidgetStyle::FitLastColumn;
			Color m_AlternateRowColor;
			bool m_UsingSystemTheme = false;
			std::vector<std::unique_ptr<DV::Column>> m_Columns;
			UI::ToolTipEx m_ToolTip;

			int m_HeaderHeight = 0;
			int m_HeaderBorder = 0;

			// This indicates that at least one entry in 'm_Columns' has 'm_Dirty'
			// flag set. It's cheaper to check one flag in 'OnInternalIdle' than to
			// iterate over 'm_Columns' to check if anything needs to be done.
			bool m_ColumnsDirty = false;

		private:
			void InvalidateColumnsBestWidth();
			void UpdateColumnsWidth();
			bool HasColumn(const DV::Column& column) const;

			void OnSize(wxSizeEvent& event);
			wxSize GetSizeAvailableForScrollTarget(const wxSize& size) override;

			std::vector<DV::Column*> DoGetColumnsInDisplayOrder(bool physicalOrder) const;

			// Called by header window after reorder
			void MoveColumn(DV::Column& column, size_t newIndex);
			void MoveColumnToPhysicalIndex(DV::Column& movedColumn, size_t newIndex);

			// Update the display after a change to an individual column
			void OnColumnChange(DV::Column& column);

			// Update after a change to the number of columns
			void OnColumnCountChanged();

			void DoEnable(bool value) override;
			void DoInsertColumn(std::unique_ptr<DV::Column> column, size_t position);
			void ResetAllSortColumns();

			// We need to return a special WM_GETDLGCODE value to process just the arrows but let the other navigation characters through
			bool MSWHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;
			void OnInternalIdle() override;
			void DoEnableSystemTheme(bool enable, wxWindow* window) override;

		public:
			View(IDataViewWidget& widget);
			~View();

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

			HeaderCtrl* GetHeaderCtrl()
			{
				return m_HeaderArea;
			}
			const HeaderCtrl* GetHeaderCtrl() const
			{
				return m_HeaderArea;
			}

			MainWindow* GetMainWindow()
			{
				return m_ClientArea.get();
			}
			const MainWindow* GetMainWindow() const
			{
				return m_ClientArea.get();
			}

			// Styles
			FlagSet<WidgetStyle> GetStyle() const
			{
				return m_Style;
			}
			void SetStyle(FlagSet<WidgetStyle> style);

			// Columns
			size_t InsertColumn(std::unique_ptr<DV::Column> column, size_t index);
			bool DeleteColumn(DV::Column& column);
			bool ClearColumns();

			size_t GetColumnCount() const
			{
				return m_Columns.size();
			}
			size_t GetVisibleColumnCount() const
			{
				return std::ranges::count_if(m_Columns, [](const auto& column)
				{
					return column->IsVisible();
				});
			}
			size_t GetInvisibleColumnCount() const
			{
				return GetColumnCount() - GetVisibleColumnCount();
			}

			DV::Column* GetColumnAt(size_t index) const;
			DV::Column* GetColumnByID(WidgetID id) const;
			DV::Column* GetColumnDisplayedAt(size_t displayIndex) const;
			DV::Column* GetColumnPhysicallyDisplayedAt(size_t displayIndex) const;
			auto GetColumnsInDisplayOrder() const
			{
				return DoGetColumnsInDisplayOrder(false);
			}
			auto GetColumnsInPhysicalDisplayOrder() const
			{
				return DoGetColumnsInDisplayOrder(true);
			}

			DV::Column* GetCurrentColumn() const;
			DV::Column* GetExpanderColumnOrFirstOne();
			DV::Column* GetExpanderColumn() const;
			void SetExpanderColumn(DV::Column& column);

			bool IsMultiColumnSortUsed() const;
			DV::SortMode GetSortMode() const
			{
				return m_Widget;
			}
			DV::Column* GetSortingColumn() const;
			std::vector<DV::Column*> GetSortingColumns() const;

			// Items
			int GetIndent() const;
			void SetIndent(int indent);

			// Current item is the one used by the keyboard navigation, it is the same as the (unique) selected item
			// in single selection mode so these functions are mostly useful for controls with 'CtrlStyle::MultipleSelection' style.
			DV::Node* GetCurrentItem() const;
			DV::Node* GetHotTrackedItem() const;
			DV::Column* GetHotTrackedColumn() const;

			size_t GetSelectedCount() const;
			bool HasSelection() const
			{
				return GetSelectedCount() != 0;
			}

			DV::Node* GetSelection() const;
			size_t GetSelections(std::function<bool(DV::Node*)> func) const;
			void SetSelections(const std::vector<DV::Node*>& selection);

			void SelectAll();
			void UnselectAll();

			int GetUniformRowHeight() const;
			void SetUniformRowHeight(int rowHeight);

			void HitTest(const Point& point, DV::Node*& item, DV::Column*& column) const;

			// Drag and drop
			bool EnableDND(std::unique_ptr<wxDataObjectSimple> dataObject, DNDOpType type, bool isPreferredDrop = false);
			bool DisableDND(const wxDataObjectSimple& dataObject);
			bool DisableDND(const wxDataFormat& format);

			// Window
			bool SetFont(const wxFont& font) override;

			void DoSetVirtualSize(int x, int y) override;
			void ScrollWindow(int dx, int dy, const wxRect* rect = nullptr) override;

			//bool CreateColumnSelectionMenu(Menu& menu);
			//DV::Column* OnColumnSelectionMenu(Menu& menu);

			// Visuals
			wxBorder GetDefaultBorder() const override
			{
				return wxBORDER_THEME;
			}

			bool SetForegroundColour(const wxColour& color) override;
			bool SetBackgroundColour(const wxColour& color) override;

			Color GetAlternateRowColor() const
			{
				return m_AlternateRowColor;
			}
			void SetAlternateRowColor(const Color& color);
	};
}
