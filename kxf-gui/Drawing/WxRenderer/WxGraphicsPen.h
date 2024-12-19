#pragma once
#include "Common.h"
#include "WxGraphicsRenderer.h"
#include "../GDIRenderer/GDIPen.h"
#include "../GraphicsRenderer/IGraphicsPen.h"
#include <wx/graphics.h>

namespace kxf
{
	class IGraphicsBrush;
}

namespace kxf
{
	class KXF_API WxGraphicsPen: public RTTI::ExtendInterface<WxGraphicsPen, IGraphicsPen>
	{
		kxf_RTTI_DeclareIID(WxGraphicsPen, {0x5b2d709c, 0x741b, 0x4f1c, {0x90, 0xd2, 0x2c, 0x61, 0x49, 0x31, 0x9, 0xf4}});

		protected:
			WxGraphicsRenderer* m_Renderer = nullptr;
			wxGraphicsPen m_Graphics;
			GDIPen m_Pen;
			float m_PenWidth = -1.0f;
			std::shared_ptr<IGraphicsBrush> m_Brush;

			bool m_Initialized = false;

		private:
			void Initialize();
			void Invalidate()
			{
				m_Initialized = false;
			}
			void AssignBrushData();

		public:
			WxGraphicsPen() noexcept = default;
			WxGraphicsPen(WxGraphicsRenderer& rendrer, const GDIPen& pen)
				:m_Renderer(&rendrer), m_Pen(pen), m_PenWidth(pen.GetWidth())
			{
			}
			WxGraphicsPen(WxGraphicsRenderer& rendrer, const Color& color, float width)
				:m_Renderer(&rendrer), m_Pen(color, static_cast<int>(width)), m_PenWidth(width)
			{
			}

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_Pen.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<WxGraphicsPen>())
				{
					return m_Pen.IsSameAs(object->m_Pen);
				}
				return false;
			}
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<WxGraphicsPen>(*this);
			}

			WxGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Graphics.GetGraphicsData();
			}

			// IGraphicsPen
			bool IsTransparent() const override
			{
				return m_Pen.IsTransparent();
			}

			Color GetColor() const override
			{
				return m_Pen.GetColor();
			}
			void SetColor(const Color& color) override
			{
				m_Pen.SetColor(color);
				Invalidate();
			}

			PenStyle GetStyle() const override;
			void SetStyle(PenStyle style) override;

			DashStyle GetDashStyle() const
			{
				return m_Pen.GetDashStyle();
			}
			void SetDashStyle(DashStyle style)
			{
				m_Pen.SetDashStyle(style);
				Invalidate();
			}

			LineJoin GetLineJoin() const override
			{
				return m_Pen.GetJoin();
			}
			void SetLineJoin(LineJoin join) override
			{
				m_Pen.SetJoin(join);
				Invalidate();
			}

			LineCap GetLineCap() const override
			{
				return m_Pen.GetCap();
			}
			void SetLineCap(LineCap cap) override
			{
				m_Pen.SetCap(cap);
				Invalidate();
			}

			LineAlignment GetLineAlignment() const override
			{
				return LineAlignment::Center;
			}
			void SetLineAlignment(LineAlignment alignment) override
			{
			}

			float GetWidth() const override
			{
				return m_PenWidth;
			}
			void SetWidth(float width) override
			{
				m_PenWidth = width;
				m_Pen.SetWidth(static_cast<int>(width));
				Invalidate();
			}

			std::shared_ptr<IGraphicsBrush> GetBrush() const override
			{
				return m_Brush;
			}
			void SetBrush(std::shared_ptr<IGraphicsBrush> brush) override
			{
				m_Brush = std::move(brush);
				AssignBrushData();
				Invalidate();
			}

			size_t GetDashPattern(float* dashes, size_t maxCount) const override
			{
				GDIPen::Dash* gdiDashes = nullptr;
				const size_t gdiCount = m_Pen.GetDashes(gdiDashes);

				const size_t count = std::min(gdiCount, maxCount);
				std::copy_n(gdiDashes, count, dashes);
				return count;
			}
			void SetDashPattern(const float* dashes, size_t count) override
			{
				std::vector<GDIPen::Dash> gdiDashes;
				gdiDashes.resize(count);
				std::copy_n(dashes, count, gdiDashes.begin());

				m_Pen.SetDashes(gdiDashes.data(), gdiDashes.size());
				Invalidate();
			}

			// WxGraphicsPen
			const wxGraphicsPen& Get() const
			{
				const_cast<WxGraphicsPen&>(*this).Initialize();
				return m_Graphics;
			}
			wxGraphicsPen& Get()
			{
				Initialize();
				return m_Graphics;
			}

			const GDIPen& GetPen() const
			{
				return m_Pen;
			}
			GDIPen& GetPen()
			{
				return m_Pen;
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
