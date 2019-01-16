#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithOptions.h"
#include "Common.h"
#include "Row.h"
#include "Node.h"
#include "Column.h"
class KX_API KxMenu;

namespace Kx::DataView2
{
	class KX_API HeaderCtrl;
	class KX_API MainWindow;
	class KX_API Model;
}

namespace Kx::DataView2
{
	class KX_API View: public wxSystemThemedControl<wxControl>,	public wxScrollHelper
	{
		friend class HeaderCtrl;
		friend class MainWindow;
		friend class Column;
		friend class Renderer;
		friend class Editor;

		public:
			enum: size_t
			{
				INVALID_ROW = (size_t)-1,
				INVALID_COLUMN = (size_t)-1,
				INVALID_COUNT = (size_t)-1,
			};

			template<class TRenderer, class TEditor = void>
			class AddedColumnInfo
			{
				private:
					Column* m_Column = nullptr;
					Renderer* m_Renderer = nullptr;
					Editor* m_Editor = nullptr;

				public:
					AddedColumnInfo(Column* column, Renderer* renderer, Editor* editor = nullptr)
						:m_Column(column), m_Renderer(renderer), m_Editor(editor)
					{
					}

				public:
					Column& GetColumn() const
					{
						return *m_Column;
					}
					TRenderer& GetRenderer() const
					{
						return static_cast<TRenderer&>(m_Renderer);
					}
					typename std::conditional<std::is_same_v<TEditor, void>, void, TEditor&>::type GetEditor() const
					{
						if constexpr(!std::is_same<TEditor, void>::value)
						{
							return *static_cast<TEditor*>(m_Editor);
						}
						else
						{
							static_assert(false, "This column has no editor");
						}
					}
			};

		private:
			KxWithOptions<CtrlStyle, CtrlStyle::DefaultStyle> m_Styles;

			HeaderCtrl* m_HeaderArea = nullptr;
			MainWindow* m_ClientArea = nullptr;
			Column* m_ExpanderColumn = nullptr;

			wxBoxSizer* m_Sizer = nullptr;
			wxSizerItem* m_HeaderAreaSI = nullptr;
			wxSizerItem* m_ClientAreaSI = nullptr;

			bool m_UsingSystemTheme = false;
			bool m_AllowMultiColumnSort = false;
			KxColor m_BorderColor;
			KxColor m_AlternateRowColor;
			std::vector<std::unique_ptr<Column>> m_Columns;

			// This indicates that at least one entry in 'm_Columns' has 'm_Dirty'
			// flag set. It's cheaper to check one flag in OnInternalIdle() than to
			// iterate over 'm_Columns' to check if anything needs to be done.
			bool m_IsColumnsDirty = false;

			// Columns indices used for sorting, empty if nothing is sorted
			KxIntPtrVector m_ColumnsSortingIndexes;

		private:
			void InvalidateColumnsBestWidth();
			void UpdateColumnsWidth();

			void OnSize(wxSizeEvent& event);
			void OnPaint(wxPaintEvent& event);
			wxSize GetSizeAvailableForScrollTarget(const wxSize& size) override;

		protected:
			void DoEnable(bool value) override;
			void DoInsertColumn(Column* column, size_t position);

			enum class ICEAction
			{
				Append,
				Prepend,
				Insert
			};

			template<ICEAction action, class TValue, class TRenderer = void, class TEditor = void>
			auto InsertColumnEx(const TValue& value, int id, ColumnWidth width = {}, ColumnStyle style = ColumnStyle::DefaultStyle, size_t index = 0)
			{
				Column* column = new Column(value, id, width, style);

				// Assign renderer and editor if needed
				TEditor* editor = nullptr;
				TRenderer* renderer = nullptr;
				if constexpr(!std::is_void_v<TEditor>)
				{
					editor = new TEditor();
					column->AssignEditor(editor);
				}
				if constexpr(!std::is_void_v<TRenderer>)
				{
					renderer = new TRenderer();
					column->AssignRenderer(renderer);
				}

				// Add column
				if constexpr(action == ICEAction::Append)
				{
					AppendColumn(column);
				}
				else if constexpr(action == ICEAction::Prepend)
				{
					PrependColumn(column);
				}
				else if constexpr (action == ICEAction::Insert)
				{
					InsertColumn(index, column);
				}
				else
				{
					static_assert(false, "Invalid ICE action");
				}

				// Return tuple
				if constexpr(!std::is_void_v<TRenderer> && !std::is_void_v<TEditor>)
				{
					return std::make_tuple(std::ref(*column), std::ref(*renderer), std::ref(*editor));
				}
				else if constexpr (std::is_void_v<TRenderer> && !std::is_void_v<TEditor>)
				{
					return std::make_tuple(std::ref(*column), std::ref(*editor));
				}
				else if constexpr(!std::is_void_v<TRenderer> && std::is_void_v<TEditor>)
				{
					return std::make_tuple(std::ref(*column), std::ref(*renderer));
				}
				else
				{
					return std::make_tuple(std::ref(*column));
				}
			}

			// TODO: rewrite sorting columns code
			void UseColumnForSorting(size_t index);
			void DontUseColumnForSorting(size_t index);
			bool IsColumnSorted(size_t index) const;
			void ResetAllSortColumns();

			// We need to return a special WM_GETDLGCODE value to process just the arrows but let the other navigation characters through
			WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;
			void OnInternalIdle() override;
			void DoEnableSystemTheme(bool enable, wxWindow* window) override;

		public:
			View()
				:wxScrollHelper(this)
			{
			}
			View(wxWindow* parent, wxWindowID id, CtrlStyle style = CtrlStyle::DefaultStyle)
				:View()
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent, wxWindowID id, CtrlStyle style = CtrlStyle::DefaultStyle);
			virtual ~View();

		public:
			// Styles
			bool IsOptionEnabled(CtrlStyle option) const
			{
				return m_Styles.IsOptionEnabled(option);
			}
			void SetOptionEnabled(CtrlStyle option, bool enable = true)
			{
				m_Styles.SetOptionEnabled(option, enable);
				wxControl::SetWindowStyleFlag(wxControl::GetWindowStyleFlag()|static_cast<long>(option));
			}
			void SetWindowStyleFlag(long style) override
			{
				m_Styles.SetOptionsValue(style);
				wxControl::SetWindowStyleFlag(style);
			}

			// Model
			Model* GetModel() const;
			void SetModel(Model* model);
			void AssignModel(Model* model);

			Node& GetRootNode() const;
			void ItemsChanged();

			// Columns
			Renderer& AppendColumn(Column* column);
			Renderer& PrependColumn(Column* column);
			Renderer& InsertColumn(size_t index, Column* column);

			template<class TRenderer = void, class TEditor = void>
			auto AppendColumn(const wxString& title, int id, ColumnWidth width = {}, ColumnStyle style = ColumnStyle::DefaultStyle)
			{
				return InsertColumnEx<ICEAction::Append, wxString, TRenderer, TEditor>(title, id, width, style);
			}

			template<class TRenderer = void, class TEditor = void>
			auto PrependColumn(const wxString& title, int id, ColumnWidth width = {}, ColumnStyle style = ColumnStyle::DefaultStyle)
			{
				return InsertColumnEx<ICEAction::Prepend, wxString, TRenderer, TEditor>(title, id, width, style);
			}

			template<class TRenderer = void, class TEditor = void>
			auto InsertColumn(size_t index, const wxString& title, int id, ColumnWidth width = {}, ColumnStyle style = ColumnStyle::DefaultStyle)
			{
				return InsertColumnEx<ICEAction::Insert, wxString, TRenderer, TEditor>(title, id, width, style, index);
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
			Column* GetColumnByID(int id) const;
			Column* GetColumnDisplayedAt(size_t displayIndex) const;

			bool DeleteColumn(Column& column);
			bool ClearColumns();

			Column* GetCurrentColumn() const;
			Column* GetExpanderColumnOrFirstOne();
			Column* GetExpanderColumn() const;
			void SetExpanderColumn(Column *column);

			bool IsMultiColumnSortUsed() const
			{
				return m_ColumnsSortingIndexes.size() > 1;
			}
			Column* GetSortingColumn() const;
			Column::Vector GetSortingColumns() const;

			bool IsMultiColumnSortAllowed() const
			{
				return m_AllowMultiColumnSort;
			}
			bool AllowMultiColumnSort(bool allow = true);
			void ToggleSortByColumn(size_t position);

			// Items
			int GetIndent() const;
			void SetIndent(int indent);

			// Current item is the one used by the keyboard navigation, it is the same as the (unique) selected item
			// in single selection mode so these functions are mostly useful for controls with 'CtrlStyle::MultipleSelection' style.
			Node* GetCurrentItem() const;
			void SetCurrentItem(Node& node);

			size_t GetSelectedCount() const;
			bool HasSelection() const
			{
				return GetSelectedCount() != 0;
			}
			Node* GetSelection() const;
			size_t GetSelections(Node::Vector& selection) const;
			void SetSelections(const Node::Vector& selection);
			void Select(Node& node);
			void Unselect(Node& node);
			bool IsSelected(const Node& node) const;

			Node* GetHotTrackedItem() const;
			Column* GetHotTrackedColumn() const;

			void GenerateSelectionEvent(Node& node, const Column* column = nullptr);

			void SelectAll();
			void UnselectAll();

			void Expand(Node& item);
			void ExpandAncestors(Node& item);
			void Collapse(Node& item);
			bool IsExpanded(Node& item) const;
			void SetItemExpanded(Node& item, bool expanded)
			{
				if (expanded)
				{
					Expand(item);
				}
				else
				{
					Collapse(item);
				}
			}
			void ToggleItemExpanded(Node& item)
			{
				SetItemExpanded(item, !IsExpanded(item));
			}

			void EnsureVisible(Node& item, const Column* column = nullptr);
			void HitTest(const wxPoint& point, Node*& item, Column*& column) const;
			wxRect GetItemRect(const Node& item, const Column* column = nullptr) const;
			wxRect GetAdjustedItemRect(const Node& item, const Column* column = nullptr) const;
			wxPoint GetDropdownMenuPosition(const Node& item, const Column* column = nullptr) const;

			int GetUniformRowHeight() const;
			void SetUniformRowHeight(int rowHeight);
			int GetDefaultRowHeight(UniformHeight type = UniformHeight::Default) const;

			bool EditItem(Node& item, Column& column);

			// Drag and drop
			bool EnableDragSource(const wxDataFormat& format);
			bool EnableDropTarget(const wxDataFormat& format);

			// Window
			void SetFocus() override;
			bool SetFont(const wxFont& font) override;

			bool HasHeaderCtrl() const
			{
				return m_HeaderArea != nullptr;
			}
			wxHeaderCtrl* GetHeaderCtrl();
			const wxHeaderCtrl* GetHeaderCtrl() const;

			MainWindow* GetMainWindow()
			{
				return m_ClientArea;
			}
			const MainWindow* GetMainWindow() const
			{
				return m_ClientArea;
			}

			bool CreateColumnSelectionMenu(KxMenu& menu);
			Column* OnColumnSelectionMenu(KxMenu& menu);

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

			KxColor GetAlternateRowColor() const
			{
				return m_AlternateRowColor;
			}
			void SetAlternateRowColor(const KxColor& color)
			{
				m_AlternateRowColor = color;
			}

			KxColor GetBorderColor() const
			{
				return m_BorderColor;
			}
			void SetBorderColor(const KxColor& color, int size = 1);

		private:
			// Called by header window after reorder
			void ColumnMoved(Column& column, size_t newPos);

			// Update the display after a change to an individual column
			void OnColumnChange(size_t index);

			// Update after a change to the number of columns
			void OnColumnsCountChanged();

		public:
			wxDECLARE_DYNAMIC_CLASS_NO_COPY(View);

		private:
			WX_FORWARD_TO_SCROLL_HELPER();
	};
}
