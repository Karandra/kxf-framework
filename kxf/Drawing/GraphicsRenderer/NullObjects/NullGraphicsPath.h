#pragma once
#include "../Common.h"
#include "../IGraphicsPath.h"

namespace kxf::Drawing
{
	class KXF_API NullGraphicsPath final: public IGraphicsPath
	{
		public:
			NullGraphicsPath() noexcept = default;

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return true;
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return other.IsNull();
			}
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return nullptr;
			}

			IGraphicsRenderer& GetRenderer() override
			{
				return Drawing::Private::GetNullGraphicsRenderer();
			}
			void* GetNativeHandle() const
			{
				return nullptr;
			}

			// IGraphicsPath
			bool ContainsPoint(const PointF& point, PolygonFillMode fill = PolygonFillMode::OddEvenRule) const override
			{
				return false;
			}
			PointF GetCurrentPoint() const override
			{
				return {};
			}
			void MoveToPoint(const PointF& point) override
			{
			}
			RectF GetBoundingBox() const override
			{
				return {};
			}

			void AddArc(const PointF& point1, const PointF& point2, float radius) override
			{
			}
			void AddArc(const PointF& point, float radius, Angle start, Angle end, ClockDirection direction) override
			{
			}
			void AddCurve(const PointF& control1, const PointF& control2, const PointF& end) override
			{
			}
			void AddQuadCurve(const PointF& control, const PointF& end) override
			{
			}
			void AddLine(const PointF& point) override
			{
			}
			void AddCircle(const PointF& point, float radius) override
			{
			}
			void AddEllipse(const RectF& rect) override
			{
			}
			void AddRectangle(const RectF& rect) override
			{
			}
			void AddRoundedRectangle(const RectF& rect, float radius) override
			{
			}

			void AddPath(const IGraphicsPath& path) override
			{
			}
			void CloseSubPath() override
			{
			}

			void Transform(AffineMatrixF transform) override
			{
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsPath NullGraphicsPath;
}
