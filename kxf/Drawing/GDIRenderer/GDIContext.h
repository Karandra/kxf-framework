#pragma once
#include "Common.h"
#include "GDIPen.h"
#include "GDIBrush.h"
#include "Private/GDI.h"
#include "kxf/Core/Private/Mapping.h"
#include "kxf/Widgets/Common.h"
#include "../AffineMatrix.h"
#include "../Private/Common.h"
#include <wx/dc.h>
#include <wx/graphics.h>

namespace kxf
{
	class IGraphicsContext;
}

namespace kxf
{
	class KXF_API GDIContext: public IGDIObject
	{
		public:
			template<class T>
			using GDICoordPair = Geometry::OrderedPairTemplate<T, int>;

		protected:
			wxDC* m_DC = nullptr;

		public:
			GDIContext() = default;
			GDIContext(wxDC& other)
				:m_DC(&other)
			{
			}
			GDIContext(const GDIContext&) = delete;
			GDIContext(GDIContext&& other) noexcept
				:m_DC(other.m_DC)
			{
				other.m_DC = nullptr;
			}
			virtual ~GDIContext() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_DC || !m_DC->IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDIContext>())
				{
					return GDIContext::GetHandle() == other.GetHandle();
				}
				return false;
			}
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return nullptr;
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// GDIContext
			const wxDC& ToWxDC() const noexcept
			{
				return *m_DC;
			}
			wxDC& ToWxDC() noexcept
			{
				return *m_DC;
			}

			void CopyAttributes(const GDIContext& other)
			{
				m_DC->CopyAttributes(*other.m_DC);
			}
			Color GetPixel(const Point& point) const
			{
				wxColour color;
				if (m_DC->GetPixel(point, &color))
				{
					return color;
				}
				return {};
			}
			wxWindow* GetWindow() const
			{
				return m_DC->GetWindow();
			}

			GDIBitmap ToBtmap() const
			{
				return m_DC->GetAsBitmap();
			}
			GDIBitmap ToBitmap(const Rect& subRect) const
			{
				wxRect subRectWx = subRect;
				return m_DC->GetAsBitmap(&subRectWx);
			}

			ColorDepth GetColorDepth() const
			{
				return m_DC->GetDepth();
			}
			Size GetSize() const
			{
				return Size(m_DC->GetSize());
			}
			int GetWidth() const
			{
				return m_DC->GetSize().GetWidth();
			}
			int GetHeight() const
			{
				return m_DC->GetSize().GetHeight();
			}

			Size GetSizeMM() const
			{
				return Size(m_DC->GetSizeMM());
			}
			Size GetDPI() const
			{
				return Size(m_DC->GetPPI());
			}

			SizeD GetUserScale() const
			{
				double x = 0;
				double y = 0;
				m_DC->GetUserScale(&x, &y);

				return {x, y};
			}
			void SetUserScale(const SizeD& userScale)
			{
				m_DC->SetUserScale(userScale.GetWidth(), userScale.GetHeight());
			}

			SizeD GetLogicalScale() const
			{
				double x = 0;
				double y = 0;
				m_DC->GetLogicalScale(&x, &y);
				return {};
			}
			void SetLogicalScale(const SizeD& userScale)
			{
				m_DC->SetLogicalScale(userScale.GetWidth(), userScale.GetHeight());
			}

			Point GetLogicalOrigin() const
			{
				return Point(m_DC->GetLogicalOrigin());
			}
			void SetLogicalOrigin(const Point& point)
			{
				m_DC->SetLogicalOrigin(point.GetX(), point.GetY());
			}

			Point GetDeviceOrigin() const
			{
				return Point(m_DC->GetDeviceOrigin());
			}
			void SetDeviceOrigin(const Point& point)
			{
				m_DC->SetDeviceOrigin(point.GetX(), point.GetY());
			}
			void SetDeviceLocalOrigin(const Point& point)
			{
				m_DC->SetDeviceLocalOrigin(point.GetX(), point.GetY());
			}
			void SetAxisOrientation(bool xLeftRight, bool yBottomUp)
			{
				m_DC->SetAxisOrientation(xLeftRight, yBottomUp);
			}

			GDIMappingMode GetMapMode() const
			{
				return static_cast<GDIMappingMode>(m_DC->GetMapMode());
			}
			void SetMapMode(GDIMappingMode mode) const
			{
				m_DC->SetMapMode(static_cast<wxMappingMode>(mode));
			}

			GDILogicalFunction GetLogicalFunction() const
			{
				return static_cast<GDILogicalFunction>(m_DC->GetLogicalFunction());
			}
			void SetLogicalFunction(GDILogicalFunction mode) const
			{
				m_DC->SetLogicalFunction(static_cast<wxRasterOperationMode>(mode));
			}

			// Graphics Context
			wxGraphicsContext* GetGraphicsContext() const
			{
				return m_DC->GetGraphicsContext();
			}
			void SetGraphicsContext(std::unique_ptr<wxGraphicsContext> graphicsContext)
			{
				m_DC->SetGraphicsContext(graphicsContext.release());
			}

			// Coordinate conversion functions
			template<class TDerived, class TPair = TDerived>
			TPair DeviceToLogical(const GDICoordPair<TDerived>& pair) const
			{
				return {m_DC->DeviceToLogicalX(pair.GetX()), m_DC->DeviceToLogicalY(pair.GetY())};
			}

			template<class TDerived, class TPair = TDerived>
			TPair DeviceToLogicalRelative(const GDICoordPair<TDerived>& pair) const
			{
				return {m_DC->DeviceToLogicalXRel(pair.GetX()), m_DC->DeviceToLogicalYRel(pair.GetY())};
			}

			Rect DeviceToLogical(const Rect& rect) const
			{
				return {DeviceToLogical(rect.GetPosition()), DeviceToLogicalRelative(rect.GetSize())};
			}

			template<class TDerived, class TPair = TDerived>
			TPair LogicalToDevice(const GDICoordPair<TDerived>& pair) const
			{
				return {m_DC->LogicalToDeviceX(pair.GetX()), m_DC->LogicalToDeviceY(pair.GetY())};
			}

			template<class TDerived, class TPair = TDerived>
			TPair LogicalToDeviceRelative(const GDICoordPair<TDerived>& pair) const
			{
				return {m_DC->LogicalToDeviceXRel(pair.GetX()), m_DC->LogicalToDeviceYRel(pair.GetY())};
			}

			Rect LogicalToDevice(const Rect& rect) const
			{
				return {LogicalToDevice(rect.GetPosition()), LogicalToDeviceRelative(rect.GetSize())};
			}

			// Drawing functions
			void Clear()
			{
				m_DC->Clear();
			}
			void DrawArc(const Point& start, const Point& end, const Point& center)
			{
				m_DC->DrawArc(start, end, center);
			}
			void DrawCircle(const Point& pos, int radius)
			{
				m_DC->DrawCircle(pos, radius);
			}
			void DrawEllipse(const Rect& rect)
			{
				m_DC->DrawEllipse(rect);
			}
			void DrawEllipse(const Point& pos, const Size& size)
			{
				m_DC->DrawEllipse(pos, size);
			}
			void DrawEllipticArc(const Rect& rect, Angle start, Angle end)
			{
				m_DC->DrawEllipticArc(rect.GetPosition(), rect.GetSize(), start.ToDegrees(), end.ToDegrees());
			}
			void DrawEllipticArc(const Point& pos, const Size& size, Angle start, Angle end)
			{
				m_DC->DrawEllipticArc(pos, size, start.ToDegrees(), end.ToDegrees());
			}
			void DrawLine(const Point& pos1, const Point& pos2)
			{
				m_DC->DrawLine(pos1, pos2);
			}

			void DrawPolyLine(const Point* points, size_t count, const Point& offset = {0, 0})
			{
				auto pointsBuffer = Private::ConvertWxPoints(points, count);
				m_DC->DrawLines(static_cast<int>(pointsBuffer.size()), pointsBuffer.data(), offset.GetX(), offset.GetY());
			}

			template<size_t N>
			void DrawPolyLine(const Point(&points)[N], const Point& offset = {0, 0})
			{
				auto pointsBuffer = Private::ConvertWxPoints(points);
				m_DC->DrawLines(static_cast<int>(pointsBuffer.size()), offset.GetX(), offset.GetY());
			}

			template<size_t N>
			void DrawPolyLine(const std::array<Point, N>& points, const Point& offset = {0, 0})
			{
				auto pointsBuffer = Private::ConvertWxPoints(points);
				m_DC->DrawLines(static_cast<int>(pointsBuffer.size()), offset.GetX(), offset.GetY());
			}

			void DrawSpline(const Point* points, size_t count)
			{
				auto pointsBuffer = Private::ConvertWxPoints(points, count);
				m_DC->DrawSpline(static_cast<int>(pointsBuffer.size()), pointsBuffer.data());
			}

			template<size_t N>
			void DrawSpline(const Point(&points)[N])
			{
				auto pointsBuffer = Private::ConvertWxPoints(points);
				m_DC->DrawSpline(static_cast<int>(pointsBuffer.size()), pointsBuffer.data());
			}

			template<size_t N>
			void DrawSpline(const std::array<Point, N>& points)
			{
				auto pointsBuffer = Private::ConvertWxPoints(points);
				m_DC->DrawSpline(static_cast<int>(pointsBuffer.size()), pointsBuffer.data());
			}

			void DrawPoint(const Point& pos)
			{
				m_DC->DrawPoint(pos);
			}

			void DrawRectangle(const Point& pos, const Size& size)
			{
				m_DC->DrawRectangle(pos, size);
			}
			void DrawRectangle(const Rect& rect)
			{
				m_DC->DrawRectangle(rect);
			}
			void DrawRoundedRectangle(const Point& pos, const Size& size, double radius)
			{
				m_DC->DrawRoundedRectangle(pos, size, radius);
			}
			void DrawRoundedRectangle(const Rect& rect, double radius)
			{
				m_DC->DrawRoundedRectangle(rect, radius);
			}

			void DrawGradientLinear(const Rect& rect, const Color& startColor, const Color& endColor, Direction direction)
			{
				m_DC->GradientFillLinear(rect, startColor, endColor, static_cast<wxDirection>(direction));
			}
			void DrawGradientConcentric(const Rect& rect, const Color& startColor, const Color& endColor)
			{
				m_DC->GradientFillConcentric(rect, startColor, endColor);
			}
			void DrawGradientConcentric(const Rect& rect, const Point& center, const Color& startColor, const Color& endColor)
			{
				m_DC->GradientFillConcentric(rect, startColor, endColor, center);
			}

			void DrawCrossHair(const Point& pos)
			{
				m_DC->CrossHair(pos);
			}
			void DrawCrossHair(const Point& pos, const Size& size)
			{
				// Horizontal
				DrawLine({pos.GetX() - size.GetWidth(), pos.GetY()}, {pos.GetX() + size.GetWidth(), pos.GetY()});

				// Vertical
				DrawLine({pos.GetX(), pos.GetY() + size.GetHeight()}, {pos.GetX(), pos.GetY() - size.GetHeight()});
			}
			void DrawCrossHair(const Rect& rect)
			{
				DrawCrossHair(rect.GetPosition(), rect.GetSize());
			}
			void DrawCheckMark(const Rect& rect)
			{
				m_DC->DrawCheckMark(rect);
			}

			void DrawText(const String& text, const Point& pos)
			{
				m_DC->DrawText(text, pos);
			}
			void DrawRotatedText(const String& text, const Point& pos, Angle angle)
			{
				m_DC->DrawRotatedText(text, pos, angle.ToDegrees());
			}
			Rect DrawLabel(const String& text, const Rect& rect, const GDIBitmap& bitmap, FlagSet<Alignment> alignment = Alignment::Left|Alignment::Top, size_t acceleratorIndex = String::npos)
			{
				wxRect boundingBox;
				m_DC->DrawLabel(text, bitmap.ToWxBitmap(), rect, *Private::MapAlignment(alignment), acceleratorIndex != String::npos ? static_cast<int>(acceleratorIndex) : -1, &boundingBox);

				return Rect(boundingBox);
			}
			Rect DrawLabel(const String& text, const Rect& rect, FlagSet<Alignment> alignment = Alignment::Left|Alignment::Top, size_t acceleratorIndex = String::npos)
			{
				wxRect boundingBox;
				m_DC->DrawLabel(text, wxNullBitmap, rect, *Private::MapAlignment(alignment), acceleratorIndex != String::npos ? static_cast<int>(acceleratorIndex) : -1, &boundingBox);

				return Rect(boundingBox);
			}

			bool CanDrawBitmap() const
			{
				return m_DC->CanDrawBitmap();
			}
			void DrawIcon(const GDIIcon& icon, const Point& pos)
			{
				m_DC->DrawIcon(icon.ToWxIcon(), pos);
			}
			void DrawBitmap(const GDIBitmap& bitmap, const Point& pos)
			{
				m_DC->DrawBitmap(bitmap.ToWxBitmap(), pos, false);
			}

			void FloodFill(const Point& pos, const Color& color, FloodFill fillMode)
			{
				if (auto modeWx = Drawing::Private::MapFloodFill(fillMode))
				{
					m_DC->FloodFill(pos, color, *modeWx);
				}
			}

			// Clipping region functions
			void ClipRegion(const GDIRegion& region)
			{
				m_DC->SetDeviceClippingRegion(region.ToWxRegion());
			}
			void ClipBoxRegion(const Rect& rect)
			{
				m_DC->SetClippingRegion(rect);
			}
			void ResetClipRegion()
			{
				m_DC->DestroyClippingRegion();
			}
			Rect GetClipBox() const
			{
				wxRect rect;
				if (m_DC->GetClippingBox(rect))
				{
					return Rect(rect);
				}
				return {};
			}

			// Text/character extent functions
			bool CanGetTextExtent() const
			{
				return m_DC->CanGetTextExtent();
			}

			Size GetCharSize() const
			{
				return {m_DC->GetCharWidth(), m_DC->GetCharHeight()};
			}
			int GetCharWidth() const
			{
				return m_DC->GetCharWidth();
			}
			int GetCharHeight() const
			{
				return m_DC->GetCharHeight();
			}
			FontMetrics GetFontMetrics() const
			{
				return Private::FromWxFontMetrics(m_DC->GetFontMetrics());
			}

			Size GetTextExtent(const String& text) const
			{
				return Size(m_DC->GetTextExtent(text));
			}
			Size GetTextExtent(const String& text, const GDIFont& font) const
			{
				wxSize size;
				m_DC->GetTextExtent(text, &size.x, &size.y, nullptr, nullptr, &font.ToWxFont());

				return Size(size);
			}
			Size GetMultiLineTextExtent(const String& text) const
			{
				return Size(m_DC->GetMultiLineTextExtent(text));
			}
			Size GetMultiLineTextExtent(const String& text, const GDIFont& font) const
			{
				wxSize size;
				m_DC->GetMultiLineTextExtent(text, &size.x, &size.y, nullptr, &font.ToWxFont());

				return Size(size);
			}
			std::vector<int> GetPartialTextExtents(const String& text) const
			{
				wxArrayInt widths;
				if (m_DC->GetPartialTextExtents(text, widths))
				{
					return widths;
				}
				return {};
			}

			// Text properties functions
			bool IsBackgroundTransparent() const
			{
				return m_DC->GetBackgroundMode() == wxBRUSHSTYLE_TRANSPARENT;
			}
			void SetBackgroundTransparent(bool isTransparent = true)
			{
				m_DC->SetBackgroundMode(isTransparent ? wxBRUSHSTYLE_TRANSPARENT : wxBRUSHSTYLE_SOLID);
			}

			LayoutDirection GetLayoutDirection() const
			{
				switch (m_DC->GetLayoutDirection())
				{
					case wxLayout_LeftToRight:
					{
						return LayoutDirection::LeftToRight;
					}
					case wxLayout_RightToLeft:
					{
						return LayoutDirection::RightToLeft;
					}
				};
				return LayoutDirection::Default;
			}
			void SetLayoutDirection(LayoutDirection layoutDirection)
			{
				switch (layoutDirection)
				{
					case LayoutDirection::LeftToRight:
					{
						m_DC->SetLayoutDirection(wxLayout_LeftToRight);
						break;
					}
					case LayoutDirection::RightToLeft:
					{
						m_DC->SetLayoutDirection(wxLayout_RightToLeft);
						break;
					}
					default:
					{
						m_DC->SetLayoutDirection(wxLayout_Default);
						break;
					}
				};
			}

			GDIFont GetFont() const
			{
				return m_DC->GetFont();
			}
			void SetFont(const GDIFont& font)
			{
				m_DC->SetFont(font.ToWxFont());
			}

			Color GetTextForeground() const
			{
				return m_DC->GetTextForeground();
			}
			void SetTextForeground(const Color& color)
			{
				m_DC->SetTextForeground(color);
			}

			Color GetTextBackground() const
			{
				return m_DC->GetTextBackground();
			}
			void SetTextBackground(const Color& color)
			{
				m_DC->SetTextBackground(color);
			}

			// Bounding box functions
			Rect GetBoundingBox() const;
			void CalcBoundingBox(const Point& point)
			{
				m_DC->CalcBoundingBox(point.GetX(), point.GetY());
			}
			void CalcBoundingBox(const RectF& rect)
			{
				if (!rect.IsEmpty())
				{
					CalcBoundingBox(rect.GetTopLeft());
					CalcBoundingBox(rect.GetRightBottom());
				}
			}
			void ResetBoundingBox()
			{
				m_DC->ResetBoundingBox();
			}

			// Page and document start/end functions
			bool StartDocument(const String& message)
			{
				return m_DC->StartDoc(message);
			}
			void EndDocument()
			{
				m_DC->EndDoc();
			}

			void StartPage()
			{
				m_DC->StartPage();
			}
			void EndPage()
			{
				m_DC->EndPage();
			}

			// Bit-Block Transfer operations (blit)
			bool Blit(const Point& destination, const Size& size, GDIContext& sourceContext, const Point& source, GDILogicalFunction mode = GDILogicalFunction::Copy)
			{
				if (sourceContext)
				{
					return m_DC->Blit(destination, size, sourceContext.m_DC, source, static_cast<wxRasterOperationMode>(mode), false, wxDefaultPosition);
				}
				return false;
			}
			bool StretchBlit(const Rect& destination, const Size& size, GDIContext& sourceContext, const Rect& source, GDILogicalFunction mode = GDILogicalFunction::Copy)
			{
				if (sourceContext)
				{
					return m_DC->StretchBlit(destination.GetPosition(), destination.GetSize(), sourceContext.m_DC, source.GetPosition(), source.GetSize(), static_cast<wxRasterOperationMode>(mode), false, wxDefaultPosition);
				}
				return false;
			}

			// Background/foreground brush and pen
			GDIBrush GetBackgroundBrush() const
			{
				return m_DC->GetBackground();
			}
			void SetBackgroundBrush(const GDIBrush& brush)
			{
				m_DC->SetBackground(brush.ToWxBrush());
			}

			GDIBrush GetBrush() const
			{
				return m_DC->GetBrush();
			}
			void SetBrush(const GDIBrush& brush)
			{
				m_DC->SetBrush(brush.ToWxBrush());
			}

			GDIPen GetPen() const
			{
				return m_DC->GetPen();
			}
			void SetPen(const GDIPen& pen)
			{
				m_DC->SetPen(pen.ToWxPen());
			}

			// Transformation matrix
			bool CanUseTransformMatrix() const
			{
				return m_DC->CanUseTransformMatrix();
			}
			AffineMatrix GetTransformMatrix() const
			{
				return Private::FromWxAffineMatrix(m_DC->GetTransformMatrix());
			}
			bool SetTransformMatrix(const AffineMatrix& transform)
			{
				return m_DC->SetTransformMatrix(Private::ToWxAffineMatrix(transform));
			}
			void ResetTransformMatrix()
			{
				m_DC->ResetTransformMatrix();
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			GDIContext& operator=(const GDIContext&) = delete;
			GDIContext& operator=(GDIContext&& other) noexcept
			{
				m_DC = other.m_DC;
				other.m_DC = nullptr;

				return *this;
			}
	};
}
