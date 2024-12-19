#pragma once
#include "Common.h"
#include "WxGraphicsRenderer.h"
#include "../GraphicsRenderer/IGraphicsPath.h"
#include <wx/graphics.h>

namespace kxf
{
	class KXF_API WxGraphicsPath: public RTTI::ExtendInterface<WxGraphicsPath, IGraphicsPath>
	{
		kxf_RTTI_DeclareIID(WxGraphicsPath, {0x7a5f4e69, 0x24e7, 0x49b0, {0xa6, 0x36, 0x31, 0x4b, 0xf7, 0x1f, 0x32, 0xb2}});

		protected:
			WxGraphicsRenderer* m_Renderer = nullptr;
			wxGraphicsPath m_Graphics;

		public:
			WxGraphicsPath() noexcept = default;
			WxGraphicsPath(WxGraphicsRenderer& rendrer)
				:m_Renderer(&rendrer)
			{
			}
			WxGraphicsPath(WxGraphicsRenderer& rendrer, const wxGraphicsPath& path)
				:m_Renderer(&rendrer), m_Graphics(path)
			{
			}

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_Graphics.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<WxGraphicsPath>())
				{
					return m_Graphics.IsSameAs(object->m_Graphics);
				}
				return false;
			}
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<WxGraphicsPath>(*this);
			}

			WxGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Graphics.GetGraphicsData();
			}

			// IGraphicsPath
			bool ContainsPoint(const PointF& point, PolygonFillMode fill = PolygonFillMode::OddEvenRule) const override;
			PointF GetCurrentPoint() const override
			{
				return PointD(m_Graphics.GetCurrentPoint());
			}
			void MoveToPoint(const PointF& point) override
			{
				m_Graphics.MoveToPoint(point);
			}
			RectF GetBoundingBox() const override
			{
				return RectD(m_Graphics.GetBox());
			}

			void AddArc(const PointF& point1, const PointF& point2, float radius) override
			{
				m_Graphics.AddArcToPoint(point1.GetX(), point1.GetY(), point2.GetX(), point2.GetY(), radius);
			}
			void AddArc(const PointF& point, float radius, Angle start, Angle end, ClockDirection direction) override
			{
				m_Graphics.AddArc(point, radius, start.ToRadians(), end.ToRadians(), direction == ClockDirection::Clockwise);
			}
			void AddCurve(const PointF& control1, const PointF& control2, const PointF& end) override
			{
				m_Graphics.AddCurveToPoint(control1, control2, end);
			}
			void AddQuadCurve(const PointF& control, const PointF& end) override
			{
				m_Graphics.AddQuadCurveToPoint(control.GetX(), control.GetY(), end.GetX(), end.GetY());
			}
			void AddLine(const PointF& point) override
			{
				m_Graphics.AddLineToPoint(point);
			}
			void AddCircle(const PointF& point, float radius) override
			{
				m_Graphics.AddCircle(point.GetX(), point.GetY(), radius);
			}
			void AddEllipse(const RectF& rect) override
			{
				m_Graphics.AddEllipse(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
			}
			void AddRectangle(const RectF& rect) override
			{
				m_Graphics.AddRectangle(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
			}
			void AddRoundedRectangle(const RectF& rect, float radius) override
			{
				m_Graphics.AddRoundedRectangle(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), radius);
			}

			void AddPath(const IGraphicsPath& path) override
			{
				if (path)
				{
					m_Graphics.AddPath(path.QueryInterface<WxGraphicsPath>()->Get());
				}
			}
			void CloseSubPath() override
			{
				m_Graphics.CloseSubpath();
			}

			void Transform(AffineMatrixF transform) override
			{
				m_Graphics.Transform(m_Renderer->FromAffineMatrix(transform));
			}

			// WxGraphicsPath
			const wxGraphicsPath& Get() const
			{
				return m_Graphics;
			}
			wxGraphicsPath& Get()
			{
				return m_Graphics;
			}

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}
	};
}
