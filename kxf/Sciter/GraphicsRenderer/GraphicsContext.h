#pragma once
#include "kxf/Sciter/Common.h"
#include "kxf/Sciter/Utility/HandleWrapper.h"
#include "kxf/Drawing/GDIRenderer/GDIPen.h"
#include "kxf/Drawing/GDIRenderer/GDIBrush.h"
#include <wx/graphics.h>

namespace kxf::Sciter
{
	struct GraphicsContextHandle;
	class ScriptValue;
	class GraphicsText;
	class GraphicsPath;
	class GraphicsBitmap;
	class GraphicsMatrix;
}

namespace kxf::Sciter
{
	class KXF_API GraphicsMatrix final
	{
		friend class GraphicsContext;

		private:
			double m_11 = 1.0;
			double m_12 = 0.0;
			double m_21 = 0.0;
			double m_22 = 1.0;
			double m_dx = 0.0;
			double m_dy = 0.0;

		public:
			GraphicsMatrix() = default;
			GraphicsMatrix(double a, double b, double c, double d, double tx, double ty)
				:m_11(a), m_12(b), m_21(c), m_22(d), m_dx(tx), m_dy(ty)
			{
			}

		public:
			void Get(double& a, double& b, double& c, double& d, double& tx, double& ty)
			{
				a = m_11;
				b = m_12;
				c = m_21;
				d = m_22;
				tx = m_dx;
				ty = m_dy;
			}
	};
}

namespace kxf::Sciter
{
	class KXF_API GraphicsContext final: public HandleWrapper<GraphicsContext, GraphicsContextHandle>
	{
		friend class HandleWrapper<GraphicsContext, GraphicsContextHandle>;

		private:
			GraphicsContextHandle* m_Handle = nullptr;

		private:
			bool DoAcquire(GraphicsContextHandle* handle);
			void DoRelease();

		public:
			GraphicsContext() = default;
			GraphicsContext(GraphicsContextHandle* handle)
				:HandleWrapper(handle)
			{
			}
			GraphicsContext(const GraphicsContext& other)
				:HandleWrapper(other)
			{
			}
			GraphicsContext(GraphicsContext&& other)
				:HandleWrapper(std::move(other))
			{
			}
			GraphicsContext(GraphicsBitmap& bitmap);
			GraphicsContext(const ScriptValue& value);

		public:
			ScriptValue ToScriptValue() const;

			// Drawing functions
			GraphicsPath CreatePath();
			void DrawPath(const GraphicsPath& path, PolygonFill fillStyle = PolygonFill::OddEvenRule);
			void FillPath(const GraphicsPath& path, PolygonFill fillStyle = PolygonFill::OddEvenRule);
			void StrokePath(const GraphicsPath& path);

			void DrawEllipse(const RectD& rect);
			void DrawRectangle(const RectD& rect);
			void DrawRoundedRectangle(const RectD& rect, Angle radius);
			void DrawArc(const RectD& p, const RectD& r, Angle start, Angle sweep);
			void DrawStar(const RectD& p, const RectD& r, Angle start, size_t raysCount);

			void StrokeLine(const PointD& p1, const PointD& p2);
			void StrokeLines(const PointD* points, size_t count);
			void StrokeLines(const PointD* beginPoints, const PointD* endPoints, size_t count);

			RectD DrawBitmap(const GraphicsBitmap& bitmap, const RectD& rect, double opacity = 1.0);
			RectD DrawBitmap(const GraphicsBitmap& bitmap, const PointD& pos, double opacity = 1.0)
			{
				return DrawBitmap(bitmap, RectD(pos.GetX(), pos.GetY(), -1, -1), opacity);
			}

			void DrawText(const GraphicsText& text, const PointD& pos, FlagSet<Alignment> alignment = Alignment::None)
			{
				DrawText(text, pos, MapCornerAlignment(alignment));
			}
			void DrawText(const GraphicsText& text, const PointD& pos, CornerAlignment alignment = CornerAlignment::None);

			// Brush and pen functions
			void SetPen(const GDIPen& pen);
			void SetPen(const wxGraphicsPenInfo& pen);
			void ResetPen();

			void SetBrush(const GDIBrush& brush);
			void SetBrush(const wxGraphicsPenInfo& brush);
			void ResetBrush();

			// Transformation matrix
			PointD Rotate(double angle);
			void Scale(const PointD& scale);
			void Scale(double xScale, double yScale)
			{
				Scale(PointD(xScale, yScale));
			}
			void ScaleTranslate(const PointD& p);
			void ScaleTranslate(double dx, double dy)
			{
				ScaleTranslate(PointD(dx, dy));
			}

			GraphicsMatrix CreateMatrix(double a = 1.0, double b = 0.0, double c = 0.0, double d = 1.0, double tx = 0.0, double ty = 0.0);
			void Transform(const GraphicsMatrix& matrix);

			// Coordinate space
			PointD WorldToScreen(const PointD& p) const;
			PointD ScreenToWorld(const PointD& p) const;

			// Clipping region functions
			void PushClip(const RectD& rect, double opacity = 1.0);
			void PushClip(const GraphicsPath& path, double opacity = 1.0);
			void PopClip();

			// Modifying the state
			void Flush();
			void PushState();
			void PopState();

		public:
			GraphicsContext& operator=(const GraphicsContext& other)
			{
				CopyFrom(other);
				return *this;
			}
			GraphicsContext& operator=(GraphicsContext&& other)
			{
				MoveFrom(other);
				return *this;
			}
			GraphicsContext& operator=(GraphicsContextHandle* handle)
			{
				CopyFrom(handle);
				return *this;
			}
	};
}
