#pragma once
#include "Common.h"
#include "../ITabBookWidget.h"
#include "../IGraphicsRendererAwareWidget.h"
#include "../Private/WxBookCtrlWrapper.h"

namespace kxf::WXUI
{
	class TabBook;
}

namespace kxf::Widgets
{
	class KXF_API TabBook: public RTTI::Implementation<TabBook, Private::BasicWxWidget<TabBook, WXUI::TabBook, ITabBookWidget>, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;
			WXUI::Private::WxBookCtrlWrapper m_BookCtrlWrapper;

		public:
			TabBook();
			~TabBook();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IBookWidget
			size_t InsertPage(IWidget& widget, const String& label, size_t index = IBookWidget::npos) override
			{
				return m_BookCtrlWrapper.InsertPage(widget, label, index);
			}
			void RemovePage(size_t index) override
			{
				m_BookCtrlWrapper.RemovePage(index);
			}
			std::shared_ptr<IWidget> GetPage(size_t index) const override
			{
				return m_BookCtrlWrapper.GetPage(index);
			}
			size_t GetPageCount() const override
			{
				return m_BookCtrlWrapper.GetPageCount();
			}

			String GetPageLabel(size_t index) const override
			{
				return m_BookCtrlWrapper.GetPageLabel(index);
			}
			void SetPageLabel(size_t index, const String& label) const override
			{
				m_BookCtrlWrapper.SetPageLabel(index, label);
			}

			BitmapImage GetPageImage(size_t index) const override;
			void SetPageImage(size_t index, const BitmapImage& image) override;

			size_t GetSelectedPage() const override
			{
				return m_BookCtrlWrapper.GetSelectedPage();
			}
			size_t SetSelectedPage(size_t index) override
			{
				return m_BookCtrlWrapper.SetSelectedPage(index);
			}

			std::shared_ptr<IWidget> GetSelectedWidget() const override
			{
				return m_BookCtrlWrapper.GetSelectedWidget();
			}
			size_t GetSelectedPageByWidget(const IWidget& widget) const override
			{
				return m_BookCtrlWrapper.GetSelectedPageByWidget(widget);
			}

			// ITabBookWidget
			Direction GetTabPosition() const override;
			void SetTabPosition(Direction position) override;

			String GetTabTooltip(size_t index) const override;
			void SetTabTooltip(size_t index, const String& tooltip) override;

			bool IsCloseButtonVisible() const override;
			void SetCloseButtonVisible(bool isVisible = true) override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
