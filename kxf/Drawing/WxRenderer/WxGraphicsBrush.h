#pragma once
#include "Common.h"
#include "WxGraphicsRenderer.h"
#include "WxGraphicsTexture.h"
#include "../GDIRenderer/GDIBrush.h"
#include "../GDIRenderer/GDIBitmap.h"
#include "../GraphicsRenderer/IGraphicsBrush.h"
#include <wx/graphics.h>

namespace kxf
{
	class KXF_API WxGraphicsBrush: public RTTI::Interface<WxGraphicsBrush>
	{
		kxf_RTTI_DeclareIID(IGraphicsBrush, {0xe86d8ab8, 0xeb26, 0x448c, {0x9b, 0x7d, 0xa7, 0xe2, 0x4e, 0x92, 0x4, 0x2b}});

		public:
			WxGraphicsBrush() noexcept = default;
			virtual ~WxGraphicsBrush() = default;

		public:
			virtual wxGraphicsBrush& Get() = 0;
			virtual const wxGraphicsBrush& Get() const = 0;

			virtual const GDIBrush& GetBrush() const = 0;
			virtual GDIBrush& GetBrush() = 0;
	};
}

namespace kxf::Drawing::Private
{
	template<class TBase>
	class KXF_API WxGraphicsBrushBase: public RTTI::Implementation<WxGraphicsBrushBase<TBase>, TBase, WxGraphicsBrush>
	{
		protected:
			WxGraphicsRenderer* m_Renderer = nullptr;
			wxGraphicsBrush m_Graphics;
			GDIBrush m_Brush;

			bool m_Initialized = false;

		private:
			void Initialize()
			{
				if (!m_Initialized)
				{
					if (m_Brush)
					{
						m_Graphics = m_Renderer->Get().CreateBrush(m_Brush.ToWxBrush());
					}
					else
					{
						m_Graphics = {};
					}
					m_Initialized = true;
				}
			}

		protected:
			void Invalidate()
			{
				m_Initialized = false;
			}

		public:
			WxGraphicsBrushBase() = default;
			WxGraphicsBrushBase(WxGraphicsRenderer& rendrer)
				:m_Renderer(&rendrer)
			{
			}
			WxGraphicsBrushBase(WxGraphicsRenderer& rendrer, const GDIBrush& brush)
				:m_Renderer(&rendrer), m_Brush(brush)
			{
			}

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_Brush.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<WxGraphicsBrush>())
				{
					return m_Brush.IsSameAs(object->GetBrush());
				}
				return false;
			}

			WxGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Graphics.GetGraphicsData();
			}

			// IGraphicsBrush
			bool IsTransparent() const override
			{
				return m_Brush.IsTransparent();
			}

			Color GetColor() const override
			{
				return m_Brush.GetColor();
			}
			void SetColor(const Color& color) override
			{
				m_Brush.SetColor(color);
				Invalidate();
			}

			// WxGraphicsBrush
			const wxGraphicsBrush& Get() const override
			{
				const_cast<WxGraphicsBrushBase<TBase>&>(*this).Initialize();
				return m_Graphics;
			}
			wxGraphicsBrush& Get() override
			{
				Initialize();
				return m_Graphics;
			}

			const GDIBrush& GetBrush() const override
			{
				return m_Brush;
			}
			GDIBrush& GetBrush() override
			{
				return m_Brush;
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
	class KXF_API WxGraphicsSolidBrush: public Drawing::Private::WxGraphicsBrushBase<IGraphicsSolidBrush>
	{
		public:
			WxGraphicsSolidBrush() = default;
			WxGraphicsSolidBrush(WxGraphicsRenderer& rendrer, const GDIBrush& brush)
				:WxGraphicsBrushBase(rendrer, brush)
			{
			}
			WxGraphicsSolidBrush(WxGraphicsRenderer& rendrer, const Color& color)
				:WxGraphicsBrushBase(rendrer)
			{
				m_Brush.SetSolid();
				m_Brush.SetColor(color);
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<WxGraphicsSolidBrush>(*this);
			}
	};

	class KXF_API WxGraphicsHatchBrush: public Drawing::Private::WxGraphicsBrushBase<IGraphicsHatchBrush>
	{
		private:
			Color m_BackgroundColor;

		public:
			WxGraphicsHatchBrush() = default;
			WxGraphicsHatchBrush(WxGraphicsRenderer& rendrer, const GDIBrush& brush)
				:WxGraphicsBrushBase(rendrer, brush)
			{
			}
			WxGraphicsHatchBrush(WxGraphicsRenderer& rendrer, HatchStyle style)
				:WxGraphicsBrushBase(rendrer)
			{
				m_Brush.SetHatchStyle(style);
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<WxGraphicsHatchBrush>(*this);
			}

			// IGraphicsHatchBrush
			Color GetBackgroundColor() const override
			{
				return m_BackgroundColor;
			}
			void SetBackgroundColor(const Color& color) override
			{
				m_BackgroundColor = color;
			}

			Color GetForegroundColor() const override
			{
				return m_Brush.GetColor();
			}
			void SetForegroundColor(const Color& color) override
			{
				m_Brush.SetColor(color);
				Invalidate();
			}

			HatchStyle GetHatchStyle() const override
			{
				return m_Brush.GetHatchStyle();
			}
			void SetHatchStyle(HatchStyle style) override
			{
				m_Brush.SetHatchStyle(style);
				Invalidate();
			}
	};

	class KXF_API WxGraphicsTextureBrush: public Drawing::Private::WxGraphicsBrushBase<IGraphicsTextureBrush>
	{
		private:
			WrapMode m_WrapMode = WrapMode::None;

		public:
			WxGraphicsTextureBrush() = default;
			WxGraphicsTextureBrush(WxGraphicsRenderer& rendrer, const GDIBrush& brush, WrapMode wrapMode = WrapMode::None)
				:WxGraphicsBrushBase(rendrer, brush), m_WrapMode(wrapMode)
			{
			}
			WxGraphicsTextureBrush(WxGraphicsRenderer& rendrer, const GDIBitmap& bitmap, WrapMode wrapMode = WrapMode::None)
				:WxGraphicsBrushBase(rendrer), m_WrapMode(wrapMode)
			{
				m_Brush.SetStipple(bitmap);
			}
			WxGraphicsTextureBrush(WxGraphicsRenderer& rendrer, const BitmapImage& image, WrapMode wrapMode = WrapMode::None)
				:WxGraphicsTextureBrush(rendrer, image.ToGDIBitmap(), wrapMode)
			{
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<WxGraphicsTextureBrush>(*this);
			}

			// IGraphicsTextureBrush
			std::shared_ptr<IGraphicsTexture> GetTexture() const override
			{
				return std::make_shared<WxGraphicsTexture>(*m_Renderer, m_Brush.GetStipple());
			}
			void SetTexture(std::shared_ptr<IGraphicsTexture> texture) override
			{
				if (texture && *texture)
				{
					m_Brush.SetStipple(texture->QueryInterface<WxGraphicsTexture>()->GetImage().ToGDIBitmap());
				}
				else
				{
					m_Brush.SetStipple({});
				}
				Invalidate();
			}

			WrapMode GetWrapMode() const override
			{
				return m_WrapMode;
			}
			void SetWrapMode(WrapMode wrapMode) override
			{
				m_WrapMode = wrapMode;
			}
	};

	class KXF_API WxGraphicsLinearGradientBrush: public Drawing::Private::WxGraphicsBrushBase<IGraphicsLinearGradientBrush>
	{
		private:
			RectF m_Rect;
			GradientStops m_GradentStops;
			AffineMatrixF m_Transform;
			WrapMode m_WrapMode = WrapMode::None;

		public:
			WxGraphicsLinearGradientBrush() = default;
			WxGraphicsLinearGradientBrush(WxGraphicsRenderer& rendrer, const GDIBrush& brush)
				:WxGraphicsBrushBase(rendrer, brush)
			{
			}
			WxGraphicsLinearGradientBrush(WxGraphicsRenderer& rendrer, const RectF& rect, const GradientStops& colors, AffineMatrixF matrix = {}, WrapMode wrapMode = WrapMode::None)
				:WxGraphicsBrushBase(rendrer), m_Rect(rect), m_GradentStops(colors), m_Transform(std::move(matrix)), m_WrapMode(wrapMode)
			{
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<WxGraphicsLinearGradientBrush>(*this);
			}

			// IGraphicsLinearGradientBrush
			RectF GetRect() const override
			{
				return m_Rect;
			}
			void SetRect(const RectF& rect) override
			{
				m_Rect = rect;
			}

			GradientStops GetLinearColors() const
			{
				return m_GradentStops;
			}
			void SetLinearColors(const GradientStops& colors) override
			{
				m_GradentStops = colors;
			}

			AffineMatrixF GetTransform() const override
			{
				return m_Transform;
			}
			void SetTransform(const AffineMatrixF& transform) override
			{
				m_Transform = transform;
			}

			WrapMode GetWrapMode() const override
			{
				return m_WrapMode;
			}
			void SetWrapMode(WrapMode wrapMode) override
			{
				m_WrapMode = wrapMode;
			}
	};

	class KXF_API WxGraphicsRadialGradientBrush: public Drawing::Private::WxGraphicsBrushBase<IGraphicsRadialGradientBrush>
	{
		private:
			RectF m_Rect;
			GradientStops m_GradentStops;
			AffineMatrixF m_Transform;
			WrapMode m_WrapMode = WrapMode::None;

		public:
			WxGraphicsRadialGradientBrush() = default;
			WxGraphicsRadialGradientBrush(WxGraphicsRenderer& rendrer, const GDIBrush& brush)
				:WxGraphicsBrushBase(rendrer, brush)
			{
			}
			WxGraphicsRadialGradientBrush(WxGraphicsRenderer& rendrer, const RectF& rect, const GradientStops& colors, AffineMatrixF matrix = {}, WrapMode wrapMode = WrapMode::None)
				:WxGraphicsBrushBase(rendrer), m_Rect(rect), m_GradentStops(colors), m_Transform(std::move(matrix)), m_WrapMode(wrapMode)
			{
			}

		public:
			// IGraphicsObject
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<WxGraphicsRadialGradientBrush>(*this);
			}

			// IGraphicsRadialGradientBrush
			PointF GetCenterPoint() const override
			{
				return m_Rect.GetCenter();
			}
			void SetCenterPoint(const PointF& point) override
			{
				m_Rect.MoveCenterTo(point);
			}

			RectF GetRect() const override
			{
				return m_Rect;
			}
			void SetRect(const RectF& rect) override
			{
				m_Rect = rect;
			}

			GradientStops GetLinearColors() const
			{
				return m_GradentStops;
			}
			void SetLinearColors(const GradientStops& colors) override
			{
				m_GradentStops = colors;
			}

			AffineMatrixF GetTransform() const override
			{
				return m_Transform;
			}
			void SetTransform(const AffineMatrixF& transform) override
			{
				m_Transform = transform;
			}

			WrapMode GetWrapMode() const override
			{
				return m_WrapMode;
			}
			void SetWrapMode(WrapMode wrapMode) override
			{
				m_WrapMode = wrapMode;
			}
	};
}
