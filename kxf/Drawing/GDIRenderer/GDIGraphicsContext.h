#pragma once
#include "Common.h"
#include "GDIContext.h"
#include "GDIGraphicsRenderer.h"
#include "IGDIObject.h"
#include "../GraphicsRenderer/IGraphicsContext.h"
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/dcscreen.h>
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>

namespace kxf
{
	class KXF_API GDIGraphicsContext: public RTTI::ExtendInterface<GDIGraphicsContext, IGraphicsContext, IGDIObject>
	{
		kxf_RTTI_DeclareIID(GDIGraphicsContext, {0x7f0f1843, 0x27b9, 0x40df, {0x8d, 0xdc, 0x27, 0x98, 0x52, 0x44, 0xb7, 0x20}});

		protected:
			GDIGraphicsRenderer* m_Renderer = nullptr;
			GDIContext m_DC;

			std::shared_ptr<IGraphicsPen> m_SavedPen;
			std::shared_ptr<IGraphicsBrush> m_SavedBrush;

			AntialiasMode m_AntialiasMode = AntialiasMode::None;
			AntialiasMode m_TextAntialiasMode = AntialiasMode::Default;
			CompositionMode m_CompositionMode = CompositionMode::Over;
			InterpolationQuality m_InterpolationQuality = InterpolationQuality::Default;

		private:
			bool DoIsSameAs(const IObject& other) const
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDIGraphicsContext>())
				{
					return m_DC.IsSameAs(object->m_DC);
				}
				return false;
			}
			void MakeNull()
			{
				m_DC = {};
				m_Renderer = nullptr;
			}

		protected:
			void SetupDC(wxWindow* window = nullptr, const Size& size = Size::UnspecifiedSize());

		public:
			GDIGraphicsContext() noexcept = default;
			GDIGraphicsContext(GDIGraphicsRenderer& rendrer, wxDC& dc)
				:m_Renderer(&rendrer), m_DC(dc)
			{
			}

		public:
			// IGDIObject
			bool IsSameAs(const IGDIObject& other) const override
			{
				return DoIsSameAs(other);
			}
			std::shared_ptr<IGDIObject> CloneGDIObject() const override
			{
				return nullptr;
			}

			void* GetHandle() const override
			{
				return m_DC.GetHandle();
			}
			void* DetachHandle() override
			{
				return m_DC.DetachHandle();
			}
			void AttachHandle(void* handle) override
			{
				m_DC.AttachHandle(handle);
			}

			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_DC.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return DoIsSameAs(other);
			}

			GDIGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_DC.GetHandle();
			}

			// IGraphicsContext
		public:
			// Feature support
			FlagSet<GraphicsContextFeature> GetSupportedFeatures() const override;

			// Clipping region functions
			void ClipBoxRegion(const RectF& rect) override
			{
				m_DC.ClipBoxRegion(rect);
			}
			void ResetClipRegion()
			{
				m_DC.ResetClipRegion();
			}
			RectF GetClipBox() const override
			{
				return m_DC.GetClipBox();
			}

			// Transformation matrix
			AffineMatrixF GetTransform() const override;
			bool SetTransform(const AffineMatrixF& transform) override;

			bool TransformInvert() override;
			void TransformRotate(Angle angle) override;
			void TransformScale(float xScale, float yScale) override;
			void TransformTranslate(float dx, float dy) override;
			void TransformConcat(const AffineMatrixF& matrix) override;

			// Pen and brush functions
			std::shared_ptr<IGraphicsPen> GetPen() const override;
			void SetPen(std::shared_ptr<IGraphicsPen> pen) override;

			std::shared_ptr<IGraphicsBrush> GetBrush() const override;
			void SetBrush(std::shared_ptr<IGraphicsBrush> brush) override;

			// Path functions (not implemented)
			void StrokePath(const IGraphicsPath& path) override
			{
			}
			void FillPath(const IGraphicsPath& path, PolygonFillMode fill = PolygonFillMode::OddEvenRule) override
			{
			}
			void DrawPath(const IGraphicsPath& path, PolygonFillMode fill = PolygonFillMode::OddEvenRule) override
			{
			}

			// Texture functions
			void DrawTexture(const IGraphicsTexture& texture, const RectF& rect) override;
			void DrawTexture(const BitmapImage& image, const RectF& rect) override;
			void DrawTexture(const IImage2D& image, const RectF& rect) override;
			void DrawTexture(const GDIBitmap& bitmap, const RectF& rect);

			// Text functions
			std::shared_ptr<IGraphicsFont> GetFont() const override;
			void SetFont(std::shared_ptr<IGraphicsFont> font) override;

			std::shared_ptr<IGraphicsBrush> GetFontBrush() const override;
			void SetFontBrush(std::shared_ptr<IGraphicsBrush> brush) override;

			SizeF GetTextExtent(const String& text, const IGraphicsFont& font = NullGraphicsFont) const override;
			FontMetricsF GetFontMetrics(const IGraphicsFont& font = NullGraphicsFont) const override;
			std::vector<float> GetPartialTextExtents(const String& text, const IGraphicsFont& font = NullGraphicsFont) const override;

			void DrawText(const String& text, const PointF& point, const IGraphicsFont& font = NullGraphicsFont, const IGraphicsBrush& brush = NullGraphicsBrush) override;
			void DrawRotatedText(const String& text, const PointF& point, Angle angle, const IGraphicsFont& font = NullGraphicsFont, const IGraphicsBrush& brush = NullGraphicsBrush) override;
			RectF DrawLabel(const String& text,
							const RectF& rect,
							const IGraphicsTexture& icon,
							const IGraphicsFont& font,
							const IGraphicsBrush& brush,
							FlagSet<Alignment> alignment = {},
							size_t acceleratorIndex = String::npos) override;

			String EllipsizeText(const String& text, float maxWidth, EllipsizeMode mode, FlagSet<EllipsizeFlag> flags = {}, const IGraphicsFont& font = NullGraphicsFont) const override;

			// Drawing functions
			void Clear(const IGraphicsBrush& brush) override;
			void DrawCircle(const PointF& pos, float radius, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen) override;
			void DrawEllipse(const RectF& rect, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen) override;
			void DrawRectangle(const RectF& rect, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen) override;
			void DrawRoundedRectangle(const RectF& rect, float radius, const IGraphicsBrush& brush = NullGraphicsBrush, const IGraphicsPen& pen = NullGraphicsPen) override;
			void DrawLine(const PointF& point1, const PointF& point2, const IGraphicsPen& pen = NullGraphicsPen) override;
			void DrawPolyLine(const PointF* points, size_t count, const IGraphicsPen& pen = NullGraphicsPen) override;
			void DrawDisconnectedLines(const PointF* startPoints, const PointF* endPoints, size_t count, const IGraphicsPen& pen = NullGraphicsPen) override;

			void DrawGDI(const RectF& rect, std::function<void(GDIContext& dc)> func) override;

			// Getting and setting parameters
			SizeF GetSize() const override
			{
				return m_DC.GetSize();
			}
			SizeF GetDPI() const override
			{
				return m_DC.GetDPI();
			}
			wxWindow* GetWindow() const override
			{
				return m_DC.GetWindow();
			}

			AntialiasMode GetAntialiasMode() const override
			{
				return m_AntialiasMode;
			}
			bool SetAntialiasMode(AntialiasMode mode) override
			{
				m_AntialiasMode = mode;
				return false;
			}

			AntialiasMode GetTextAntialiasMode() const override
			{
				return m_TextAntialiasMode;
			}
			bool SetTextAntialiasMode(AntialiasMode mode) override
			{
				m_TextAntialiasMode = mode;
				return false;
			}

			CompositionMode GetCompositionMode() const override;
			bool SetCompositionMode(CompositionMode mode) override;

			InterpolationQuality GetInterpolationQuality() const override
			{
				return m_InterpolationQuality;
			}
			bool SetInterpolationQuality(InterpolationQuality quality) override
			{
				m_InterpolationQuality = quality;
				return true;
			}

			// Bounding box functions
			RectF GetBoundingBox() const override;
			void CalcBoundingBox(const PointF& point) override;
			void ResetBoundingBox() override;

			// Page and document start/end functions
			bool StartDocument(const String& message) override
			{
				return m_DC.StartDocument(message);
			}
			void EndDocument() override
			{
				m_DC.EndDocument();
			}

			void StartPage() override
			{
				m_DC.StartPage();
			}
			void EndPage() override
			{
				m_DC.EndPage();
			}

			// Modifying the state
			void BeginLayer(float opacity = 1.0f) override
			{
			}
			void EndLayer() override
			{
			}

			void PushState() override
			{
			}
			void PopState() override
			{
			}

			void Flush() override
			{
			}

			// Offset management
			bool ShouldOffset() const override
			{
				return false;
			}
			void EnableOffset(bool enable = true) override
			{
			}

			void OffsetForScrollableArea(const PointF& scrollPos, const PointF& scrollInc, const PointF& scale = {}, const PointF& origin = {}) override;
			using IGraphicsContext::OffsetForScrollableArea;

		public:
			// GDIGraphicsContext
			const GDIContext& Get() const
			{
				return m_DC;
			}
			GDIContext& Get()
			{
				return m_DC;
			}

			const wxDC& GetWx() const
			{
				return m_DC.ToWxDC();
			}
			wxDC& GetWx()
			{
				return m_DC.ToWxDC();
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

namespace kxf
{
	class KXF_API GDIGraphicsAnyContext: public GDIGraphicsContext
	{
		public:
			GDIGraphicsAnyContext() noexcept = default;
			GDIGraphicsAnyContext(GDIGraphicsRenderer& rendrer, wxDC& dc, const Size& size)
				:GDIGraphicsContext(rendrer, dc)
			{
				SetupDC(nullptr, size);
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return nullptr;
			}
	};

	class KXF_API GDIGraphicsMemoryContext: public GDIGraphicsContext
	{
		protected:
			wxMemoryDC m_MemoryDC;
			std::shared_ptr<IGraphicsTexture> m_Texture;

		public:
			GDIGraphicsMemoryContext() noexcept = default;
			GDIGraphicsMemoryContext(GDIGraphicsRenderer& rendrer, std::shared_ptr<IGraphicsTexture> texture, wxWindow* window = nullptr)
				:GDIGraphicsContext(rendrer, m_MemoryDC)
			{
				SetupDC(window);
				SelectTexture(std::move(texture));
			}
			~GDIGraphicsMemoryContext()
			{
				static_cast<void>(UnselectTexture());
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				if (m_Texture)
				{
					auto copy = m_Texture->CloneGraphicsObject();
					return std::make_shared<GDIGraphicsMemoryContext>(*m_Renderer, copy->QueryInterface<IGraphicsTexture>());
				}
				else
				{
					return std::make_shared<GDIGraphicsMemoryContext>(*m_Renderer, nullptr);
				}
			}

			// GDIGraphicsMemoryContext
			std::shared_ptr<IGraphicsTexture> GetSelectedTexture() const;
			void SelectTexture(std::shared_ptr<IGraphicsTexture> texture);
			std::shared_ptr<IGraphicsTexture> UnselectTexture();
	};

	class KXF_API GDIGraphicsWindowContext: public GDIGraphicsContext
	{
		protected:
			wxWindowDC m_WindowDC;

		public:
			GDIGraphicsWindowContext() noexcept = default;
			GDIGraphicsWindowContext(GDIGraphicsRenderer& rendrer, wxWindow& window)
				:GDIGraphicsContext(rendrer, m_WindowDC), m_WindowDC(&window)
			{
				SetupDC();
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<GDIGraphicsWindowContext>(*m_Renderer, *m_WindowDC.GetWindow());
			}
	};

	class KXF_API GDIGraphicsWindowClientContext: public GDIGraphicsContext
	{
		protected:
			wxClientDC m_ClientDC;

		public:
			GDIGraphicsWindowClientContext() noexcept = default;
			GDIGraphicsWindowClientContext(GDIGraphicsRenderer& rendrer, wxWindow& window)
				:GDIGraphicsContext(rendrer, m_ClientDC), m_ClientDC(&window)
			{
				SetupDC();
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<GDIGraphicsWindowClientContext>(*m_Renderer, *m_ClientDC.GetWindow());
			}
	};

	class KXF_API GDIGraphicsPaintContext: public GDIGraphicsContext
	{
		protected:
			wxPaintDC m_PaintDC;

		public:
			GDIGraphicsPaintContext() noexcept = default;
			GDIGraphicsPaintContext(GDIGraphicsRenderer& rendrer, wxWindow& window)
				:GDIGraphicsContext(rendrer, m_PaintDC), m_PaintDC(&window)
			{
				SetupDC();
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<GDIGraphicsPaintContext>(*m_Renderer, *m_PaintDC.GetWindow());
			}
	};
}

namespace kxf
{
	enum GDIBufferedContextFlag: uint32_t
	{
		None = 0,

		BufferVirtualArea = wxBUFFER_VIRTUAL_AREA,
		BufferClientArea = wxBUFFER_CLIENT_AREA,
		VirtualArea = wxBUFFER_USES_SHARED_BUFFER,
	};
	kxf_FlagSet_Declare(GDIBufferedContextFlag);

	class KXF_API GDIGraphicsBufferedContext: public GDIGraphicsContext
	{
		protected:
			wxBufferedDC m_BufferedDC;
			std::shared_ptr<IGraphicsTexture> m_Texture;

		public:
			GDIGraphicsBufferedContext() noexcept = default;
			GDIGraphicsBufferedContext(GDIGraphicsRenderer& rendrer, const SizeF& size, FlagSet<GDIBufferedContextFlag> flags = GDIBufferedContextFlag::BufferClientArea);
			GDIGraphicsBufferedContext(GDIGraphicsRenderer& rendrer, std::shared_ptr<IGraphicsTexture> texture, FlagSet<GDIBufferedContextFlag> flags = GDIBufferedContextFlag::BufferClientArea);

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				if (m_Texture)
				{
					auto copy = m_Texture->CloneGraphicsObject();
					return std::make_shared<GDIGraphicsBufferedContext>(*m_Renderer, copy->QueryInterface<IGraphicsTexture>(), GetFlags());
				}
				else
				{
					return std::make_shared<GDIGraphicsBufferedContext>(*m_Renderer, Size(m_BufferedDC.GetSize()), GetFlags());
				}
			}

			// GDIGraphicsBufferedContext
			void UnMask()
			{
				m_BufferedDC.UnMask();
			}
			FlagSet<GDIBufferedContextFlag> GetFlags() const
			{
				return static_cast<GDIBufferedContextFlag>(m_BufferedDC.GetStyle());
			}
			void SetFlags(FlagSet<GDIBufferedContextFlag> flags)
			{
				m_BufferedDC.SetStyle(flags.ToInt<int>());
			}
	};

	class KXF_API GDIGraphicsBufferedPaintContext: public GDIGraphicsContext
	{
		protected:
			wxBufferedPaintDC m_BufferedPaintDC;

		public:
			GDIGraphicsBufferedPaintContext() noexcept = default;
			GDIGraphicsBufferedPaintContext(GDIGraphicsRenderer& rendrer, wxWindow& window, FlagSet<GDIBufferedContextFlag> flags = GDIBufferedContextFlag::BufferClientArea)
				:GDIGraphicsContext(rendrer, m_BufferedPaintDC), m_BufferedPaintDC(&window, flags.ToInt())
			{
				SetupDC();
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<GDIGraphicsBufferedPaintContext>(*m_Renderer, *m_BufferedPaintDC.GetWindow(), GetFlags());
			}

			// GDIGraphicsBufferedContext
			void UnMask()
			{
				m_BufferedPaintDC.UnMask();
			}
			FlagSet<GDIBufferedContextFlag> GetFlags() const
			{
				return static_cast<GDIBufferedContextFlag>(m_BufferedPaintDC.GetStyle());
			}
			void SetFlags(FlagSet<GDIBufferedContextFlag> flags)
			{
				m_BufferedPaintDC.SetStyle(flags.ToInt<int>());
			}
	};
}

