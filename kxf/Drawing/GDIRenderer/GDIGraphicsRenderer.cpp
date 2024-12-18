#include "kxf-pch.h"
#include "GDIGraphicsRenderer.h"
#include "GDIGraphicsContext.h"
#include "GDIGraphicsTexture.h"
#include "GDIGraphicsBrush.h"
#include "GDIGraphicsPen.h"
#include "GDIGraphicsFont.h"
#include "../SVGImage.h"
#include "../BitmapImage.h"
#include "kxf/Utility/Container.h"
#include "kxf-gui/Widgets/IWidget.h"

namespace kxf
{
	// GDIGraphicsRenderer
	GDIPen GDIGraphicsRenderer::ToGDIPen(const IGraphicsPen& pen) const
	{
		if (auto gdiPen = pen.QueryInterface<GDIGraphicsPen>())
		{
			return gdiPen->Get();
		}

		GDIPen gdiPen;
		gdiPen.SetCap(pen.GetLineCap());
		gdiPen.SetJoin(pen.GetLineJoin());
		gdiPen.SetColor(pen.GetColor());
		gdiPen.SetWidth(pen.GetWidth());

		switch (pen.GetStyle())
		{
			case PenStyle::None:
			{
				gdiPen.SetTransparent();
				break;
			}
			case PenStyle::Solid:
			{
				gdiPen.SetSolid();
				break;
			}
			case PenStyle::Hatch:
			{
				auto brush = pen.GetBrush();
				if (std::shared_ptr<IGraphicsHatchBrush> hatchBrush; brush && brush->QueryInterface(hatchBrush))
				{
					gdiPen.SetHatchStyle(hatchBrush->GetHatchStyle());
				}
				break;
			}
			case PenStyle::Dash:
			{
				gdiPen.SetDashStyle(pen.GetDashStyle());
				gdiPen.SetDashes(Utility::Container::ConvertVector<GDIPen::Dash>(pen.GetDashPattern(), [](const auto& dash)
				{
					return static_cast<GDIPen::Dash>(dash);
				}));
				break;
			}
			case PenStyle::Texture:
			{
				auto brush = pen.GetBrush();
				if (std::shared_ptr<IGraphicsTextureBrush> textureBrush; brush && brush->QueryInterface(textureBrush))
				{
					if (auto texture = textureBrush->GetTexture())
					{
						gdiPen.SetStipple(texture->QueryInterface<GDIGraphicsTexture>()->Get());
					}
				}
				break;
			}
		};
		return gdiPen;
	}
	GDIBrush GDIGraphicsRenderer::ToGDIBrush(const IGraphicsBrush& brush) const
	{
		if (auto gdiBrush = brush.QueryInterface<GDIGraphicsBrush>())
		{
			return gdiBrush->Get();
		}

		GDIBrush gdiBrush;
		if (brush.IsTransparent())
		{
			gdiBrush.SetTransparent();
		}
		else if (auto solidBrush = brush.QueryInterface<IGraphicsSolidBrush>())
		{
			gdiBrush.SetSolid();
			gdiBrush.SetColor(solidBrush->GetColor());
		}
		else if (auto hatchBrush = brush.QueryInterface<IGraphicsHatchBrush>())
		{
			gdiBrush.SetHatchStyle(hatchBrush->GetHatchStyle());
			gdiBrush.SetColor(hatchBrush->GetBackgroundColor());
		}
		else if (auto textureBrush = brush.QueryInterface<IGraphicsTextureBrush>())
		{
			if (auto texture = textureBrush->GetTexture())
			{
				gdiBrush.SetStipple(texture->QueryInterface<GDIGraphicsTexture>()->Get());
			}
		}
		return gdiBrush;
	}

	// IGraphicsRenderer
	String GDIGraphicsRenderer::GetName() const
	{
		return "GDI";
	}
	Version GDIGraphicsRenderer::GetVersion() const
	{
		return "1.0";
	}

	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateContext(std::shared_ptr<IGraphicsTexture> texture, IWidget* widget)
	{
		return GDIGraphicsRenderer::CreateLegacyContext(std::move(texture), widget ? widget->GetWxWindow() : nullptr);
	}
	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateWidgetContext(IWidget& widget)
	{
		if (wxWindow* window = widget.GetWxWindow())
		{
			return GDIGraphicsRenderer::CreateLegacyWindowContext(*window);
		}
		return nullptr;
	}
	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateWidgetClientContext(IWidget& widget)
	{
		if (wxWindow* window = widget.GetWxWindow())
		{
			return GDIGraphicsRenderer::CreateLegacyWindowClientContext(*window);
		}
		return nullptr;
	}
	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateWidgetPaintContext(IWidget& widget)
	{
		if (wxWindow* window = widget.GetWxWindow())
		{
			return GDIGraphicsRenderer::CreateLegacyWindowPaintContext(*window);
		}
		return nullptr;
	}
	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateMeasuringContext(IWidget* widget)
	{
		return GDIGraphicsRenderer::CreateLegacyMeasuringContext(widget ? widget->GetWxWindow() : nullptr);
	}

	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateLegacyContext(std::shared_ptr<IGraphicsTexture> texture, wxWindow* window)
	{
		if (texture)
		{
			return std::make_shared<GDIGraphicsMemoryContext>(*this, std::move(texture), window);
		}
		return nullptr;
	}
	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateLegacyContext(wxDC& dc, const Size& size)
	{
		if (dc.IsOk())
		{
			return std::make_shared<GDIGraphicsAnyContext>(*this, dc, size);
		}
		return nullptr;
	}
	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateLegacyWindowContext(wxWindow& window)
	{
		return std::make_shared<GDIGraphicsWindowContext>(*this, window);
	}
	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateLegacyWindowClientContext(wxWindow& window)
	{
		return std::make_shared<GDIGraphicsWindowClientContext>(*this, window);
	}
	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateLegacyWindowPaintContext(wxWindow& window)
	{
		if (window.IsDoubleBuffered())
		{
			return std::make_shared<GDIGraphicsPaintContext>(*this, window);
		}
		else
		{
			return std::make_shared<GDIGraphicsBufferedPaintContext>(*this, window);
		}
	}
	std::shared_ptr<IGraphicsContext> GDIGraphicsRenderer::CreateLegacyMeasuringContext(wxWindow* window)
	{
		return std::make_shared<GDIGraphicsMemoryContext>(*this, nullptr, window);
	}

	// Pen and brush functions
	const kxf::IGraphicsPen& GDIGraphicsRenderer::GetTransparentPen() const
	{
		if (!m_TransparentPen)
		{
			auto pen = std::make_unique<GDIGraphicsPen>(const_cast<GDIGraphicsRenderer&>(*this), Drawing::GetStockColor(StockColor::Transparent), 0.0f);
			pen->Get().SetTransparent();

			m_TransparentPen = std::move(pen);
		}
		return *m_TransparentPen;
	}
	const kxf::IGraphicsBrush& GDIGraphicsRenderer::GetTransparentBrush() const
	{
		if (!m_TransparentBrush)
		{
			auto brush = std::make_unique<GDIGraphicsSolidBrush>(const_cast<GDIGraphicsRenderer&>(*this), Drawing::GetStockColor(StockColor::Transparent));
			brush->Get().SetTransparent();

			m_TransparentBrush = std::move(brush);
		}
		return *m_TransparentBrush;
	}

	std::shared_ptr<IGraphicsPen> GDIGraphicsRenderer::CreatePen(const Color& color, float width)
	{
		return std::make_shared<GDIGraphicsPen>(*this, color, width);
	}
	std::shared_ptr<IGraphicsSolidBrush> GDIGraphicsRenderer::CreateSolidBrush(const Color& color)
	{
		return std::make_shared<GDIGraphicsSolidBrush>(*this, color);
	}
	std::shared_ptr<IGraphicsTextureBrush> GDIGraphicsRenderer::CreateTextureBrush(const BitmapImage& image)
	{
		if (image)
		{
			return std::make_shared<GDIGraphicsTextureBrush>(*this, image);
		}
		return {};
	}
	std::shared_ptr<IGraphicsTextureBrush> GDIGraphicsRenderer::CreateTextureBrush(const GDIBitmap& bitmap)
	{
		if (bitmap)
		{
			return std::make_shared<GDIGraphicsTextureBrush>(*this, bitmap);
		}
		return {};
	}
	std::shared_ptr<IGraphicsLinearGradientBrush> GDIGraphicsRenderer::CreateLinearGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform)
	{
		return std::make_shared<GDIGraphicsLinearGradientBrush>(*this, rect, colors, std::move(transform));
	}
	std::shared_ptr<IGraphicsRadialGradientBrush> GDIGraphicsRenderer::CreateRadialGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform)
	{
		return std::make_shared<GDIGraphicsRadialGradientBrush>(*this, rect, colors, std::move(transform));
	}

	// Texture functions
	std::shared_ptr<IGraphicsTexture> GDIGraphicsRenderer::CreateTexture()
	{
		return std::make_shared<GDIGraphicsTexture>(*this);
	}
	std::shared_ptr<IGraphicsTexture> GDIGraphicsRenderer::CreateTexture(const IImage2D& image)
	{
		if (image)
		{
			if (auto vector = image.QueryInterface<SVGImage>())
			{
				return std::make_shared<GDIGraphicsVectorTexture>(*this, *vector);
			}
			else if (auto bitmap = image.QueryInterface<BitmapImage>())
			{
				return std::make_shared<GDIGraphicsTexture>(*this, *bitmap);
			}
			else
			{
				return std::make_shared<GDIGraphicsTexture>(*this, image.ToBitmapImage());
			}
		}
		return nullptr;
	}
	std::shared_ptr<IGraphicsTexture> GDIGraphicsRenderer::CreateTexture(const BitmapImage& image)
	{
		if (image)
		{
			return std::make_shared<GDIGraphicsTexture>(*this, image);
		}
		return nullptr;
	}
	std::shared_ptr<IGraphicsTexture> GDIGraphicsRenderer::CreateTexture(const SizeF& size, const Color& color)
	{
		return std::make_shared<GDIGraphicsTexture>(*this, size, color);
	}
	std::shared_ptr<IGraphicsTexture> GDIGraphicsRenderer::CreateTexture(const GDIBitmap& bitmap)
	{
		if (bitmap)
		{
			return std::make_shared<GDIGraphicsTexture>(*this, bitmap);
		}
		return nullptr;
	}

	// Text functions
	std::shared_ptr<IGraphicsFont> GDIGraphicsRenderer::CreateFont()
	{
		return std::make_shared<GDIGraphicsFont>(*this);
	}
	std::shared_ptr<IGraphicsFont> GDIGraphicsRenderer::CreateFont(const Font& font)
	{
		return std::make_shared<GDIGraphicsFont>(*this, font);
	}
	std::shared_ptr<IGraphicsFont> GDIGraphicsRenderer::CreateFont(float pointSize, const String& faceName)
	{
		return std::make_shared<GDIGraphicsFont>(*this, pointSize, faceName);
	}
	std::shared_ptr<IGraphicsFont> GDIGraphicsRenderer::CreateFont(const SizeF& pixelSize, const String& faceName)
	{
		return std::make_shared<GDIGraphicsFont>(*this, pixelSize, faceName);
	}
}
