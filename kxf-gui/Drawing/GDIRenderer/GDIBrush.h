#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include <wx/brush.h>

namespace kxf
{
	class GDIBitmap;
}

namespace kxf
{
	class KXF_API GDIBrush: public IGDIObject
	{
		private:
			wxBrush m_Brush;

		public:
			GDIBrush() = default;
			GDIBrush(const wxBrush& other)
				:m_Brush(other)
			{
			}
			GDIBrush(const wxColour& color)
				:m_Brush(color, wxBRUSHSTYLE_SOLID)
			{
			}

			GDIBrush(const Color& color)
				:m_Brush(color, wxBRUSHSTYLE_SOLID)
			{
			}
			GDIBrush(const GDIBrush& other)
				:m_Brush(other.m_Brush)
			{
			}
			GDIBrush(const GDIBitmap& stippleBitmap);

			~GDIBrush() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Brush.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				return this == &other || GetHandle() == other.GetHandle();
			}
			std::shared_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_shared<GDIBrush>(m_Brush);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// GDIBrush
			const wxBrush& ToWxBrush() const noexcept
			{
				return m_Brush;
			}
			wxBrush& ToWxBrush() noexcept
			{
				return m_Brush;
			}

			Color GetColor() const
			{
				return m_Brush.GetColour();
			}
			void SetColor(const Color& color)
			{
				m_Brush.SetColour(color);
			}

			bool IsTransparent() const
			{
				return m_Brush.IsTransparent();
			}
			void SetTransparent()
			{
				m_Brush.SetStyle(wxBRUSHSTYLE_TRANSPARENT);
			}

			bool IsSolid() const
			{
				return m_Brush.GetStyle() == wxBRUSHSTYLE_SOLID;
			}
			void SetSolid()
			{
				m_Brush.SetStyle(wxBRUSHSTYLE_SOLID);
			}

			GDIBitmap GetStipple() const;
			void SetStipple(const GDIBitmap& stipple);

			bool IsHatch() const
			{
				return m_Brush.IsHatch();
			}
			HatchStyle GetHatchStyle() const;
			void SetHatchStyle(HatchStyle style);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			GDIBrush& operator=(const GDIBrush& other)
			{
				m_Brush = other.m_Brush;

				return *this;
			}
	};
}

namespace kxf::Drawing
{
	KXF_API GDIBrush GetStockGDIBrush(StockBrush brush);
}
