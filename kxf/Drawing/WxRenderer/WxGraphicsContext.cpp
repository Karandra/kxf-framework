#include "kxf-pch.h"
#include "WxGraphicsContext.h"
#include "WxGraphicsTexture.h"
#include "WxGraphicsBrush.h"
#include "WxGraphicsPen.h"
#include "WxGraphicsFont.h"
#include "WxGraphicsPath.h"
#include "../BitmapImage.h"
#include "../GDIRenderer/GDIBitmap.h"
#include <wx/msw/dc.h>

namespace kxf
{
	WxGraphicsContext::ChangeTextParameters::ChangeTextParameters(WxGraphicsContext& gc, const IGraphicsFont& font, const IGraphicsBrush& brush)
		:m_Context(*gc.m_Context)
	{
		if (font || brush)
		{
			m_OldFont = gc.MakeGCFont();
			m_Context.SetFont(gc.MakeGCFont(font, brush));
		}
		else
		{
			gc.UpdateCurrentFont();
		}
	}
	WxGraphicsContext::ChangeTextParameters::~ChangeTextParameters()
	{
		if (!m_OldFont.IsNull())
		{
			m_Context.SetFont(m_OldFont);
		}
		if (m_OldAntialiasMode)
		{
			m_Context.SetAntialiasMode(*m_OldAntialiasMode);
		}
		if (m_OldInterpolationQuality)
		{
			m_Context.SetInterpolationQuality(*m_OldInterpolationQuality);
		}
	}

	WxGraphicsContext::ChangeDrawParameters::ChangeDrawParameters(WxGraphicsContext& gc, const IGraphicsBrush& brush, const IGraphicsPen& pen)
		:m_Context(*gc.m_Context)
	{
		const bool canUseNullPen = gc.GetRenderer().CanUseNullPen();

		if (brush)
		{
			if (gc.m_CurrentBrush)
			{
				m_OldBrush = gc.m_CurrentBrush->QueryInterface<WxGraphicsBrush>()->Get();
			}
			m_Context.SetBrush(brush.QueryInterface<WxGraphicsBrush>()->Get());
		}
		if (pen || (!canUseNullPen && brush))
		{
			if (gc.m_CurrentPen)
			{
				m_OldPen = gc.m_CurrentPen->QueryInterface<WxGraphicsPen>()->Get();
			}

			if (!pen && !canUseNullPen)
			{
				m_Context.SetPen(m_Context.CreatePen(brush.GetColor()));
			}
			else
			{
				m_Context.SetPen(pen.QueryInterface<WxGraphicsPen>()->Get());
			}
		}
	}
	WxGraphicsContext::ChangeDrawParameters::~ChangeDrawParameters()
	{
		if (!m_OldBrush.IsNull())
		{
			m_Context.SetBrush(m_OldBrush);
		}
		if (!m_OldPen.IsNull())
		{
			m_Context.SetPen(m_OldPen);
		}
	}
}

namespace kxf
{
	void WxGraphicsContext::CommonInit()
	{
		// Nothing to do
	}

	void WxGraphicsContext::SetupGC(wxWindow* window)
	{
		if (m_GCDC)
		{
			m_GCDC.SetMapMode(GDIMappingMode::Text);
			m_GCDC.SetTextBackground(Drawing::GetStockColor(StockColor::Transparent));

			if (window)
			{
				m_GCDC.SetBackgroundBrush(window->GetBackgroundColour());
			}
		}
		if (m_Context && window)
		{
			SetFont(m_Renderer->CreateFont(window->GetFont()));
			SetFontBrush(m_Renderer->CreateSolidBrush(window->GetForegroundColour()));
		}
	}
	void WxGraphicsContext::CopyAttributesFromDC(const GDIContext& dc)
	{
		if (dc)
		{
			SetFont(std::make_shared<WxGraphicsFont>(*m_Renderer, dc.GetFont()));
			SetFontBrush(std::make_shared<WxGraphicsSolidBrush>(*m_Renderer, dc.GetTextForeground()));
			SetBrush(std::make_shared<WxGraphicsSolidBrush>(*m_Renderer, dc.GetBrush()));
			SetPen(std::make_shared<WxGraphicsPen>(*m_Renderer, dc.GetPen()));
		}
	}
	void WxGraphicsContext::Initialize(WxGraphicsRenderer& rendrer, std::unique_ptr<wxGraphicsContext> gc)
	{
		m_Renderer = &rendrer;
		m_Context = gc.get();
		m_WxGCDC.SetGraphicsContext(gc.release());
		m_GCDC = GDIContext(m_WxGCDC);

		CommonInit();
	}

	BitmapImage& WxGraphicsContext::InitTextureBuffer(std::shared_ptr<IGraphicsTexture> texture)
	{
		BitmapImage& image = texture->QueryInterface<WxGraphicsTexture>()->GetImage();
		image.SetAreaRGBA(image.GetSize(), Drawing::GetStockColor(StockColor::Transparent).GetFixed8());

		m_BufferTexture = std::move(texture);
		return image;
	}

	wxGraphicsFont WxGraphicsContext::MakeGCFont() const
	{
		return m_Renderer->Get().CreateFont(m_CurrentFont.ToWxFont(), m_CurrentFontColor);
	}
	wxGraphicsFont WxGraphicsContext::MakeGCFont(const IGraphicsFont& font, const Color& color) const
	{
		GDIFont gdiFont;
		if (auto fontWx = font.QueryInterface<WxGraphicsFont>())
		{
			gdiFont = fontWx->GetFont();
		}
		else
		{
			gdiFont = font.ToFont();
		}
		return m_Renderer->Get().CreateFont(gdiFont.ToWxFont(), color);
	}
	wxGraphicsFont WxGraphicsContext::MakeGCFont(const IGraphicsFont& font, const IGraphicsBrush& brush) const
	{
		return MakeGCFont(font, brush ? brush.GetColor() : m_CurrentFontColor);
	}

	void WxGraphicsContext::UpdateCurrentFont()
	{
		if (!m_CurrentFontValid)
		{
			m_Context->SetFont(MakeGCFont());
			m_CurrentFontValid = true;
		}
	}

	// Feature support
	FlagSet<GraphicsContextFeature> WxGraphicsContext::GetSupportedFeatures() const
	{
		if (m_Context)
		{
			// Assume we support all operations (except bounding box), we can't check that anyway
			FlagSet<GraphicsContextFeature> features;
			features.Add(GraphicsContextFeature::ClippingRegion);
			features.Add(GraphicsContextFeature::TransformationMatrix);
			features.Add(GraphicsContextFeature::DrawText);
			features.Add(GraphicsContextFeature::DrawShape);
			features.Add(GraphicsContextFeature::DrawTexture);
			features.Add(GraphicsContextFeature::TextMeasurement);
			features.Add(GraphicsContextFeature::Antialiasing);
			features.Add(GraphicsContextFeature::Interpolation);
			features.Add(GraphicsContextFeature::Composition);
			features.Add(GraphicsContextFeature::Layers);
			features.Add(GraphicsContextFeature::States);

			return features;
		}
		return {};
	}

	// Clipping region functions
	void WxGraphicsContext::ClipBoxRegion(const RectF& rect)
	{
		m_Context->Clip(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
	}
	void WxGraphicsContext::ResetClipRegion()
	{
		m_Context->ResetClip();
	}
	RectF WxGraphicsContext::GetClipBox() const
	{
		wxDouble x = 0;
		wxDouble y = 0;
		wxDouble width = 0;
		wxDouble height = 0;
		m_Context->GetClipBox(&x, &y, &width, &height);

		return {static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)};
	}

	// Transformation matrix
	AffineMatrixF WxGraphicsContext::GetTransform() const
	{
		wxGraphicsMatrix transform = m_Context->GetTransform();
		if (!transform.IsNull())
		{
			return m_Renderer->ToAffineMatrix(transform);
		}
		return {};
	}
	bool WxGraphicsContext::SetTransform(const AffineMatrixF& transform)
	{
		m_Context->SetTransform(m_Renderer->FromAffineMatrix(transform));
		return true;
	}

	bool WxGraphicsContext::TransformInvert()
	{
		AffineMatrixF transform = m_Renderer->ToAffineMatrix(m_Context->GetTransform());
		if (transform.Invert())
		{
			m_Context->SetTransform(m_Renderer->FromAffineMatrix(transform));
			return true;
		}
		return false;
	}
	void WxGraphicsContext::TransformRotate(Angle angle)
	{
		if (angle.IsValid() && angle.ToNormalized() != 0)
		{
			m_Context->Rotate(angle.ToRadians());
		}
	}
	void WxGraphicsContext::TransformScale(float xScale, float yScale)
	{
		m_Context->Scale(xScale, yScale);
	}
	void WxGraphicsContext::TransformTranslate(float dx, float dy)
	{
		if (dx != 0 || dy != 0)
		{
			m_Context->Translate(dx, dy);
		}
	}
	void WxGraphicsContext::TransformConcat(const AffineMatrixF& transform)
	{
		m_Context->ConcatTransform(m_Renderer->FromAffineMatrix(transform));
	}

	// Pen and brush functions
	std::shared_ptr<IGraphicsPen> WxGraphicsContext::GetPen() const
	{
		return m_CurrentPen;
	}
	void WxGraphicsContext::SetPen(std::shared_ptr<IGraphicsPen> pen)
	{
		if (pen && *pen)
		{
			if (auto penWx = pen->QueryInterface<WxGraphicsPen>())
			{
				m_Context->SetPen(penWx->Get());
				m_CurrentPen = std::move(pen);

				return;
			}
		}

		m_CurrentPen = {};
		m_Context->SetPen(wxGraphicsPen());
	}

	std::shared_ptr<IGraphicsBrush> WxGraphicsContext::GetBrush() const
	{
		return m_CurrentBrush;
	}
	void WxGraphicsContext::SetBrush(std::shared_ptr<IGraphicsBrush> brush)
	{
		if (brush && *brush)
		{
			if (auto brushWx = brush->QueryInterface<WxGraphicsBrush>())
			{
				m_Context->SetBrush(brushWx->Get());
				m_CurrentBrush = std::move(brush);

				return;
			}
		}

		m_CurrentBrush = {};
		m_Context->SetBrush(wxGraphicsBrush());
	}

	// Path functions
	void WxGraphicsContext::StrokePath(const IGraphicsPath& path)
	{
		m_Context->StrokePath(path.QueryInterface<WxGraphicsPath>()->Get());
	}
	void WxGraphicsContext::FillPath(const IGraphicsPath& path, PolygonFill fill)
	{
		if (auto fillMode = Drawing::Private::MapPolygonFill(fill))
		{
			m_Context->FillPath(path.QueryInterface<WxGraphicsPath>()->Get(), *fillMode);
		}
	}
	void WxGraphicsContext::DrawPath(const IGraphicsPath& path, PolygonFill fill)
	{
		if (auto fillMode = Drawing::Private::MapPolygonFill(fill))
		{
			m_Context->DrawPath(path.QueryInterface<WxGraphicsPath>()->Get(), *fillMode);
		}
	}

	// Texture functions
	void WxGraphicsContext::DrawTexture(const IGraphicsTexture& texture, const RectF& rect)
	{
		if (!rect.IsEmpty() && texture)
		{
			if (auto textureWx = texture.QueryInterface<WxGraphicsTexture>())
			{
				if (m_Renderer->CanRescaleBitmapOnDraw() || textureWx->GetSize() == rect.GetSize())
				{
					m_Context->DrawBitmap(textureWx->Get(), rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
				}
				else
				{
					BitmapImage image = textureWx->GetImage();
					image.Rescale(rect.GetSize(), m_InterpolationQuality);

					m_Context->DrawBitmap(m_Renderer->Get().CreateBitmapFromImage(image.ToWxImage()), rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
				}
				CalcBoundingBox(rect);
			}
			else if (auto textureVectorWx = texture.QueryInterface<WxGraphicsVectorTexture>())
			{
				m_Context->DrawBitmap(textureVectorWx->Get(rect.GetSize(), m_InterpolationQuality), rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
				CalcBoundingBox(rect);
			}
			else
			{
				WxGraphicsContext::DrawTexture(texture.ToBitmapImage(), rect);
			}
		}
	}
	void WxGraphicsContext::DrawTexture(const BitmapImage& image, const RectF& rect)
	{
		if (m_Renderer->CanRescaleBitmapOnDraw() || SizeF(image.GetSize()) == rect.GetSize())
		{
			m_Context->DrawBitmap(m_Renderer->Get().CreateBitmapFromImage(image.ToWxImage()), rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
		}
		else
		{
			BitmapImage temp = image.ToBitmapImage(rect.GetSize(), m_InterpolationQuality);
			m_Context->DrawBitmap(m_Renderer->Get().CreateBitmapFromImage(temp.ToWxImage()), rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
		}
	}
	void WxGraphicsContext::DrawTexture(const IImage2D& image, const RectF& rect)
	{
		if (image && !rect.IsEmpty())
		{
			BitmapImage temp = image.ToBitmapImage(rect.GetSize(), m_InterpolationQuality);
			m_Context->DrawBitmap(m_Renderer->Get().CreateBitmapFromImage(temp.ToWxImage()), rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());

			CalcBoundingBox(rect);
		}
	}

	// Text functions
	std::shared_ptr<IGraphicsFont> WxGraphicsContext::GetFont() const
	{
		if (m_CurrentFont)
		{
			return std::make_shared<WxGraphicsFont>(*m_Renderer, m_CurrentFont);
		}
		return nullptr;
	}
	void WxGraphicsContext::SetFont(std::shared_ptr<IGraphicsFont> font)
	{
		m_CurrentFont = {};
		if (font && *font)
		{
			if (auto fontWx = font->QueryInterface<WxGraphicsFont>())
			{
				m_CurrentFont = fontWx->GetFont();
			}
			else
			{
				m_CurrentFont = font->ToFont();
			}
		}
		InvalidateCurrentFont();
	}

	std::shared_ptr<IGraphicsBrush> WxGraphicsContext::GetFontBrush() const
	{
		if (m_CurrentFontColor)
		{
			return std::make_shared<WxGraphicsSolidBrush>(*m_Renderer, m_CurrentFontColor);
		}
		return nullptr;
	}
	void WxGraphicsContext::SetFontBrush(std::shared_ptr<IGraphicsBrush> brush)
	{
		m_CurrentFontColor = {};
		if (brush && *brush)
		{
			if (auto solidBrush = brush->QueryInterface<IGraphicsSolidBrush>())
			{
				m_CurrentFontColor = solidBrush->GetColor();
			}
		}
		InvalidateCurrentFont();
	}

	SizeF WxGraphicsContext::GetTextExtent(const String& text, const IGraphicsFont& font) const
	{
		if (!text.IsEmpty())
		{
			wxGraphicsFont oldFont;
			if (font)
			{
				oldFont = MakeGCFont();
				m_Context->SetFont(MakeGCFont(font));
			}
			else
			{
				const_cast<WxGraphicsContext&>(*this).UpdateCurrentFont();
			}

			wxDouble width = 0;
			wxDouble height = 0;
			m_Context->GetTextExtent(text, &width, &height, nullptr, nullptr);

			if (!oldFont.IsNull())
			{
				m_Context->SetFont(oldFont);
			}
			return {static_cast<float>(width), static_cast<float>(height)};
		}
		return {};
	}
	FontMetricsF WxGraphicsContext::GetFontMetrics(const IGraphicsFont& font) const
	{
		wxGraphicsFont oldFont;
		if (font)
		{
			oldFont = MakeGCFont();
			m_Context->SetFont(MakeGCFont(font));
		}
		else
		{
			const_cast<WxGraphicsContext&>(*this).UpdateCurrentFont();
		}

		wxDouble width = 0;
		wxDouble height = 0;
		wxDouble descent = 0;
		wxDouble externalLeading = 0;
		m_Context->GetTextExtent(wxS("W"), &width, &height, &descent, &externalLeading);

		if (!oldFont.IsNull())
		{
			m_Context->SetFont(oldFont);
		}
		return FontMetricsF(height, 0, descent, width, 0, externalLeading);
	}
	std::vector<float> WxGraphicsContext::GetPartialTextExtents(const String& text, const IGraphicsFont& font) const
	{
		if (!text.IsEmpty())
		{
			wxGraphicsFont oldFont;
			if (font)
			{
				oldFont = MakeGCFont();
				m_Context->SetFont(MakeGCFont(font));
			}
			else
			{
				const_cast<WxGraphicsContext&>(*this).UpdateCurrentFont();
			}

			wxArrayDouble gcWidths;
			m_Context->GetPartialTextExtents(text, gcWidths);

			std::vector<float> widths;
			widths.resize(gcWidths.size());
			std::copy(gcWidths.begin(), gcWidths.end(), widths.begin());

			if (!oldFont.IsNull())
			{
				m_Context->SetFont(oldFont);
			}
			return widths;
		}
		return {};
	}

	void WxGraphicsContext::DrawText(const String& text, const PointF& point, const IGraphicsFont& font, const IGraphicsBrush& brush)
	{
		if (!text.IsEmpty())
		{
			ChangeTextParameters textParametrs(*this, font, brush, m_TextAntialiasMode);

			m_Context->DrawText(text, point.GetX(), point.GetY());
			CalcBoundingBox(point);
		}
	}
	void WxGraphicsContext::DrawRotatedText(const String& text, const PointF& point, Angle angle, const IGraphicsFont& font, const IGraphicsBrush& brush)
	{
		if (!text.IsEmpty())
		{
			ChangeTextParameters textParametrs(*this, font, brush, m_TextAntialiasMode);

			m_Context->DrawText(text, point.GetX(), point.GetY(), angle.ToRadians());
			CalcBoundingBox(point);
		}
	}
	RectF WxGraphicsContext::DrawLabel(const String& text, const RectF& rect, const IGraphicsTexture& icon, const IGraphicsFont& font, const IGraphicsBrush& brush, FlagSet<Alignment> alignment, size_t acceleratorIndex)
	{
		if (!rect.IsEmpty() && (!text.IsEmpty() || icon))
		{
			ChangeTextParameters textParametrs(*this, font, brush, m_TextAntialiasMode);

			Rect boundingBox;
			if (icon)
			{
				if (auto iconWx = icon.QueryInterface<WxGraphicsTexture>())
				{
					boundingBox = m_GCDC.DrawLabel(text, rect, iconWx->GetImage().ToGDIBitmap(), alignment, acceleratorIndex);
				}
				else
				{
					boundingBox = m_GCDC.DrawLabel(text, rect, icon.ToBitmapImage().ToGDIBitmap(), alignment, acceleratorIndex);
				}
			}
			else
			{
				boundingBox = m_GCDC.DrawLabel(text, rect, alignment, acceleratorIndex);
			}

			CalcBoundingBox(boundingBox);
			return boundingBox;
		}
		return {};
	}

	String WxGraphicsContext::EllipsizeText(const String& text, float maxWidth, EllipsizeMode mode, FlagSet<EllipsizeFlag> flags, const IGraphicsFont& font) const
	{
		return EllipsizeTextGeneric(text, maxWidth, mode, flags);
	}

	// Drawing functions
	void WxGraphicsContext::Clear(const IGraphicsBrush& brush)
	{
		WxGraphicsContext::DrawRectangle({{0, 0}, GetSize()}, brush);
	}
	void WxGraphicsContext::DrawCircle(const PointF& pos, float radius, const IGraphicsBrush& brush, const IGraphicsPen& pen)
	{
		if (radius != 0)
		{
			ChangeDrawParameters drawParameters(*this, brush, pen);

			RectF rect(pos.GetX() - radius, pos.GetY() - radius, 2 * radius, 2 * radius);
			m_Context->DrawEllipse(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
			CalcBoundingBox(rect);
		}
	}
	void WxGraphicsContext::DrawEllipse(const RectF& rect, const IGraphicsBrush& brush, const IGraphicsPen& pen)
	{
		if (!rect.IsEmpty())
		{
			ChangeDrawParameters drawParameters(*this, brush, pen);

			m_Context->DrawEllipse(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
			CalcBoundingBox(rect);
		}
	}
	void WxGraphicsContext::DrawRectangle(const RectF& rect, const IGraphicsBrush& brush, const IGraphicsPen& pen)
	{
		if (!rect.IsEmpty())
		{
			ChangeDrawParameters drawParameters(*this, brush, pen);

			m_Context->DrawRectangle(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
			CalcBoundingBox(rect);
		}
	}
	void WxGraphicsContext::DrawRoundedRectangle(const RectF& rect, float radius, const IGraphicsBrush& brush, const IGraphicsPen& pen)
	{
		if (!rect.IsEmpty())
		{
			ChangeDrawParameters drawParameters(*this, brush, pen);

			m_Context->DrawRoundedRectangle(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), radius);
			CalcBoundingBox(rect);
		}
	}
	void WxGraphicsContext::DrawLine(const PointF& point1, const PointF& point2, const IGraphicsPen& pen)
	{
		ChangeDrawParameters drawParameters(*this, NullGraphicsBrush, pen);

		m_Context->StrokeLine(point1.GetX(), point1.GetY(), point2.GetX(), point2.GetY());
		CalcBoundingBox(point1);
		CalcBoundingBox(point2);
	}
	void WxGraphicsContext::DrawPolyLine(const PointF* points, size_t count, const IGraphicsPen& pen)
	{
		if (count != 0)
		{
			ChangeDrawParameters drawParameters(*this, NullGraphicsBrush, pen);

			std::vector<wxPoint2DDouble> gcPoints;
			gcPoints.resize(count);
			std::copy_n(points, count, gcPoints.begin());
			m_Context->StrokeLines(gcPoints.size(), gcPoints.data());

			for (size_t i = 0; i < count; i++)
			{
				CalcBoundingBox(points[i]);
			}
		}
	}
	void WxGraphicsContext::DrawDisconnectedLines(const PointF* startPoints, const PointF* endPoints, size_t count, const IGraphicsPen& pen)
	{
		if (count != 0)
		{
			ChangeDrawParameters drawParameters(*this, NullGraphicsBrush, pen);

			for (size_t i = 0; i < count; i++)
			{
				m_Context->StrokeLine(startPoints[i].GetX(), startPoints[i].GetY(), endPoints[i].GetX(), endPoints[i].GetY());

				CalcBoundingBox(startPoints[i]);
				CalcBoundingBox(endPoints[i]);
			}
		}
	}

	void WxGraphicsContext::DrawGDI(const RectF& rect, std::function<void(GDIContext& dc)> func)
	{
		if (GDIBitmap bitmap = DrawGDIOnBitmap(rect, std::move(func)))
		{
			m_Context->DrawBitmap(bitmap.ToWxBitmap(), rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
			CalcBoundingBox(rect);
		}
	}

	// Getting and setting parameters
	SizeF WxGraphicsContext::GetSize() const
	{
		wxDouble width = 0;
		wxDouble height = 0;
		m_Context->GetSize(&width, &height);

		return {static_cast<float>(width), static_cast<float>(height)};
	}
	SizeF WxGraphicsContext::GetDPI() const
	{
		wxDouble width = 0;
		wxDouble height = 0;
		m_Context->GetDPI(&width, &height);

		return {static_cast<float>(width), static_cast<float>(height)};
	}
	wxWindow* WxGraphicsContext::GetWindow() const
	{
		return m_Context->GetWindow();
	}

	AntialiasMode WxGraphicsContext::GetAntialiasMode() const
	{
		switch (m_Context->GetAntialiasMode())
		{
			case wxANTIALIAS_DEFAULT:
			{
				return AntialiasMode::Default;
			}
		};
		return AntialiasMode::None;
	}
	bool WxGraphicsContext::SetAntialiasMode(AntialiasMode mode)
	{
		switch (mode)
		{
			case AntialiasMode::Default:
			case AntialiasMode::BestAvailable:
			{
				m_AntialiasMode = AntialiasMode::Default;
				return m_Context->SetAntialiasMode(wxANTIALIAS_DEFAULT);
			}
			default:
			{
				m_AntialiasMode = AntialiasMode::None;
				return m_Context->SetAntialiasMode(wxANTIALIAS_NONE);
			}
		};
	}

	AntialiasMode WxGraphicsContext::GetTextAntialiasMode() const
	{
		switch (m_TextAntialiasMode)
		{
			case wxANTIALIAS_DEFAULT:
			{
				return AntialiasMode::Default;
			}
		};
		return AntialiasMode::None;
	}
	bool WxGraphicsContext::SetTextAntialiasMode(AntialiasMode mode)
	{
		switch (mode)
		{
			case AntialiasMode::Default:
			case AntialiasMode::BestAvailable:
			{
				m_TextAntialiasMode = wxAntialiasMode::wxANTIALIAS_DEFAULT;
				return true;
			}
			default:
			{
				m_TextAntialiasMode = wxAntialiasMode::wxANTIALIAS_NONE;
				return true;
			}
		};
	}

	CompositionMode WxGraphicsContext::GetCompositionMode() const
	{
		return m_CompositionMode;
	}
	bool WxGraphicsContext::SetCompositionMode(CompositionMode mode)
	{
		auto DoSetMode = [this](CompositionMode mode)
		{
			switch (mode)
			{
				case CompositionMode::None:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_INVALID);
				}
				case CompositionMode::Clear:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_CLEAR);
				}
				case CompositionMode::Add:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_ADD);
				}
				case CompositionMode::Xor:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_XOR);
				}
				case CompositionMode::Source:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_SOURCE);
				}
				case CompositionMode::Over:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_OVER);
				}
				case CompositionMode::In:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_IN);
				}
				case CompositionMode::Out:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_OUT);
				}
				case CompositionMode::Atop:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_ATOP);
				}
				case CompositionMode::Dest:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_DEST);
				}
				case CompositionMode::DestOver:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_DEST_OVER);
				}
				case CompositionMode::DestIn:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_DEST_IN);
				}
				case CompositionMode::DestOut:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_DEST_OUT);
				}
				case CompositionMode::DestAtop:
				{
					return m_Context->SetCompositionMode(wxCOMPOSITION_DEST_ATOP);
				}
			};
			return false;
		};
		if (DoSetMode(mode))
		{
			m_CompositionMode = mode;
			return true;
		}
		return false;
	}

	InterpolationQuality WxGraphicsContext::GetInterpolationQuality() const
	{
		return m_InterpolationQuality;
	}
	bool WxGraphicsContext::SetInterpolationQuality(InterpolationQuality quality)
	{
		auto DoSetQuality = [this](InterpolationQuality quality)
		{
			switch (quality)
			{
				case InterpolationQuality::None:
				{
					return m_Context->SetInterpolationQuality(wxINTERPOLATION_NONE);
				}
				case InterpolationQuality::Default:
				{
					return m_Context->SetInterpolationQuality(wxINTERPOLATION_DEFAULT);
				}
				case InterpolationQuality::FastestAvailable:
				case InterpolationQuality::NearestNeighbor:
				{
					return m_Context->SetInterpolationQuality(wxINTERPOLATION_FAST);
				}
				case InterpolationQuality::Bilinear:
				case InterpolationQuality::Bicubic:
				{
					return m_Context->SetInterpolationQuality(wxINTERPOLATION_GOOD);
				}
				case InterpolationQuality::BestAvailable:
				{
					return m_Context->SetInterpolationQuality(wxINTERPOLATION_BEST);
				}
			};
			return false;
		};
		if (DoSetQuality(quality))
		{
			m_InterpolationQuality = quality;
			return true;
		}
		return false;
	}

	// Bounding box functions
	RectF WxGraphicsContext::GetBoundingBox() const
	{
		if (m_BoundingBox)
		{
			return *m_BoundingBox;
		}
		return {};
	}
	void WxGraphicsContext::CalcBoundingBox(const PointF& point)
	{
		if (m_BoundingBox)
		{
			m_BoundingBox->IncludePoint(point);
		}
		else
		{
			m_BoundingBox = RectF(point, point);
		}
	}
	void WxGraphicsContext::ResetBoundingBox()
	{
		m_BoundingBox = {};
	}
}

namespace kxf
{
	void WxGraphicsGDIContext::Initialize(WxGraphicsRenderer& rendrer, wxDC& dc, const Size& size)
	{
		m_DC = dc;

		Size dcSize = Size(dc.GetSize());
		dcSize.SetDefaults(size);

		auto texture = rendrer.CreateTexture(dcSize, Drawing::GetStockColor(StockColor::Transparent));
		m_Image = &InitTextureBuffer(std::move(texture));

		WxGraphicsContext::Initialize(rendrer, std::unique_ptr<wxGraphicsContext>(rendrer.Get().CreateContextFromImage(m_Image->ToWxImage())));
	}

	bool WxGraphicsGDIContext::FlushContent()
	{
		if (m_Context && m_DC)
		{
			m_Context->Flush();
			m_DC.DrawBitmap(m_Image->ToGDIBitmap(), {0, 0});

			return true;
		}
		return false;
	}
	void WxGraphicsGDIContext::ResetContext()
	{
		m_DC = {};
	}
}
