#include "kxf-pch.h"
#include "ImageGallery.h"
#include "WXUI/ImageGallery.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"

namespace kxf::Widgets
{
	// ImageGallery
	ImageGallery::ImageGallery()
	{
		InitializeWxWidget();
	}
	ImageGallery::~ImageGallery() = default;

	// IWidget
	bool ImageGallery::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IImageGalleryWidget
	size_t ImageGallery::GetItemCount() const
	{
		return Get()->GetItemCount();
	}
	size_t ImageGallery::AddItem(const IImage2D& image)
	{
		return Get()->AddItem(image);
	}
	void ImageGallery::RemoveItem(size_t index)
	{
		Get()->RemoveItem(index);
	}
	void ImageGallery::ClearItems()
	{
		Get()->ClearItems();
	}

	size_t ImageGallery::GetSelectedItem() const
	{
		return Get()->GetSelectedItem();
	}
	void ImageGallery::SetSelectedItem(size_t index)
	{
		Get()->SetSelectedItem(index);
	}

	Size ImageGallery::GetItemSize() const
	{
		return Get()->GetItemSize();
	}
	void ImageGallery::SetItemSize(const Size& size)
	{
		Get()->SetItemSize(size);
	}

	Size ImageGallery::GetSpacing() const
	{
		return Get()->GetSpacing();
	}
	void ImageGallery::SetSpacing(const Size& spacing)
	{
		Get()->SetSpacing(spacing);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> ImageGallery::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
