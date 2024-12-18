#pragma once
#include "Common.h"
#include "kxf/Core/RecursionGuard.h"

namespace kxf::WXUI::DataView
{
	class View;
	class MainWindow;
	class HeaderCtrl;
	class HeaderCtrl2;
}

namespace kxf::DataView
{
	class KXF_API Column final
	{
		friend class WXUI::DataView::View;
		friend class WXUI::DataView::MainWindow;
		friend class WXUI::DataView::HeaderCtrl;
		friend class WXUI::DataView::HeaderCtrl2;
		friend class ToolTip;
		friend class CellEditor;
		friend class CellRenderer;

		public:
			static constexpr auto npos = std::numeric_limits<size_t>::max();

		private:
			static int GetAbsMinColumnWidth() noexcept;
			static int GetAbsMaxColumnWidth() noexcept;

		private:
			WXUI::DataView::View* m_View = nullptr;
			std::shared_ptr<IDataViewCellRenderer> m_CellRenderer;
			std::shared_ptr<IDataViewCellEditor> m_Editor;

			WidgetID m_ID;
			size_t m_Index = npos;
			size_t m_DisplayIndex = npos;

			String m_Title;
			BitmapImage m_Icon;
			FlagSet<Alignment> m_TitleAlignment = Alignment::Invalid;
			bool m_IsChecked = false;

			FlagSet<ColumnStyle> m_Style;
			int m_Width = -1;
			int m_MinWidth = GetAbsMinColumnWidth();
			mutable int m_BestWidth = -1;
			mutable RecursionGuardFlag m_BestWidthRG;

			bool m_IsDirty = true;
			bool m_IsVisible = true;
			SortOrder m_SortOrder = SortOrder::None;

		private:
			bool IsDirty() const
			{
				return m_IsDirty;
			}
			void MarkDirty(bool value = true);
			void UpdateDisplay();
			void AssignPhysicalDisplayIndex(size_t index)
			{
				m_DisplayIndex = GetInvisibleColumnsBefore() + index;
			}

			WXUI::DataView::View* GetView() const
			{
				return m_View;
			}
			WXUI::DataView::MainWindow* GetMainWindow() const;

			int CalcBestWidth() const;
			int CalcTitleWidth() const;

			void OnColumnAttached(WXUI::DataView::View& view, size_t index, size_t displayIndex = npos);
			void OnColumnDetached();

		public:
			Column() = default;
			Column(String title, WidgetID id, FlagSet<ColumnStyle> style = {}, std::shared_ptr<IDataViewCellRenderer> cellRenderer = nullptr)
				:m_Title(std::move(title)), m_CellRenderer(std::move(cellRenderer)), m_ID(id), m_Style(style)
			{
			}

		public:
			IDataViewWidget& GetOwningWdget() const;

			std::shared_ptr<IDataViewCellRenderer> GetCellRenderer() const
			{
				return m_CellRenderer;
			}
			void SetCellRenderer(std::shared_ptr<IDataViewCellRenderer> renderer)
			{
				m_CellRenderer = std::move(renderer);
				UpdateDisplay();
			}

			std::shared_ptr<IDataViewCellEditor> GetCellEditor() const
			{
				return m_Editor;
			}
			void SetCellEditor(std::shared_ptr<IDataViewCellEditor> editor)
			{
				m_Editor = std::move(editor);
			}

			FlagSet<ColumnStyle> GetStyle() const
			{
				return m_Style;
			}
			void SetStyle(FlagSet<ColumnStyle> style)
			{
				m_Style = style;
				UpdateDisplay();
			}

			WidgetID GetID() const
			{
				return m_ID;
			}
			void SetID(WidgetID id)
			{
				m_ID = id;
			}

			size_t GetIndex() const
			{
				return m_Index;
			}
			size_t GetDisplayIndex() const
			{
				return m_DisplayIndex;
			}
			size_t GetPhysicalDisplayIndex() const;
			void SetDisplayIndex(size_t index);
			void SetPhysicalDisplayIndex(size_t index);
			size_t GetInvisibleColumnsBefore() const;

			int GetWidth() const
			{
				if (m_Width < 0)
				{
					return GetBestWidth();
				}
				else
				{
					return m_Width;
				}
			}
			void SetWidth(int width)
			{
				if (width != m_Width)
				{
					if (width < 0)
					{
						m_Width = -1;
					}
					else
					{
						m_Width = std::clamp<int>(width, m_MinWidth, GetAbsMaxColumnWidth());
					}
					UpdateDisplay();
				}
			}

			int GetMinWidth() const;
			void SetMinWidth(int width)
			{
				m_MinWidth = std::clamp(width, GetAbsMinColumnWidth(), GetAbsMaxColumnWidth());
			}
			
			int GetBestWidth() const
			{
				if (m_BestWidth < 0)
				{
					// Recursion guard for calling from user code
					if (RecursionGuard guard(m_BestWidthRG); !guard.IsInside())
					{
						m_BestWidth = std::clamp(CalcBestWidth(), GetAbsMinColumnWidth(), GetAbsMaxColumnWidth());
					}
				}
				return m_BestWidth;
			}
			void SetBestWidth(int width)
			{
				if (width != m_BestWidth)
				{
					if (width < 0)
					{
						m_BestWidth = -1;
						MarkDirty();
					}
					else
					{
						m_BestWidth = std::clamp(width, GetAbsMinColumnWidth(), GetAbsMaxColumnWidth());
					}
				}
			}

			Rect GetRect() const;
			Rect GetDropdownRect() const;

			bool IsVisible() const
			{
				return m_IsVisible;
			}
			void SetVisible(bool isVisible = true)
			{
				if (isVisible != m_IsVisible)
				{
					m_IsVisible = isVisible;
					UpdateDisplay();
				}
			}
			bool IsExposed() const
			{
				int width = 0;
				return IsExposed(width);
			}
			bool IsExposed(int& width) const;

			void Refresh() const;
			void FitInside();
			bool FitContent();

			bool IsChecked() const
			{
				return m_IsChecked;
			}
			void SetChecked(bool isChecked = true)
			{
				if (isChecked != m_IsChecked)
				{
					m_IsChecked = isChecked;
					UpdateDisplay();
				}
			}

			String GetTitle() const
			{
				return m_Title;
			}
			void SetTitle(const String& title)
			{
				if (title != m_Title)
				{
					m_Title = title;
					UpdateDisplay();
				}
			}

			FlagSet<Alignment> GetTitleAlignment() const
			{
				return m_TitleAlignment;
			}
			void SetTitleAlignment(FlagSet<Alignment> alignment)
			{
				if (alignment != m_TitleAlignment)
				{
					m_TitleAlignment = alignment;
					UpdateDisplay();
				}
			}

			BitmapImage GetIcon() const
			{
				return m_Icon;
			}
			void SetBitmap(const BitmapImage& icon)
			{
				if (!icon.IsSameAs(m_Icon))
				{
					m_Icon = icon;
					UpdateDisplay();
				}
			}

			SortOrder GetSortOrder() const
			{
				return m_SortOrder;
			}
			void SetSortOrder(SortOrder order)
			{
				if (m_SortOrder != order)
				{
					m_SortOrder = order;
					UpdateDisplay();
				}
			}
			void ToggleSortOrder()
			{
				switch (m_SortOrder)
				{
					case SortOrder::Ascending:
					{
						SetSortOrder(SortOrder::Descending);
						break;
					}
					case SortOrder::None:
					case SortOrder::Descending:
					{
						SetSortOrder(SortOrder::Ascending);
						break;
					}
				};
			}

			bool IsCurrent() const;
			bool IsExpander() const;
			bool IsHotTracked() const;

			bool IsFirst() const;
			bool IsDisplayedFirst() const;

			bool IsLast() const;
			bool IsDisplayedLast() const;
	};
}
