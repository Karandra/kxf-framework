#pragma once
#include "Common.h"
#include "../IImageGalleryWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::WXUI
{
	class ImageGallery;
}

namespace kxf::Widgets
{
	class KXF_API ImageGallery: public RTTI::Implementation<ImageGallery, Private::BasicWxWidget<ImageGallery, WXUI::ImageGallery, IImageGalleryWidget>, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			ImageGallery();
			~ImageGallery();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IImageGalleryWidget
			size_t GetItemCount() const override;
			size_t AddItem(const IImage2D& image) override;
			void RemoveItem(size_t index) override;
			void ClearItems() override;

			size_t GetSelectedItem() const override;
			void SetSelectedItem(size_t index) override;

			Size GetItemSize() const override;
			void SetItemSize(const Size& size) override;

			Size GetSpacing() const override;
			void SetSpacing(const Size& spacing) override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
