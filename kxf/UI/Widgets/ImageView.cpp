#include "kxf-pch.h"
#include "ImageView.h"
#include "WXUI/ImageView.h"
#include "kxf/Drawing/GraphicsRenderer.h"

namespace kxf::Widgets
{
	// ImageView
	ImageView::ImageView()
	{
		InitializeWxWidget();
	}
	ImageView::~ImageView() = default;

	// IWidget
	bool ImageView::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IImageViewWidget
	IImageViewWidget::BackgroundMode ImageView::GetBackgroundMode()
	{
		return Get()->GetBackgroundMode();
	}
	void ImageView::SetBackgroundMode(BackgroundMode mode)
	{
		Get()->SetBackgroundMode(mode);
	}

	Direction ImageView::GetGradientDirection() const
	{
		return Get()->GetGradientDirection();
	}
	void ImageView::SetGradientDirection(Direction direction)
	{
		Get()->SetGradientDirection(direction);
	}

	IImageViewWidget::ScaleMode ImageView::GetScaleMode() const
	{
		return Get()->GetScaleMode();
	}
	void ImageView::SetScaleMode(ScaleMode mode)
	{
		Get()->SetScaleMode(mode);
	}

	float ImageView::GetScaleFactor() const
	{
		return Get()->GetScaleFactor();
	}
	void ImageView::SetScaleFactor(float factor)
	{
		Get()->SetScaleFactor(factor);
	}

	void ImageView::SetImage(const IImage2D& image)
	{
		Get()->SetImage(image);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> ImageView::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
