#pragma once
#include "Common.h"
#include "Row.h"
#include "Node.h"
#include "Column.h"
#include "ColumnID.h"
#include "SortMode.h"
#include "kxf/UI/WindowWithStyles.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include "kxf/UI/IGraphicsRendererAwareWidget.h"
#include <wx/systhemectrl.h>
#include <wx/scrolwin.h>
#include <wx/dataobj.h>
#include <wx/sizer.h>

namespace kxf::UI
{
	class Menu;
}
namespace kxf::UI::DataView
{
	class HeaderCtrl;
	class MainWindow;
	class Model;
}

namespace kxf::UI::DataView
{
	class KXF_API View:
		public WindowRefreshScheduler<wxSystemThemedControl<wxScrolled<wxWindow>>>,
		public WindowWithStyles<View, CtrlStyle>,
		public WindowWithExtraStyles<View, CtrlExtraStyle>,
		public IGraphicsRendererAwareWidget
	{
		friend class HeaderCtrl;
		friend class HeaderCtrl2;
		friend class MainWindow;
		friend class Column;
		friend class Renderer;
		friend class Editor;

		public:
			enum: size_t
			{
				INVALID_ROW = std::numeric_limits<size_t>::max(),
				INVALID_COLUMN = std::numeric_limits<size_t>::max(),
				INVALID_COUNT = std::numeric_limits<size_t>::max(),
			};

		protected:
			using ViewBase = WindowRefreshScheduler<wxSystemThemedControl<wxScrolled<wxWindow>>>;

		private:
			FlagSet<CtrlStyle> m_Styles = CtrlStyle::Default;
			FlagSet<CtrlExtraStyle> m_ExtraStyles;

			HeaderCtrl* m_HeaderArea = nullptr;
			MainWindow* m_ClientArea = nullptr;
			Column* m_ExpanderColumn = nullptr;

			wxBoxSizer* m_Sizer = nullptr;
			wxSizerItem* m_HeaderAreaSI = nullptr;
			wxSizerItem* m_HeaderAreaSpacerSI = nullptr;
			wxSizerItem* m_ClientAreaSI = nullptr;

			bool m_UsingSystemTheme = false;
			bool m_AllowMultiColumnSort = false;
			Color m_BorderColor;
			Color m_AlternateRowColor;
			Column::Vector m_Columns;

			// This indicates that at least one entry in 'm_Columns' has 'm_Dirty'
			// flag set. It's cheaper to check one flag in 'OnInternalIdle' than to
			// iterate over 'm_Columns' to check if anything needs to be done.
			bool m_ColumnsDirty = false;

			std::shared_ptr<IGraphicsRenderer> m_PendingGraphicsRenderer;

		private:
			void InvalidateColumnsBestWidth();
			void UpdateColumnsWidth();

			void OnSize(wxSizeEvent& event);
			void OnPaint(wxPaintEvent& event);
			wxSize GetSizeAvailableForScrollTarget(const wxSize& size) override;

			Column::RefVector DoGetColumnsInDisplayOrder(bool physicalOrder) const;

			// Called by header window after reorder
			void MoveColumn(Column& column, size_t newIndex);
			void MoveColumnToPhysicalIndex(Column& movedColumn, size_t newIndex);

			// Update the display after a change to an individual column
			void OnColumnChange(Column& column);

			// Update after a change to the number of columns
			void OnColumnCountChanged();

		protected:
			void DoEnable(bool value) override;
			void DoInsertColumn(std::unique_ptr<Column> column, size_t position);
			void ResetAllSortColumns();

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
					static_assert(sizeof(TValue*) == 0, "Invalid ICE action");
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

			// We need to return a special WM_GETDLGCODE value to process just the arrows but let the other navigation characters through
			bool MSWHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;
			void OnInternalIdle() override;
			void DoEnableSystemTheme(bool enable, wxWindow* window) override;

		public:
			View() = default;
			View(wxWindow* parent, wxWindowID id, FlagSet<CtrlStyle> style = CtrlStyle::Default)
			{
				Create(parent, id, style);
			}
			~View();

			bool Create(wxWindow* parent,
						wxWindowID id,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						FlagSet<CtrlStyle> style = CtrlStyle::Default,
						const String& name = {}
			);
			bool Create(wxWindow* parent, wxWindowID id, FlagSet<CtrlStyle> style = CtrlStyle::Default)
			{
				return Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style);
			}

		public:
			// Styles
			long GetWindowStyleFlag() const override;
			void SetWindowStyleFlag(long styles) override;
			void SetExtraStyle(long styles) override;

			// Model
			Model* GetModel();
			void SetModel(Model& model);
			void AssignModel(std::unique_ptr<Model> model);

			RootNode& GetRootNode();
			const RootNode& GetRootNode() const;
			void ItemsChanged();

			// Columns
			Renderer& AppendColumn(std::unique_ptr<Column> column);
			Renderer& PrependColumn(std::unique_ptr<Column> column);
			Renderer& InsertColumn(size_t index, std::unique_ptr<Column> column);

			template<class TRenderer = void, class TEditor = void>
			auto AppendColumn(const String& title, ColumnID id, ColumnWidth width = {}, ColumnStyle style = ColumnStyle::Default)
			{
				return InsertColumnEx<ICEAction::Append, String, TRenderer, TEditor>(title, id, width, style);
			}

			template<class TRenderer = void, class TEditor = void>
			auto PrependColumn(const String& title, ColumnID id, ColumnWidth width = {}, ColumnStyle style = ColumnStyle::Default)
			{
				return InsertColumnEx<ICEAction::Prepend, String, TRenderer, TEditor>(title, id, width, style);
			}

			template<class TRenderer = void, class TEditor = void>
			auto InsertColumn(size_t index, const String& title, ColumnID id, ColumnWidth width = {}, ColumnStyle style = ColumnStyle::Default)
			{
				return InsertColumnEx<ICEAction::Insert, String, TRenderer, TEditor>(title, id, width, style, index);
			}

			size_t GetColumnCount() const
			{
				return m_Columns.size();
			}
			size_t GetVisibleColumnCount() const;
			size_t GetHiddenColumnCount() const
			{
				return GetColumnCount() - GetVisibleColumnCount();
			}

			Column* GetColumn(size_t position) const;
			Column* GetColumnByID(ColumnID id) const;
			Column* GetColumnDisplayedAt(size_t displayIndex) const;
			Column* GetColumnPhysicallyDisplayedAt(size_t displayIndex) const;
			Column::RefVector GetColumnsInDisplayOrder() const
			{
				return DoGetColumnsInDisplayOrder(false);
			}
			Column::RefVector GetColumnsInPhysicalDisplayOrder() const
			{
				return DoGetColumnsInDisplayOrder(true);
			}

			bool DeleteColumn(Column& column);
			bool ClearColumns();

			Column* GetCurrentColumn() const;
			Column* GetExpanderColumnOrFirstOne();
			Column* GetExpanderColumn() const;
			void SetExpanderColumn(Column *column);

			bool IsMultiColumnSortUsed() const;
			SortMode GetSortMode() const
			{
				return *this;
			}
			Column* GetSortingColumn() const;
			Column::RefVector GetSortingColumns() const;

			bool IsMultiColumnSortAllowed() const
			{
				return m_AllowMultiColumnSort;
			}
			bool AllowMultiColumnSort(bool allow = true);
			void ToggleSortByColumn(size_t index);

			// Items
			int GetIndent() const;
			void SetIndent(int indent);

			// Current item is the one used by the keyboard navigation, it is the same as the (unique) selected item
			// in single selection mode so these functions are mostly useful for controls with 'CtrlStyle::MultipleSelection' style.
			Node* GetCurrentItem() const;
			Node* GetHotTrackedItem() const;
			Column* GetHotTrackedColumn() const;

			size_t GetSelectedCount() const;
			bool HasSelection() const
			{
				return GetSelectedCount() != 0;
			}

			Node* GetSelection() const;
			size_t GetSelections(std::function<bool(Node&)> func) const;
			void SetSelections(const std::vector<Node*>& selection);

			void GenerateSelectionEvent(Node& node, const Column* column = nullptr);

			void SelectAll();
			void UnselectAll();

			int GetUniformRowHeight() const;
			void SetUniformRowHeight(int rowHeight);
			int GetDefaultRowHeight(UniformHeight type = UniformHeight::Default) const;

			void HitTest(const Point& point, Node*& item, Column*& column) const;

			// Drag and drop
			bool EnableDND(std::unique_ptr<wxDataObjectSimple> dataObject, DNDOpType type, bool isPreferredDrop = false);
			bool DisableDND(const wxDataObjectSimple& dataObject);
			bool DisableDND(const wxDataFormat& format);

			// Window
			void SetFocus() override;
			bool SetFont(const wxFont& font) override;
			void InitDialog() override;

			bool HasHeaderCtrl() const
			{
				return m_HeaderArea != nullptr;
			}
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

			bool CreateColumnSelectionMenu(Menu& menu);
			Column* OnColumnSelectionMenu(Menu& menu);

			String GetEmptyControlLabel() const;
			void SetEmptyControlLabel(const String& value);

			// Control visuals
			static wxVisualAttributes GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL)
			{
				return wxControl::GetCompositeControlsDefaultAttributes(variant);
			}
			wxVisualAttributes GetDefaultAttributes() const override
			{
				return GetClassDefaultAttributes(GetWindowVariant());
			}
			wxBorder GetDefaultBorder() const override;

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

			BitmapImage GetBackgroundBitmap() const;
			void SetBackgroundBitmap(const BitmapImage& bitmap, FlagSet<Alignment> align = Alignment::Invalid, bool fit = false);

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override;

		public:
			wxDECLARE_DYNAMIC_CLASS_NO_COPY(View);
	};
}
