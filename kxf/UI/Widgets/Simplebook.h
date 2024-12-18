#pragma once
#include "Common.h"
#include "../IBookWidget.h"
#include "../Private/WxBookCtrlWrapper.h"

namespace kxf::WXUI
{
	class Simplebook;
}

namespace kxf::Widgets
{
	class KXF_API Simplebook: public RTTI::Implementation<Simplebook, Private::BasicWxWidget<Simplebook, WXUI::Simplebook, IBookWidget>>
	{
		private:
			WXUI::Private::WxBookCtrlWrapper m_BookCtrlWrapper;

		public:
			Simplebook();
			~Simplebook();

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

			BitmapImage GetPageImage(size_t index) const override
			{
				return {};
			}
			void SetPageImage(size_t index, const BitmapImage& image) override
			{
			}

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
	};
}
