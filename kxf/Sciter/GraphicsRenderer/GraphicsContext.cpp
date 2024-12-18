#include "kxf-pch.h"
#include "GraphicsContext.h"
#include "GraphicsBitmap.h"
#include "GraphicsText.h"
#include "GraphicsPath.h"
#include "kxf/Sciter/ScriptValue.h"
#include "kxf/Sciter/SciterAPI.h"
#include "kxf/Sciter/Private/Conversion.h"

namespace kxf::Sciter
{
	void DoDrawPath(GraphicsContext& gc, const GraphicsPath& path, DRAW_PATH_MODE mode)
	{
		GetGrapchicsAPI()->gDrawPath(ToSciterGraphicsContext(gc.GetHandle()), ToSciterPath(path.GetHandle()), mode);
	}

	SCITER_LINE_JOIN_TYPE MapPenJoinType(wxPenJoin type)
	{
		switch (type)
		{
			case wxPenJoin::wxJOIN_BEVEL:
			{
				return SCITER_JOIN_BEVEL;
			}
			case wxPenJoin::wxJOIN_MITER:
			{
				return SCITER_JOIN_MITER;
			}
			case wxPenJoin::wxJOIN_ROUND:
			{
				return SCITER_JOIN_ROUND;
			}
		};
		return SCITER_JOIN_MITER_OR_BEVEL;
	}
	SCITER_LINE_CAP_TYPE MapPenCapType(wxPenCap type)
	{
		switch (type)
		{
			case wxPenCap::wxCAP_BUTT:
			{
				return SCITER_LINE_CAP_BUTT;
			}
			case wxPenCap::wxCAP_PROJECTING:
			{
				return SCITER_LINE_CAP_SQUARE;
			}
			case wxPenCap::wxCAP_ROUND:
			{
				return SCITER_LINE_CAP_ROUND;
			}
		};
		return SCITER_LINE_CAP_ROUND;
	}
	std::vector<SC_COLOR_STOP> MakeSciterGradientStops(const wxGraphicsGradientStops& stops)
	{
		std::vector<SC_COLOR_STOP> nativeStops;
		nativeStops.reserve(stops.GetCount());
		for (size_t i = 0; i < stops.GetCount(); i++)
		{
			const wxGraphicsGradientStop& stopItem = stops.Item(i);

			SC_COLOR_STOP color;
			color.color = CreateSciterColor(stopItem.GetColour());
			color.offset = stopItem.GetPosition();
			nativeStops.push_back(std::move(color));
		}
		return nativeStops;
	}
}

namespace kxf::Sciter
{
	bool GraphicsContext::DoAcquire(GraphicsContextHandle* handle)
	{
		return GetGrapchicsAPI()->gAddRef(ToSciterGraphicsContext(handle)) == GRAPHIN_OK;
	}
	void GraphicsContext::DoRelease()
	{
		GetGrapchicsAPI()->gRelease(ToSciterGraphicsContext(m_Handle));
	}

	GraphicsContext::GraphicsContext(const ScriptValue& value)
	{
		HGFX handle = nullptr;
		if (GetGrapchicsAPI()->vUnWrapGfx(ToSciterScriptValue(value.GetNativeValue()), &handle))
		{
			Acquire(FromSciterGraphicsContext(handle));
		}
	}
	GraphicsContext::GraphicsContext(GraphicsBitmap& bitmap)
	{
		HGFX handle = nullptr;
		if (GetGrapchicsAPI()->gCreate(ToSciterImage(bitmap.GetHandle()), &handle))
		{
			Acquire(FromSciterGraphicsContext(handle));
		}
	}

	ScriptValue GraphicsContext::ToScriptValue() const
	{
		ScriptValue value;
		if (GetGrapchicsAPI()->vWrapGfx(ToSciterGraphicsContext(m_Handle), ToSciterScriptValue(value.GetNativeValue())) == GRAPHIN_OK)
		{
			return value;
		}
		return {};
	}

	// Drawing functions
	GraphicsPath GraphicsContext::CreatePath()
	{
		HPATH path = nullptr;
		GetGrapchicsAPI()->pathCreate(&path);
		return FromSciterPath(path);
	}
	void GraphicsContext::DrawPath(const GraphicsPath& path, PolygonFill fillStyle)
	{
		DoDrawPath(*this, path, DRAW_PATH_MODE::DRAW_FILL_AND_STROKE);
	}
	void GraphicsContext::FillPath(const GraphicsPath& path, PolygonFill fillStyle)
	{
		DoDrawPath(*this, path, DRAW_PATH_MODE::DRAW_FILL_ONLY);
	}
	void GraphicsContext::StrokePath(const GraphicsPath& path)
	{
		DoDrawPath(*this, path, DRAW_PATH_MODE::DRAW_STROKE_ONLY);
	}

	void GraphicsContext::DrawEllipse(const RectD& rect)
	{
		auto p1 = rect.GetLeftTop();
		auto p2 = rect.GetRightBottom();
		GetGrapchicsAPI()->gEllipse(ToSciterGraphicsContext(m_Handle), p1.GetX(), p1.GetY(), p2.GetX(), p2.GetY());
	}
	void GraphicsContext::DrawRectangle(const RectD& rect)
	{
		auto p1 = rect.GetLeftTop();
		auto p2 = rect.GetRightBottom();
		GetGrapchicsAPI()->gRectangle(ToSciterGraphicsContext(m_Handle), p1.GetX(), p1.GetY(), p2.GetX(), p2.GetY());
	}
	void GraphicsContext::DrawRoundedRectangle(const RectD& rect, Angle radius)
	{
		auto p1 = rect.GetLeftTop();
		auto p2 = rect.GetRightBottom();

		std::array<SC_DIM, 8> radii8;
		radii8.fill(radius.ToRadians());

		GetGrapchicsAPI()->gRoundedRectangle(ToSciterGraphicsContext(m_Handle), p1.GetX(), p1.GetY(), p2.GetX(), p2.GetY(), radii8.data());
	}
	void GraphicsContext::DrawArc(const RectD& p, const RectD& r, Angle start, Angle sweep)
	{
		GetGrapchicsAPI()->gArc(ToSciterGraphicsContext(m_Handle), p.GetX(), p.GetY(), r.GetX(), r.GetY(), start.ToRadians(), sweep.ToRadians());
	}
	void GraphicsContext::DrawStar(const RectD& p, const RectD& r, Angle start, size_t raysCount)
	{
		GetGrapchicsAPI()->gStar(ToSciterGraphicsContext(m_Handle), p.GetX(), p.GetY(), r.GetX(), r.GetY(), start.ToRadians(), static_cast<uint32_t>(raysCount));
	}

	void GraphicsContext::StrokeLine(const PointD& p1, const PointD& p2)
	{
		GetGrapchicsAPI()->gLine(ToSciterGraphicsContext(m_Handle), p1.GetX(), p1.GetY(), p2.GetX(), p2.GetY());
	}
	void GraphicsContext::StrokeLines(const PointD* points, size_t count)
	{
		std::vector<SC_POS> pointData;
		pointData.reserve(count * 2);
		for (size_t i = 0; i < count; i++)
		{
			pointData.push_back(points[i].GetX());
			pointData.push_back(points[i].GetY());
		}

		GetGrapchicsAPI()->gPolygon(ToSciterGraphicsContext(m_Handle), pointData.data(), count);
	}
	void GraphicsContext::StrokeLines(const PointD* beginPoints, const PointD* endPoints, size_t count)
	{
		std::vector<SC_POS> pointData;
		pointData.reserve(count * 2);

		auto PackPoints = [&](const PointD* points)
		{
			for (size_t i = 0; i < count; i++)
			{
				pointData.push_back(points[i].GetX());
				pointData.push_back(points[i].GetY());
			}
		};
		PackPoints(beginPoints);
		PackPoints(endPoints);

		GetGrapchicsAPI()->gPolygon(ToSciterGraphicsContext(m_Handle), pointData.data(), count);
	}

	RectD GraphicsContext::DrawBitmap(const GraphicsBitmap& bitmap, const RectD& rect, double opacity)
	{
		const SC_DIM drawWidth = rect.GetWidth();
		const SC_DIM drawHeight = rect.GetHeight();
		const float drawOpacity = opacity;

		UINT x = 0;
		UINT y = 0;
		UINT width = 0;
		UINT height = 0;

		GetGrapchicsAPI()->gDrawImage(ToSciterGraphicsContext(m_Handle),
									  ToSciterImage(bitmap.GetHandle()),
									  rect.GetX(),
									  rect.GetY(),
									  drawWidth ? &drawWidth : nullptr,
									  drawHeight >= 0 ? &drawHeight : nullptr,
									  &x, &y, &width, &height,
									  &drawOpacity);
		return RectD(x, y, width, height);
	}
	void GraphicsContext::DrawText(const GraphicsText& text, const PointD& pos, CornerAlignment alignment)
	{
		GetGrapchicsAPI()->gDrawText(ToSciterGraphicsContext(m_Handle), ToSciterText(text.GetHandle()), pos.GetX(), pos.GetY(), ToInt(alignment));
	}

	// Brush and pen functions
	void GraphicsContext::SetPen(const GDIPen& pen)
	{
		if (pen)
		{
			wxGraphicsPenInfo penInfo;
			penInfo.Colour(pen.GetColor());
			penInfo.Width(pen.GetWidth());
			penInfo.Join(static_cast<wxPenJoin>(pen.GetJoin()));
			penInfo.Cap(static_cast<wxPenCap>(pen.GetCap()));
			penInfo.Style(pen.ToWxPen().GetStyle());
			penInfo.Stipple(pen.GetStipple().ToWxBitmap());

			GDIPen::Dash* dashes = nullptr;
			size_t dashCount = pen.GetDashes(dashes);
			penInfo.Dashes(dashCount, dashes);

			SetPen(penInfo);
		}
		else
		{
			ResetPen();
		}
	}
	void GraphicsContext::SetPen(const wxGraphicsPenInfo& pen)
	{
		GetGrapchicsAPI()->gLineWidth(ToSciterGraphicsContext(m_Handle), pen.GetWidth());
		GetGrapchicsAPI()->gLineJoin(ToSciterGraphicsContext(m_Handle), MapPenJoinType(pen.GetJoin()));
		GetGrapchicsAPI()->gLineCap(ToSciterGraphicsContext(m_Handle), MapPenCapType(pen.GetCap()));
		GetGrapchicsAPI()->gLineColor(ToSciterGraphicsContext(m_Handle), CreateSciterColor(pen.GetColour()));

		if (pen.GetGradientType() == wxGradientType::wxGRADIENT_LINEAR)
		{
			std::vector<SC_COLOR_STOP> stops = MakeSciterGradientStops(pen.GetStops());
			GetGrapchicsAPI()->gLineGradientLinear(ToSciterGraphicsContext(m_Handle), pen.GetX1(), pen.GetY1(), pen.GetX2(), pen.GetY2(), stops.data(), stops.size());
		}
		else if (pen.GetGradientType() == wxGradientType::wxGRADIENT_RADIAL)
		{
			std::vector<SC_COLOR_STOP> stops = MakeSciterGradientStops(pen.GetStops());
			GetGrapchicsAPI()->gLineGradientRadial(ToSciterGraphicsContext(m_Handle), pen.GetX1(), pen.GetY1(), pen.GetRadius(), pen.GetRadius(), stops.data(), stops.size());
		}
	}
	void GraphicsContext::ResetPen()
	{
		GetGrapchicsAPI()->gLineWidth(ToSciterGraphicsContext(m_Handle), 0);
	}

	void GraphicsContext::SetBrush(const GDIBrush& brush)
	{
		if (brush)
		{
			wxGraphicsPenInfo brushInfo;
			brushInfo.Colour(brush.GetColor());

			SetBrush(brushInfo);
		}
		else
		{
			ResetBrush();
		}
	}
	void GraphicsContext::SetBrush(const wxGraphicsPenInfo& brush)
	{
		GetGrapchicsAPI()->gFillColor(ToSciterGraphicsContext(m_Handle), CreateSciterColor(brush.GetColour()));

		if (brush.GetGradientType() == wxGradientType::wxGRADIENT_LINEAR)
		{
			std::vector<SC_COLOR_STOP> stops = MakeSciterGradientStops(brush.GetStops());
			GetGrapchicsAPI()->gFillGradientLinear(ToSciterGraphicsContext(m_Handle), brush.GetX1(), brush.GetY1(), brush.GetX2(), brush.GetY2(), stops.data(), stops.size());
		}
		else if (brush.GetGradientType() == wxGradientType::wxGRADIENT_RADIAL)
		{
			std::vector<SC_COLOR_STOP> stops = MakeSciterGradientStops(brush.GetStops());
			GetGrapchicsAPI()->gFillGradientRadial(ToSciterGraphicsContext(m_Handle), brush.GetX1(), brush.GetY1(), brush.GetRadius(), brush.GetRadius(), stops.data(), stops.size());
		}
	}
	void GraphicsContext::ResetBrush()
	{
		GetGrapchicsAPI()->gFillColor(ToSciterGraphicsContext(m_Handle), CreateSciterColor(wxNullColour));
	}

	// Transformation matrix
	PointD GraphicsContext::Rotate(double angle)
	{
		SC_POS x = 0;
		SC_POS y = 0;
		if (GetGrapchicsAPI()->gRotate(ToSciterGraphicsContext(m_Handle), angle, &x, &y) == GRAPHIN_OK)
		{
			return PointD(x, y);
		}
		return PointD::UnspecifiedPosition();
	}
	void GraphicsContext::Scale(const PointD& scale)
	{
		GetGrapchicsAPI()->gScale(ToSciterGraphicsContext(m_Handle), scale.GetX(), scale.GetY());
	}
	void GraphicsContext::ScaleTranslate(const PointD& p)
	{
		GetGrapchicsAPI()->gTranslate(ToSciterGraphicsContext(m_Handle), p.GetX(), p.GetY());
	}

	GraphicsMatrix GraphicsContext::CreateMatrix(double a, double b, double c, double d, double tx, double ty)
	{
		return GraphicsMatrix(a, b, c, d, tx, ty);
	}
	void GraphicsContext::Transform(const GraphicsMatrix& matrix)
	{
		GetGrapchicsAPI()->gTransform(ToSciterGraphicsContext(m_Handle), matrix.m_11, matrix.m_12, matrix.m_21, matrix.m_22, matrix.m_dx, matrix.m_dy);
	}

	// Coordinate space
	PointD GraphicsContext::WorldToScreen(const PointD& p) const
	{
		SC_POS x = p.GetX();
		SC_POS y = p.GetY();
		GetGrapchicsAPI()->gWorldToScreen(ToSciterGraphicsContext(m_Handle), &x, &y);

		return PointD(x, y);
	}
	PointD GraphicsContext::ScreenToWorld(const PointD& p) const
	{
		SC_POS x = p.GetX();
		SC_POS y = p.GetY();
		GetGrapchicsAPI()->gScreenToWorld(ToSciterGraphicsContext(m_Handle), &x, &y);

		return PointD(x, y);
	}

	// Clipping region functions
	void GraphicsContext::PushClip(const RectD& rect, double opacity)
	{
		auto p1 = rect.GetLeftTop();
		auto p2 = rect.GetRightBottom();
		GetGrapchicsAPI()->gPushClipBox(ToSciterGraphicsContext(m_Handle), p1.GetX(), p1.GetY(), p2.GetX(), p2.GetY(), opacity);
	}
	void GraphicsContext::PushClip(const GraphicsPath& path, double opacity)
	{
		GetGrapchicsAPI()->gPushClipPath(ToSciterGraphicsContext(m_Handle), ToSciterPath(path.GetHandle()), opacity);
	}
	void GraphicsContext::PopClip()
	{
		GetGrapchicsAPI()->gPopClip(ToSciterGraphicsContext(m_Handle));
	}

	// Modifying the state
	void GraphicsContext::Flush()
	{
		// Nothing to do
	}
	void GraphicsContext::PushState()
	{
		GetGrapchicsAPI()->gStateSave(ToSciterGraphicsContext(m_Handle));
	}
	void GraphicsContext::PopState()
	{
		GetGrapchicsAPI()->gStateRestore(ToSciterGraphicsContext(m_Handle));
	}
}
