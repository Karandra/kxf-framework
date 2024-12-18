#pragma once
#include "Common.h"
#include "../IImageViewWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::WXUI
{
	class ImageView;
}

namespace kxf::Widgets
{
	class KXF_API ImageView: public RTTI::Implementation<ImageView, Private::BasicWxWidget<ImageView, WXUI::ImageView, IImageViewWidget>, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			ImageView();
			~ImageView();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IImageViewWidget
			BackgroundMode GetBackgroundMode() override;
			void SetBackgroundMode(BackgroundMode mode) override;

			Direction GetGradientDirection() const override;
			void SetGradientDirection(Direction direction) override;

			ScaleMode GetScaleMode() const override;
			void SetScaleMode(ScaleMode mode) override;

			float GetScaleFactor() const override;
			void SetScaleFactor(float factor) override;

			void SetImage(const IImage2D& image) override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
