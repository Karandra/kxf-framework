#pragma once
#include "Common.h"
#include "IGraphicsObject.h"

namespace kxf
{
	class KXF_API IGraphicsPath: public RTTI::ExtendInterface<IGraphicsPath, IGraphicsObject>
	{
		kxf_RTTI_DeclareIID(IGraphicsPath, {0x1c3cae6c, 0xf9a3, 0x4d24, {0xbd, 0xa0, 0x16, 0x18, 0x2d, 0x7d, 0x46, 0xc2}});

		public:
			virtual ~IGraphicsPath() = default;

		public:
			virtual bool ContainsPoint(const PointF& point, PolygonFillMode fill = PolygonFillMode::OddEvenRule) const = 0;
			virtual PointF GetCurrentPoint() const = 0;
			virtual void MoveToPoint(const PointF& point) = 0;
			virtual RectF GetBoundingBox() const = 0;

			virtual void AddArc(const PointF& point1, const PointF& point2, float radius) = 0;
			virtual void AddArc(const PointF& point, float radius, Angle start, Angle end, ClockDirection direction) = 0;
			virtual void AddCurve(const PointF& control1, const PointF& control2, const PointF& end) = 0;
			virtual void AddQuadCurve(const PointF& control, const PointF& end) = 0;
			virtual void AddLine(const PointF& point) = 0;
			virtual void AddCircle(const PointF& point, float radius) = 0;
			virtual void AddEllipse(const RectF& rect) = 0;
			virtual void AddRectangle(const RectF& rect) = 0;
			virtual void AddRoundedRectangle(const RectF& rect, float radius) = 0;

			virtual void AddPath(const IGraphicsPath& path) = 0;
			virtual void CloseSubPath() = 0;

			virtual void Transform(AffineMatrixF transform) = 0;
	};
}
