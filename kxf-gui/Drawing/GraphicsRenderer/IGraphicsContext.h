#pragma once
#include "Common.h"
#include "IGraphicsObject.h"
#include "NullObjects/NullGraphicsObject.h"
#include "NullObjects/NullGraphicsRenderer.h"
#include "NullObjects/NullGraphicsFont.h"
#include "NullObjects/NullGraphicsPen.h"
#include "NullObjects/NullGraphicsBrush.h"
#include "NullObjects/NullGraphicsTexture.h"
#include "NullObjects/NullGraphicsPath.h"
class wxWindow;
class wxScrollHelper;

namespace kxf
{
	class IImage2D;
	class BitmapImage;
	class GDIContext;
	class GDIBitmap;

	class IGraphicsRenderer;
	class IGraphicsTexture;
	class IGraphicsPath;
	class IGraphicsFont;
	class IGraphicsPen;
	class IGraphicsBrush;
	class IGraphicsSolidBrush;
	class IGraphicsTextureBrush;
	class IGraphicsLinearGradientBrush;
	class IGraphicsRadialGradientBrush;
}

namespace kxf
{
	enum class GraphicsContextFeature: uint32_t
	{
		None = 0,

		ClippingRegion = 1 << 0,
		TransformationMatrix = 1 << 1,
		DrawPath = 1 << 2,
		DrawText = 1 << 3,
		DrawShape = 1 << 4,
		DrawTexture = 1 << 5,
		TextMeasurement = 1 << 6,
		Antialiasing = 1 << 7,
		Interpolation = 1 << 8,
		Composition = 1 << 9,
		Layers = 1 << 10,
		States = 1 << 11,
		BoundingBox = 1 << 12
	};
}

namespace kxf
{
	class KXF_API IGraphicsContext: public RTTI::ExtendInterface<IGraphicsContext, IGraphicsObject>
	{
		kxf_RTTI_DeclareIID(IGraphicsContext, {0x7a91357c, 0x87f9, 0x4230, {0x92, 0xf, 0x54, 0xbf, 0xde, 0x42, 0x35, 0xf5}});

		protected:
			GDIBitmap DrawGDIOnBitmap(const RectF& rect, std::function<void(GDIContext& dc)> func, bool forceAlpha = false);
			String EllipsizeTextGeneric(const String& text, float maxWidth, EllipsizeMode mode, FlagSet<EllipsizeFlag> flags = {}, const IGraphicsFont& font = NullGraphicsFont) const;

		public:
			virtual ~IGraphicsContext() = default;

		public:
			// Feature support
			virtual FlagSet<GraphicsContextFeature> GetSupportedFeatures() const = 0;

			// Clipping region functions
			virtual void ClipBoxRegion(const RectF& rect) = 0;
			virtual void ResetClipRegion() = 0;
			virtual RectF GetClipBox() const = 0;

			// Transformation matrix
			virtual AffineMatrixF GetTransform() const = 0;
			virtual bool SetTransform(const AffineMatrixF& transform) = 0;

			virtual bool TransformInvert() = 0;
			virtual void TransformRotate(Angle angle) = 0;
			virtual void TransformScale(float xScale, float yScale) = 0;
			virtual void TransformTranslate(float dx, float dy) = 0;
			virtual void TransformConcat(const AffineMatrixF& matrix) = 0;

			// Pen and brush functions
			virtual std::shared_ptr<IGraphicsPen> GetPen() const = 0;
			virtual void SetPen(std::shared_ptr<IGraphicsPen> pen) = 0;

			virtual std::shared_ptr<IGraphicsBrush> GetBrush() const = 0;

			// Path functions
			virtual void StrokePath(const IGraphicsPath& path) = 0;
			virtual void FillPath(const IGraphicsPath& path, PolygonFillMode fill = PolygonFillMode::OddEvenRule) = 0;
			virtual void DrawPath(const IGraphicsPath& path, PolygonFillMode fill = PolygonFillMode::OddEvenRule) = 0;

			// Texture functions
			virtual void DrawTexture(const IGraphicsTexture& texture, const RectF& rect) = 0;
			virtual void DrawTexture(const BitmapImage& image, const RectF& rect) = 0;
			virtual void DrawTexture(const IImage2D& image, const RectF& rect) = 0;

			// Text functions
			virtual std::shared_ptr<IGraphicsFont> GetFont() const = 0;
			virtual void SetFont(std::shared_ptr<IGraphicsFont> font) = 0;
			virtual void SetBrush(std::shared_ptr<IGraphicsBrush> brush) = 0;

			virtual std::shared_ptr<IGraphicsBrush> GetFontBrush() const = 0;
			virtual void SetFontBrush(std::shared_ptr<IGraphicsBrush> brush) = 0;

			virtual SizeF GetTextExtent(const String& text, const IGraphicsFont& font = NullGraphicsFont) const = 0;
			virtual FontMetricsF GetFontMetrics(const IGraphicsFont& font = NullGraphicsFont) const = 0;
			virtual std::vector<float> GetPartialTextExtents(const String& text, const IGraphicsFont& font = NullGraphicsFont) const = 0;

			virtual void DrawText(const String& text, const PointF& point, const IGraphicsFont& font = NullGraphicsFont, const IGraphicsBrush& brush = NullGraphicsBrush) = 0;
			virtual void DrawRotatedText(const String& text, const PointF& point, Angle angle, const IGraphicsFont& font = NullGraphicsFont, const IGraphicsBrush& brush = NullGraphicsBrush) = 0;

			RectF DrawLabel(const String& text, const RectF& rect, FlagSet<Alignment> alignment = {}, size_t acceleratorIndex = String::npos)
			{
				return DrawLabel(text, rect, NullGraphicsTexture, NullGraphicsFont, NullGraphicsBrush, alignment, acceleratorIndex);
			}
			RectF DrawLabel(const String& text, const RectF& rect, const IGraphicsFont& font, const IGraphicsBrush& brush, FlagSet<Alignment> alignment = {}, size_t acceleratorIndex = String::npos)
			{
				return DrawLabel(text, rect, NullGraphicsTexture, font, brush, alignment, acceleratorIndex);
			}
			RectF DrawLabel(const String& text, const RectF& rect, const IImage2D& icon, FlagSet<Alignment> alignment = {}, size_t acceleratorIndex = String::npos)
			{
				if (auto texture = GetRenderer().CreateTexture(icon))
				{
					return DrawLabel(text, rect, *texture, NullGraphicsFont, NullGraphicsBrush, alignment, acceleratorIndex);
				}
				else
				{
					return DrawLabel(text, rect, NullGraphicsTexture, NullGraphicsFont, NullGraphicsBrush, alignment, acceleratorIndex);
				}
			}
			RectF DrawLabel(const String& text, const RectF& rect, const BitmapImage& icon, FlagSet<Alignment> alignment = {}, size_t acceleratorIndex = String::npos)
			{
				if (auto texture = GetRenderer().CreateTexture(icon))
				{
					return DrawLabel(text, rect, *texture, NullGraphicsFont, NullGraphicsBrush, alignment, acceleratorIndex);
				}
				else
				{
					return DrawLabel(text, rect, NullGraphicsTexture, NullGraphicsFont, NullGraphicsBrush, alignment, acceleratorIndex);
				}
			}
			RectF DrawLabel(const String& text, const RectF& rect, const IGraphicsTexture& icon, FlagSet<Alignment> alignment = {}, size_t acceleratorIndex = String::npos)
			{
				return DrawLabel(text, rect, icon, NullGraphicsFont, NullGraphicsBrush, alignment, acceleratorIndex);
			}
			virtual RectF DrawLabel(const String& text,
									const RectF& rect,
									const IGraphicsTexture& icon,
									const IGraphicsFont& font,
									const IGraphicsBrush& brush,
									FlagSet<Alignment> alignment = {},
									size_t acceleratorIndex = String::npos) = 0;

			virtual String EllipsizeText(const String& text, float maxWidth, EllipsizeMode mode, FlagSet<EllipsizeFlag> flags = {}, const IGraphicsFont& font = NullGraphicsFont) const = 0;

			// Drawing functions
			virtual void Clear(const IGraphicsBrush& brush) = 0;

			virtual void DrawCircle(const PointF& pos, float radius, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen) = 0;

			virtual void DrawEllipse(const RectF& rect, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen) = 0;
			void DrawEllipse(const PointF& pos, const Size& size, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen)
			{
				DrawEllipse({pos, size}, brush, pen);
			}

			virtual void DrawRectangle(const RectF& rect, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen) = 0;
			void DrawRectangle(const PointF& pos, const Size& size, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen)
			{
				DrawRectangle({pos, size}, brush, pen);
			}

			virtual void DrawRoundedRectangle(const RectF& rect, float radius, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen) = 0;
			void DrawRoundedRectangle(const PointF& pos, const Size& size, float radius, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen)
			{
				DrawRoundedRectangle({pos, size}, radius, brush, pen);
			}

			virtual void DrawLine(const PointF& point1, const PointF& point2, const IGraphicsPen& pen = NullGraphicsPen) = 0;
			virtual void DrawPolyLine(const PointF* points, size_t count, const IGraphicsPen& pen = NullGraphicsPen) = 0;

			template<size_t N>
			void DrawPolyLine(const PointF(&points)[N], const IGraphicsPen& pen = NullGraphicsPen)
			{
				DrawPolyLine(points, N, pen);
			}

			template<size_t N>
			void DrawPolyLine(const std::array<PointF, N>& points, const IGraphicsPen& pen = NullGraphicsPen)
			{
				DrawPolyLine(points.data(), points.size(), pen);
			}

			virtual void DrawDisconnectedLines(const PointF* startPoints, const PointF* endPoints, size_t count, const IGraphicsPen& pen = NullGraphicsPen) = 0;

			template<size_t N>
			void DrawDisconnectedLines(const PointF(&startPoints)[N], const PointF(&endPoints)[N], const IGraphicsPen& pen = NullGraphicsPen)
			{
				DrawDisconnectedLines(startPoints, endPoints, N, pen);
			}

			template<size_t N>
			void DrawDisconnectedLines(const std::array<PointF, N>& startPoints, const std::array<PointF, N>& endPoints, const IGraphicsPen& pen = NullGraphicsPen)
			{
				DrawDisconnectedLines(startPoints.data(), endPoints.data(), N, pen);
			}

			virtual void DrawGDI(const RectF& rect, std::function<void(GDIContext& dc)> func) = 0;

			// Getting and setting parameters
			virtual SizeF GetSize() const = 0;
			virtual SizeF GetDPI() const = 0;
			virtual wxWindow* GetWindow() const = 0;

			virtual AntialiasMode GetAntialiasMode() const = 0;
			virtual bool SetAntialiasMode(AntialiasMode mode) = 0;

			virtual AntialiasMode GetTextAntialiasMode() const = 0;
			virtual bool SetTextAntialiasMode(AntialiasMode mode) = 0;

			virtual CompositionMode GetCompositionMode() const = 0;
			virtual bool SetCompositionMode(CompositionMode mode) = 0;

			virtual InterpolationQuality GetInterpolationQuality() const = 0;
			virtual bool SetInterpolationQuality(InterpolationQuality quality) = 0;

			// Bounding box functions
			virtual RectF GetBoundingBox() const = 0;
			virtual void CalcBoundingBox(const PointF& point) = 0;
			void CalcBoundingBox(const RectF& rect)
			{
				if (!rect.IsEmpty())
				{
					CalcBoundingBox(rect.GetTopLeft());
					CalcBoundingBox(rect.GetRightBottom());
				}
			}
			virtual void ResetBoundingBox() = 0;

			// Page and document start/end functions
			virtual bool StartDocument(const String& message) = 0;
			virtual void EndDocument() = 0;

			virtual void StartPage() = 0;
			virtual void EndPage() = 0;

			// Modifying the state
			virtual void BeginLayer(float opacity = 1.0f) = 0;
			virtual void EndLayer() = 0;

			virtual void PushState() = 0;
			virtual void PopState() = 0;

			virtual void Flush() = 0;

			// Offset management
			virtual bool ShouldOffset() const = 0;
			virtual void EnableOffset(bool enable = true) = 0;

			virtual void OffsetForScrollableArea(const PointF& scrollPos, const PointF& scrollInc, const PointF& scale = {}, const PointF& origin = {});
			void OffsetForScrollableArea(const wxScrollHelper& scrollableWidget);
	};
}
