#include "kxf-pch.h"
#include "ImageView.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"

namespace kxf::WXUI
{
	void ImageView::OnDrawBackground(wxEraseEvent& event)
	{
		auto clientRect = RectF(GetClientRect());
		auto renderer = m_RendererAware->GetActiveGraphicsRenderer();
		auto gc = renderer->CreateLegacyContext(*event.GetDC(), clientRect.GetSize());

		switch (m_BackgroundMode)
		{
			case IImageViewWidget::BackgroundMode::Gradient:
			{
				auto brush = renderer->CreateLinearGradientBrush(clientRect, GradientStops(GetBackgroundColour(), GetForegroundColour()));
				gc->DrawRectangle(clientRect, *brush, NullGraphicsPen);
				break;
			}
			case IImageViewWidget::BackgroundMode::Checker:
			{
				const int sideSize = FromDIP(8);

				gc->Clear(renderer->GetTransparentBrush());
				auto brush = renderer->CreateSolidBrush(GetBackgroundColour());
				auto pen = renderer->CreatePen(GetForegroundColour());

				size_t indexY = 0;
				for (int y = 0; y <= clientRect.GetHeight(); y += sideSize)
				{
					for (int x = indexY % 2 == 0 ? 0 : sideSize; x <= clientRect.GetWidth(); x += 2 * sideSize)
					{
						gc->DrawRectangle(RectF(x, y, sideSize, sideSize), *brush, *pen);
					}
					indexY++;
				}
				break;
			}
			default:
			{
				auto brush = renderer->CreateSolidBrush(GetBackgroundColour());
				gc->Clear(*brush);

				break;
			}
		};
	}
	void ImageView::OnDrawForeground(wxPaintEvent& event)
	{
		auto renderer = m_RendererAware->GetActiveGraphicsRenderer();
		auto gc = renderer->CreateLegacyWindowPaintContext(*this);
		gc->Clear(renderer->GetTransparentBrush());

		if (m_Image)
		{
			auto clientRect = RectF(GetClientRect());

			RectF imageRect;
			switch (m_ScaleMode)
			{
				case IImageViewWidget::ScaleMode::AspectFit:
				case IImageViewWidget::ScaleMode::AspectFill:
				{
					float scaleFactor = 1.0f;
					float scaleX = clientRect.GetWidth() / m_Image->GetWidth();
					float scaleY = clientRect.GetHeight() / m_Image->GetHeight();
					if ((m_ScaleMode == IImageViewWidget::ScaleMode::AspectFit && scaleY < scaleX) || (m_ScaleMode == IImageViewWidget::ScaleMode::AspectFill && scaleY > scaleX))
					{
						scaleFactor = scaleY;
					}
					else
					{
						scaleFactor = scaleX;
					}
					scaleFactor *= m_ScaleFactor;

					imageRect.Width() = m_Image->GetWidth() * scaleFactor;
					imageRect.Height() = m_Image->GetHeight() * scaleFactor;
					break;
				}
				case IImageViewWidget::ScaleMode::Fill:
				{
					imageRect = clientRect;
					break;
				}
				default:
				{
					imageRect.SetSize(m_Image->GetSize());
					break;
				}
			};

			imageRect.X() += (clientRect.GetWidth() - imageRect.GetWidth()) / 2.0f;
			imageRect.Y() += (clientRect.GetHeight() - imageRect.GetHeight()) / 2.0f;
			gc->DrawTexture(*m_Image, imageRect);
		}
	}
	void ImageView::OnSize(wxSizeEvent& event)
	{
		Refresh();
		event.Skip();
	}

	bool ImageView::Create(wxWindow* parent,
						   const String& label,
						   const Point& pos,
						   const Size& size
	)
	{
		if (wxControl::Create(parent, wxID_NONE, pos, size, wxBORDER_THEME))
		{
			SetLabel(label);
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

			return m_Widget.QueryInterface(m_RendererAware);
		}
		return false;
	}
}
