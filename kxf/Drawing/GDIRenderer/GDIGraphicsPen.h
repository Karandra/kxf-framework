#pragma once
#include "Common.h"
#include "GDIPen.h"
#include "GDIGraphicsRenderer.h"
#include "IGDIObject.h"
#include "../GraphicsRenderer/IGraphicsPen.h"

namespace kxf
{
	class IGraphicsBrush;
}

namespace kxf
{
	class KXF_API GDIGraphicsPen: public RTTI::ExtendInterface<GDIGraphicsPen, IGraphicsPen, IGDIObject>
	{
		kxf_RTTI_DeclareIID(GDIGraphicsPen, {0xc5cfc8bc, 0x1f1f, 0x4c5e, {0xbb, 0x51, 0x8, 0xb6, 0x9d, 0xc2, 0xa0, 0x7c}});

		protected:
			GDIGraphicsRenderer* m_Renderer = nullptr;
			GDIPen m_Pen;
			std::shared_ptr<IGraphicsBrush> m_Brush;

		private:
			bool DoIsSameAs(const IObject& other) const
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDIGraphicsPen>())
				{
					return m_Pen.IsSameAs(object->m_Pen);
				}
				return false;
			}
			void AssignStippleFromBrush();

		public:
			GDIGraphicsPen() noexcept = default;
			GDIGraphicsPen(GDIGraphicsRenderer& rendrer, const GDIPen& pen)
				:m_Renderer(&rendrer), m_Pen(pen)
			{
			}
			GDIGraphicsPen(GDIGraphicsRenderer& rendrer, const Color& color, float width)
				:m_Renderer(&rendrer), m_Pen(color)
			{
				GDIGraphicsPen::SetWidth(width);
			}

		public:
			// IGDIObject
			bool IsSameAs(const IGDIObject& other) const override
			{
				return DoIsSameAs(other);
			}
			std::shared_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_shared<GDIGraphicsPen>(*this);
			}

			void* GetHandle() const override
			{
				return m_Pen.GetHandle();
			}
			void* DetachHandle() override
			{
				return m_Pen.DetachHandle();
			}
			void AttachHandle(void* handle) override
			{
				m_Pen.AttachHandle(handle);
			}

			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_Pen.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return DoIsSameAs(other);
			}
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<GDIGraphicsPen>(*this);
			}

			GDIGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Pen.GetHandle();
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
			}

			LineJoin GetLineJoin() const override
			{
				return m_Pen.GetJoin();
			}
			void SetLineJoin(LineJoin join) override
			{
				m_Pen.SetJoin(join);
			}

			LineCap GetLineCap() const override
			{
				return m_Pen.GetCap();
			}
			void SetLineCap(LineCap cap) override
			{
				m_Pen.SetCap(cap);
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
				return m_Pen.GetWidth();
			}
			void SetWidth(float width) override
			{
				m_Pen.SetWidth(static_cast<int>(width));
			}

			std::shared_ptr<IGraphicsBrush> GetBrush() const override
			{
				return m_Brush;
			}
			void SetBrush(std::shared_ptr<IGraphicsBrush> brush) override
			{
				m_Brush = std::move(brush);
				if (m_Brush)
				{
					AssignStippleFromBrush();
				}
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
			}

			// GDIGraphicsPen
			const GDIPen& Get() const
			{
				return m_Pen;
			}
			GDIPen& Get()
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
