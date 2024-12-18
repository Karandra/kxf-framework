#pragma once
#include "Common.h"
#include "WxGraphicsRenderer.h"
#include "kxf/Core/OptionalPtr.h"
#include "../GDIRenderer/GDIContext.h"
#include "../GDIRenderer/GDIFont.h"
#include "../GraphicsRenderer/IGraphicsContext.h"

namespace kxf
{
	class KXF_API WxGraphicsContext: public RTTI::ExtendInterface<WxGraphicsContext, IGraphicsContext>
	{
		kxf_RTTI_DeclareIID(WxGraphicsContext, {0x941a0455, 0x2c28, 0x4545, {0xb1, 0xfa, 0xa5, 0xa5, 0xbf, 0x56, 0x45, 0x69}});

		protected:
			class ChangeTextParameters final
			{
				private:
					wxGraphicsContext& m_Context;
					wxGraphicsFont m_OldFont;
					std::optional<wxAntialiasMode> m_OldAntialiasMode;
					std::optional<wxInterpolationQuality> m_OldInterpolationQuality;

				public:
					ChangeTextParameters(WxGraphicsContext& gc, const IGraphicsFont& font, const IGraphicsBrush& brush);
					ChangeTextParameters(WxGraphicsContext& gc, const IGraphicsFont& font, const IGraphicsBrush& brush, wxAntialiasMode antialiasMode)
						:ChangeTextParameters(gc, font, brush)
					{
						m_OldAntialiasMode = m_Context.GetAntialiasMode();
						m_Context.SetAntialiasMode(antialiasMode);
					}
					ChangeTextParameters(WxGraphicsContext& gc, const IGraphicsFont& font, const IGraphicsBrush& brush, wxInterpolationQuality interpolationQuality)
						:ChangeTextParameters(gc, font, brush)
					{
						m_OldInterpolationQuality = m_Context.GetInterpolationQuality();
						m_Context.SetInterpolationQuality(interpolationQuality);
					}
					ChangeTextParameters(WxGraphicsContext& gc, const IGraphicsFont& font, const IGraphicsBrush& brush, wxAntialiasMode antialiasMode, wxInterpolationQuality interpolationQuality)
						:ChangeTextParameters(gc, font, brush)
					{
						m_OldAntialiasMode = m_Context.GetAntialiasMode();
						m_OldInterpolationQuality = m_Context.GetInterpolationQuality();

						m_Context.SetAntialiasMode(antialiasMode);
						m_Context.SetInterpolationQuality(interpolationQuality);
					}
					~ChangeTextParameters();
			};
			class ChangeDrawParameters final
			{
				private:
					wxGraphicsContext& m_Context;
					wxGraphicsBrush m_OldBrush;
					wxGraphicsPen m_OldPen;

				public:
					ChangeDrawParameters(WxGraphicsContext& gc, const IGraphicsBrush& brush, const IGraphicsPen& pen);
					~ChangeDrawParameters();
			};

		protected:
			// Context
			WxGraphicsRenderer* m_Renderer = nullptr;
			wxGraphicsContext* m_Context = nullptr;
			wxGCDC m_WxGCDC;
			GDIContext m_GCDC;

			// Buffer to draw on (for derived classes)
			std::shared_ptr<IGraphicsTexture> m_BufferTexture;

			// Currently active pen/brush/font
			std::shared_ptr<IGraphicsPen> m_CurrentPen;
			std::shared_ptr<IGraphicsBrush> m_CurrentBrush;

			GDIFont m_CurrentFont;
			Color m_CurrentFontColor;
			bool m_CurrentFontValid = false;

			// Bounding box
			std::optional<RectF> m_BoundingBox;

			// Options
			InterpolationQuality m_InterpolationQuality = InterpolationQuality::Default;
			CompositionMode m_CompositionMode = CompositionMode::Over;
			AntialiasMode m_AntialiasMode = AntialiasMode::Default;
			wxAntialiasMode m_TextAntialiasMode = wxAntialiasMode::wxANTIALIAS_DEFAULT;

		private:
			void CommonInit();

		protected:
			void SetupGC(wxWindow* window = nullptr);
			void CopyAttributesFromDC(const GDIContext& dc);
			void Initialize(WxGraphicsRenderer& rendrer, std::unique_ptr<wxGraphicsContext> gc);

			BitmapImage& InitTextureBuffer(std::shared_ptr<IGraphicsTexture> texture);

			wxGraphicsFont MakeGCFont() const;
			wxGraphicsFont MakeGCFont(const IGraphicsFont& font, const Color& color = {}) const;
			wxGraphicsFont MakeGCFont(const IGraphicsFont& font, const IGraphicsBrush& brush) const;
			void UpdateCurrentFont();
			void InvalidateCurrentFont()
			{
				m_CurrentFontValid = false;
			}

		public:
			WxGraphicsContext()
				:m_WxGCDC(static_cast<wxGraphicsContext*>(nullptr))
			{
			}
			WxGraphicsContext(WxGraphicsRenderer& rendrer, std::unique_ptr<wxGraphicsContext> gc)
				:m_Renderer(&rendrer), m_Context(gc.get()), m_WxGCDC(gc.release()), m_GCDC(m_WxGCDC)
			{
				CommonInit();
			}

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_Context == nullptr;
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<WxGraphicsContext>())
				{
					return m_Context == object->m_Context;
				}
				return false;
			}

			WxGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Context->GetGraphicsData();
			}

			// IGraphicsContext
		public:
			// Feature support
			FlagSet<GraphicsContextFeature> GetSupportedFeatures() const override;

			// Clipping region functions
			void ClipBoxRegion(const RectF& rect) override;
			void ResetClipRegion();
			RectF GetClipBox() const override;

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

			// Path functions
			void StrokePath(const IGraphicsPath& path) override;
			void FillPath(const IGraphicsPath& path, PolygonFill fill = PolygonFill::OddEvenRule) override;
			void DrawPath(const IGraphicsPath& path, PolygonFill fill = PolygonFill::OddEvenRule) override;

			// Texture functions
			void DrawTexture(const IGraphicsTexture& texture, const RectF& rect) override;
			void DrawTexture(const BitmapImage& image, const RectF& rect) override;
			void DrawTexture(const IImage2D& image, const RectF& rect) override;

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
			SizeF GetSize() const override;
			SizeF GetDPI() const override;
			wxWindow* GetWindow() const override;

			AntialiasMode GetAntialiasMode() const override;
			bool SetAntialiasMode(AntialiasMode mode) override;

			AntialiasMode GetTextAntialiasMode() const override;
			bool SetTextAntialiasMode(AntialiasMode mode) override;

			CompositionMode GetCompositionMode() const override;
			bool SetCompositionMode(CompositionMode mode) override;

			InterpolationQuality GetInterpolationQuality() const override;
			bool SetInterpolationQuality(InterpolationQuality quality) override;

			// Bounding box functions
			RectF GetBoundingBox() const override;
			using IGraphicsContext::CalcBoundingBox;
			void CalcBoundingBox(const PointF& point) override;
			void ResetBoundingBox() override;

			// Page and document start/end functions
			bool StartDocument(const String& message) override
			{
				return m_Context->StartDoc(message);
			}
			void EndDocument() override
			{
				m_Context->EndDoc();
			}

			void StartPage() override
			{
				m_Context->StartPage();
			}
			void EndPage() override
			{
				m_Context->EndPage();
			}

			// Modifying the state
			void BeginLayer(float opacity = 1.0f) override
			{
				m_Context->BeginLayer(opacity);
			}
			void EndLayer() override
			{
				m_Context->EndLayer();
			}

			void PushState() override
			{
				m_Context->PushState();
			}
			void PopState() override
			{
				m_Context->PopState();
			}

			void Flush() override
			{
				m_Context->Flush();
			}

			// Offset management
			bool ShouldOffset() const override
			{
				return m_Context->ShouldOffset();
			}
			void EnableOffset(bool enable = true) override
			{
				m_Context->EnableOffset(enable);
			}

		public:
			// WxGraphicsContext
			const wxGraphicsContext& Get() const
			{
				return *m_Context;
			}
			wxGraphicsContext& Get()
			{
				return *m_Context;
			}

			const wxGCDC& GetGCDC() const
			{
				return m_WxGCDC;
			}
			wxGCDC& GetGCDC()
			{
				return m_WxGCDC;
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
	class KXF_API WxGraphicsGDIContext: public WxGraphicsContext
	{
		private:
			GDIContext m_DC;
			BitmapImage* m_Image = nullptr;

		protected:
			void Initialize(WxGraphicsRenderer& rendrer, wxDC& dc, const Size& size);

			bool FlushContent();
			void ResetContext();

		public:
			WxGraphicsGDIContext() noexcept = default;
			WxGraphicsGDIContext(WxGraphicsRenderer& rendrer, wxDC& dc, const Size& size)
			{
				Initialize(rendrer, dc, size);
			}
			~WxGraphicsGDIContext()
			{
				FlushContent();
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return nullptr;
			}

			// IGraphicsContext
			void Flush() override
			{
				if (!FlushContent())
				{
					WxGraphicsContext::Flush();
				}
			}
	};

	class KXF_API WxGraphicsMemoryContext: public WxGraphicsContext
	{
		private:
			BitmapImage* m_Image = nullptr;

		private:
			bool FlushContent()
			{
				if (m_Context)
				{
					m_Context->Flush();
					return true;
				}
				return false;
			}

		public:
			WxGraphicsMemoryContext() noexcept = default;
			WxGraphicsMemoryContext(WxGraphicsRenderer& rendrer, std::shared_ptr<IGraphicsTexture> texture, wxWindow* window = nullptr)
			{
				m_Image = &InitTextureBuffer(std::move(texture));
				Initialize(rendrer, std::unique_ptr<wxGraphicsContext>(rendrer.Get().CreateContextFromImage(m_Image->ToWxImage())));
				SetupGC(window);
			}
			~WxGraphicsMemoryContext()
			{
				FlushContent();
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return nullptr;
			}

			// IGraphicsContext
			void Flush() override
			{
				if (!FlushContent())
				{
					WxGraphicsContext::Flush();
				}
			}

			// WxGraphicsMemoryContext
			std::shared_ptr<IGraphicsTexture> GetSelectedTexture() const
			{
				return m_BufferTexture;
			}
			void SelectTexture(std::shared_ptr<IGraphicsTexture> texture)
			{
				InitTextureBuffer(std::move(texture));
			}
			std::shared_ptr<IGraphicsTexture> UnselectTexture()
			{
				FlushContent();
				return std::move(m_BufferTexture);
			}
	};

	class KXF_API WxGraphicsMeasuringContext: public WxGraphicsContext
	{
		private:
			wxWindow* m_Window = nullptr;

		public:
			WxGraphicsMeasuringContext() noexcept = default;
			WxGraphicsMeasuringContext(WxGraphicsRenderer& rendrer, wxWindow* window = nullptr)
				:WxGraphicsContext(rendrer, std::unique_ptr<wxGraphicsContext>(rendrer.Get().CreateMeasuringContext())), m_Window(window)
			{
				SetupGC(window);
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<WxGraphicsMeasuringContext>(*m_Renderer, m_Window);
			}

			// IGraphicsContext
			FlagSet<GraphicsContextFeature> GetSupportedFeatures() const override
			{
				return GraphicsContextFeature::TextMeasurement;
			}
	};

	namespace Drawing::Private
	{
		template<class T>
		class WxGraphicsBasicGDIContext: public WxGraphicsContext
		{
			private:
				T m_DC;

			private:
				void FlushContent()
				{
					if (m_Context)
					{
						m_Context->Flush();
					}
				}

			public:
				WxGraphicsBasicGDIContext() noexcept = default;
				WxGraphicsBasicGDIContext(WxGraphicsRenderer& rendrer, wxWindow& window)
					:m_DC(&window)
				{
					Initialize(rendrer, std::unique_ptr<wxGraphicsContext>(rendrer.Get().CreateContext(m_DC)));

					WxGraphicsContext::CopyAttributesFromDC(m_DC);
					WxGraphicsContext::SetupGC();
				}
				~WxGraphicsBasicGDIContext()
				{
					FlushContent();
				}

			public:
				// IGraphicsObject
				std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
				{
					return std::make_shared<WxGraphicsBasicGDIContext<T>>(*m_Renderer, *m_Context->GetWindow());
				}
		};

		template<class T>
		class KXF_API WxGraphicsBasicGDIContext_ImageBuffered: public WxGraphicsGDIContext
		{
			private:
				T m_DC;

			public:
				WxGraphicsBasicGDIContext_ImageBuffered() noexcept = default;
				WxGraphicsBasicGDIContext_ImageBuffered(WxGraphicsRenderer& rendrer, wxWindow& window)
					:m_DC(&window)
				{
					Initialize(rendrer, m_DC, window.GetClientRect());

					WxGraphicsContext::CopyAttributesFromDC(m_DC);
					WxGraphicsContext::SetupGC();
				}
				~WxGraphicsBasicGDIContext_ImageBuffered()
				{
					FlushContent();
					ResetContext();
				}

			public:
				// IGraphicsObject
				std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
				{
					return std::make_shared<WxGraphicsBasicGDIContext_ImageBuffered>(*m_Renderer, *m_DC.GetWindow());
				}
		};
	}

	using WxGraphicsWindowContext = Drawing::Private::WxGraphicsBasicGDIContext<wxWindowDC>;
	using WxGraphicsWindowClientContext = Drawing::Private::WxGraphicsBasicGDIContext<wxClientDC>;
	using WxGraphicsPaintContext = Drawing::Private::WxGraphicsBasicGDIContext<wxPaintDC>;
	using WxGraphicsBufferedPaintContext = Drawing::Private::WxGraphicsBasicGDIContext<wxBufferedPaintDC>;
}
