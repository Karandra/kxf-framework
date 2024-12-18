#include "kxf-pch.h"
#include "ImageView.h"
#include "kxf/Drawing/GDIRenderer/GDIWindowContext.h"
#include "kxf/UI/Windows/DrawablePanel.h"
#include "kxf/wxWidgets/StreamWrapper.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ImageView, wxControl);

	void ImageView::DoSetBitmap(const wxGraphicsBitmap& bitmap, const Size& size)
	{
		ScheduleRefresh();

		m_Bitmap = bitmap;
		m_ImageSize = size;
		m_ScaledImageSize = Size(0, 0);
		m_IsAnimation = false;
	}

	void ImageView::OnDrawBackground(wxEraseEvent& event)
	{
		GDIContext dc(*event.GetDC());
		dc.SetBackgroundTransparent();
		dc.SetBackgroundBrush(Drawing::GetStockGDIBrush(StockBrush::Transparent));

		switch (m_BackgroundMode)
		{
			case ImageViewBackground::Solid:
			{
				dc.SetBackgroundBrush(GetBackgroundColour());
				dc.Clear();
				break;
			}
			case ImageViewBackground::Gradient:
			{
				dc.DrawGradientLinear(Rect({0, 0}, Size(GetSize())), GetForegroundColour(), GetBackgroundColour(), m_GradientDirection);
				break;
			}
			case ImageViewBackground::TransparenryPattern:
			{
				DrawablePanel::DrawTransparencyPattern(dc);
				break;
			}
		};
	}
	void ImageView::OnDrawForeground(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		std::unique_ptr<wxGraphicsContext> context(m_Renderer->CreateContext(dc));

		m_ScaledImageSize = DrawablePanel::DrawScaledBitmap(&*context, m_Bitmap, m_ImageSize, Rect({0, 0}, Size(GetClientSize())), m_ScaleMode, m_ScaleFactor);
	}
	void ImageView::OnSize(wxSizeEvent& event)
	{
		Refresh();
		event.Skip();
	}

	bool ImageView::Create(wxWindow* parent,
						   wxWindowID id,
						   FlagSet<WindowStyle> style
	)
	{
		if (wxControl::Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt()))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);
			if (ShouldInheritColours())
			{
				SetBackgroundColour(parent->GetBackgroundColour());
			}

			SetBackgroundStyle(wxBG_STYLE_ERASE);
			Bind(wxEVT_ERASE_BACKGROUND, &ImageView::OnDrawBackground, this);
			Bind(wxEVT_PAINT, &ImageView::OnDrawForeground, this);
			Bind(wxEVT_SIZE, &ImageView::OnSize, this);

			// Direct2D can't be used right now because it crops images and not scales them.
			m_Renderer = wxGraphicsRenderer::GetGDIPlusRenderer();
			if (m_Renderer == nullptr)
			{
				m_Renderer = wxGraphicsRenderer::GetDefaultRenderer();
			}
			return m_Renderer != nullptr;
		}
		return false;
	}

	void ImageView::SetScaleFactor(double factor)
	{
		if (std::abs(m_ScaleFactor - factor) > 0.01)
		{
			m_ScaleFactor = factor;
			ScheduleRefresh();
		}
	}
	void ImageView::SetBitmap(const GDIBitmap& bitmap)
	{
		DoSetBitmap(m_Renderer->CreateBitmap(bitmap.ToWxBitmap()), bitmap.GetSize());
	}
	void ImageView::SetBitmap(const BitmapImage& image)
	{
		DoSetBitmap(m_Renderer->CreateBitmapFromImage(image.ToWxImage()), image.GetSize());
	}
	void ImageView::SetBitmap(const wxGraphicsBitmap& bitmap, const Size& size)
	{
		DoSetBitmap(bitmap, size);
	}
	void ImageView::Load(IInputStream& stream, const UniversallyUniqueID& format, size_t index)
	{
		BitmapImage image;
		image.Load(stream, format, index);
		SetBitmap(image);
	}
}
