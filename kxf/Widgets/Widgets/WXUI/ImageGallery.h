#pragma once
#include "Common.h"
#include "../../IImageGalleryWidget.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include <wx/vscroll.h>
#include <wx/systhemectrl.h>

namespace kxf::WXUI
{
	class KXF_API ImageGallery: public EvtHandlerWrapper<ImageGallery, wxSystemThemedControl<UI::WindowRefreshScheduler<wxVScrolledWindow>>>
	{
		private:
			IImageGalleryWidget& m_Widget;
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;

			Size m_ItemSize = Size::UnspecifiedSize();
			Size m_Spacing = Size::UnspecifiedSize();
			size_t m_Focused = IImageGalleryWidget::npos;
			size_t m_Selected = IImageGalleryWidget::npos;

			std::vector<std::unique_ptr<IImage2D>> m_Items;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnSize(wxSizeEvent& event);
			void OnMouse(wxMouseEvent& event);
			void OnKillFocus(wxFocusEvent& event);

			Rect GetItemRect(size_t row, size_t columnIndex, size_t beginRow);
			Rect GetItemFullRect(size_t row, size_t columnIndex, size_t beginRow);
			Size GetFinalItemSize() const;
			size_t GetIndexByRowColumn(size_t row, size_t columnIndex, size_t itemsInRow) const;

			size_t CalcItemsPerRow() const;
			size_t CalcRowCount() const;

			void RecalculateDisplay();

		protected:
			// wxWindow
			void OnInternalIdle() override;

			// wxVScrolledWindow
			int OnGetRowHeight(size_t index) const override
			{
				return GetFinalItemSize().GetHeight();
			}

		public:
			ImageGallery(IImageGalleryWidget& widget)
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
			wxBorder GetDefaultBorder() const override
			{
				return wxBORDER_THEME;
			}
			bool ShouldInheritColours() const override
			{
				return true;
			}

			Size GetItemSize() const
			{
				return m_ItemSize;
			}
			void SetItemSize(const Size& size)
			{
				if (size.IsPositive())
				{
					m_ItemSize = size;
				}
				else
				{
					m_ItemSize = Size::UnspecifiedSize();
				}
				ScheduleRefresh();
			}

			Size GetSpacing() const
			{
				return m_Spacing;
			}
			void SetSpacing(const Size& spacing)
			{
				m_Spacing = spacing;
				m_Spacing.SetDefaults({0, 0});

				ScheduleRefresh();
			}

			size_t GetSelectedItem() const
			{
				return m_Selected;
			}
			void SetSelectedItem(size_t index)
			{
				if (index < m_Items.size())
				{
					m_Selected = index;
				}
				else
				{
					m_Selected = IImageGalleryWidget::npos;
				}
			}

			size_t GetItemCount() const
			{
				return m_Items.size();
			}
			size_t AddItem(const IImage2D& image)
			{
				if (image)
				{
					ScheduleRefresh();
					RecalculateDisplay();

					m_Items.emplace_back(image.CloneImage2D());
					return m_Items.size() - 1;
				}
				return IImageGalleryWidget::npos;
			}
			void RemoveItem(size_t index)
			{
				if (index < m_Items.size())
				{
					if (index == m_Focused)
					{
						m_Focused = IImageGalleryWidget::npos;
					}
					if (index == m_Selected)
					{
						m_Selected = IImageGalleryWidget::npos;
					}
					m_Items.erase(m_Items.begin() + index);

					RecalculateDisplay();
					ScheduleRefresh();
				}
			}
			void ClearItems()
			{
				m_Focused = IImageGalleryWidget::npos;
				m_Selected = IImageGalleryWidget::npos;
				m_Items.clear();

				RecalculateDisplay();
				ScheduleRefresh();
			}
	};
}
