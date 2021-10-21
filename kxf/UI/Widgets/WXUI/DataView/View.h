#pragma once
#include "Common.h"
#include "Row.h"
#include "../../../DataView/Node.h"
#include "../../../DataView/Column.h"
#include "../../../DataView/SortMode.h"

#include "../../../IDataViewWidget.h"
#include "../../../IGraphicsRendererAwareWidget.h"
#include "kxf/UI/WindowWithStyles.h"
#include "kxf/UI/WindowRefreshScheduler.h"
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
	class KX_API View: public EvtHandlerWrapper<View, UI::WindowRefreshScheduler<wxSystemThemedControl<wxScrolled<wxWindow>>>>
	{
		friend class Widgets::DataView;
		friend class kxf::DataView::Column;
		friend class HeaderCtrl;
		friend class HeaderCtrl2;
		friend class MainWindow;
		friend class Renderer;
		friend class Editor;

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

			HeaderCtrl* m_HeaderArea = nullptr;
			MainWindow* m_ClientArea = nullptr;
			DV::Column* m_ExpanderColumn;

			wxBoxSizer* m_Sizer = nullptr;
			wxSizerItem* m_HeaderAreaSI = nullptr;
			wxSizerItem* m_HeaderAreaSpacerSI = nullptr;
			wxSizerItem* m_ClientAreaSI = nullptr;

			FlagSet<WidgetStyle> m_Style = WidgetStyle::VerticalRules|WidgetStyle::FullRowSelection|WidgetStyle::FitLastColumn;
			bool m_UsingSystemTheme = false;
			Color m_BorderColor;
			Color m_AlternateRowColor;
			std::vector<std::unique_ptr<DV::Column>> m_Columns;

			// This indicates that at least one entry in 'm_Columns' has 'm_Dirty'
			// flag set. It's cheaper to check one flag in 'OnInternalIdle' than to
			// iterate over 'm_Columns' to check if anything needs to be done.
			bool m_ColumnsDirty = false;

		private:
			void InvalidateColumnsBestWidth();
			void UpdateColumnsWidth();
			bool HasColumn(const DV::Column& column) const;

			void OnSize(wxSizeEvent& event);
			void OnPaint(wxPaintEvent& event);
			wxSize GetSizeAvailableForScrollTarget(const wxSize& size) override;

			std::vector<DV::Column*> DoGetColumnsInDisplayOrder(bool physicalOrder) const;

			// Called by header window after reorder
			void MoveColumn(IDataViewColumn& column, size_t newIndex);
			void MoveColumnToPhysicalIndex(IDataViewColumn& movedColumn, size_t newIndex);

			// Update the display after a change to an individual column
			void OnColumnChange(DV::Column& column);

			// Update after a change to the number of columns
			void OnColumnCountChanged();

		protected:
			void DoEnable(bool value) override;
			void DoInsertColumn(std::unique_ptr<DV::Column> column, size_t position);
			void ResetAllSortColumns();

			#if 0
			enum class ICEAction
			{
				Append,
				Prepend,
				Insert
			};
			template<ICEAction action, class TValue, class TRenderer = void, class TEditor = void>
			auto InsertColumnEx(const TValue& value, ColumnID id = {}, ColumnWidth width = {}, ColumnStyle style = ColumnStyle::Default, size_t index = 0)
			{
				auto column = std::make_unique<Column>(value, id, width, style);
				Column* columnPtr = column.get();

				// Assign renderer and editor if needed
				TEditor* editorPtr = nullptr;
				TRenderer* rendererPtr = nullptr;
				if constexpr(!std::is_void_v<TEditor>)
				{
					auto editor = std::make_unique<TEditor>();
					editorPtr = editor.get();
					column->AssignEditor(std::move(editor));
				}
				if constexpr(!std::is_void_v<TRenderer>)
				{
					auto renderer = std::make_unique<TRenderer>();
					rendererPtr = renderer.get();
					column->AssignRenderer(std::move(renderer));
				}

				// Add column
				if constexpr(action == ICEAction::Append)
				{
					AppendColumn(std::move(column));
				}
				else if constexpr(action == ICEAction::Prepend)
				{
					PrependColumn(std::move(column));
				}
				else if constexpr(action == ICEAction::Insert)
				{
					InsertColumn(index, std::move(column));
				}
				else
				{
					static_assert(false, "Invalid ICE action");
				}

				// Return tuple
				if constexpr(!std::is_void_v<TRenderer> && !std::is_void_v<TEditor>)
				{
					return std::make_tuple(std::ref(*columnPtr), std::ref(*rendererPtr), std::ref(*editorPtr));
				}
				else if constexpr(std::is_void_v<TRenderer> && !std::is_void_v<TEditor>)
				{
					return std::make_tuple(std::ref(*columnPtr), std::ref(*editorPtr));
				}
				else if constexpr(!std::is_void_v<TRenderer> && std::is_void_v<TEditor>)
				{
					return std::make_tuple(std::ref(*columnPtr), std::ref(*rendererPtr));
				}
				else
				{
					return std::make_tuple(std::ref(*columnPtr));
				}
			}
			#endif

			// We need to return a special WM_GETDLGCODE value to process just the arrows but let the other navigation characters through
			bool MSWHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;
			void OnInternalIdle() override;
			void DoEnableSystemTheme(bool enable, wxWindow* window) override;

		public:
			View(IDataViewWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
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
				return m_ClientArea;
			}
			const MainWindow* GetMainWindow() const
			{
				return m_ClientArea;
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
			DV::Node GetCurrentItem() const;
			DV::Node GetHotTrackedItem() const;
			DV::Column* GetHotTrackedColumn() const;

			size_t GetSelectedCount() const;
			bool HasSelection() const
			{
				return GetSelectedCount() != 0;
			}

			DV::Node GetSelection() const;
			size_t GetSelections(std::function<bool(DV::Node)> func) const;
			void SetSelections(const std::vector<DV::Node>& selection);

			void GenerateSelectionEvent(DV::Node& node, const DV::Column* column = nullptr);

			void SelectAll();
			void UnselectAll();

			int GetUniformRowHeight() const;
			void SetUniformRowHeight(int rowHeight);

			void HitTest(const Point& point, DV::Node& item, DV::Column*& column) const;

			// Drag and drop
			bool EnableDND(std::unique_ptr<wxDataObjectSimple> dataObject, DNDOpType type, bool isPreferredDrop = false);
			bool DisableDND(const wxDataObjectSimple& dataObject);
			bool DisableDND(const wxDataFormat& format);

			// Window
			void SetFocus() override;
			bool SetFont(const wxFont& font) override;

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
			void SetAlternateRowColor(const Color& color)
			{
				m_AlternateRowColor = color;
			}

			Color GetBorderColor() const
			{
				return m_BorderColor;
			}
			void SetBorderColor(const Color& color, int size = 1);
	};
}
