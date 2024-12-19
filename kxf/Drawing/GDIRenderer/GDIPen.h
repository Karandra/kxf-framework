#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include "kxf/wxWidgets/MapDrawing.h"
#include <wx/brush.h>

namespace kxf
{
	class GDIBitmap;
}

namespace kxf
{
	class KXF_API GDIPen: public IGDIObject
	{
		public:
			using Dash = wxDash;

		private:
			wxPen m_Pen;

		private:
			void Initialize();

		public:
			GDIPen()
			{
				Initialize();
			}
			GDIPen(const wxPen& other)
				:m_Pen(other)
			{
			}
			GDIPen(const GDIPen& other)
				:m_Pen(other.m_Pen)
			{
			}
			GDIPen(const Color& color, int width = 1)
				:m_Pen(color, width, wxPENSTYLE_SOLID)
			{
				Initialize();
			}
			GDIPen(const wxColour& color, int width = 1)
				:m_Pen(color, width, wxPENSTYLE_SOLID)
			{
				Initialize();
			}
			GDIPen(const GDIBitmap& stippleBitmap, int width = 1);
			virtual ~GDIPen()
			{
				m_Pen.SetDashes(0, nullptr);
			}

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Pen.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				return this == &other || GetHandle() == other.GetHandle();
			}
			std::shared_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_shared<GDIPen>(m_Pen);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// GDIPen
			const wxPen& ToWxPen() const noexcept
			{
				return m_Pen;
			}
			wxPen& ToWxPen() noexcept
			{
				return m_Pen;
			}

			Color GetColor() const
			{
				return m_Pen.GetColour();
			}
			void SetColor(const Color& color)
			{
				m_Pen.SetColour(color);
			}

			bool IsSolid() const
			{
				return m_Pen.GetStyle() == wxPENSTYLE_SOLID;
			}
			void SetSolid()
			{
				m_Pen.SetStyle(wxPENSTYLE_SOLID);
			}

			bool IsTransparent() const
			{
				return m_Pen.IsTransparent();
			}
			void SetTransparent()
			{
				m_Pen.SetStyle(wxPENSTYLE_TRANSPARENT);
			}

			bool IsHatch() const
			{
				const auto style = ToInt(m_Pen.GetStyle());
				return style >= wxPENSTYLE_FIRST_HATCH && style <= wxPENSTYLE_LAST_HATCH;
			}
			HatchStyle GetHatchStyle() const
			{
				if (IsHatch())
				{
					return wxWidgets::MapHatchStyle(static_cast<wxHatchStyle>(m_Pen.GetStyle()));
				}
				return HatchStyle::None;
			}
			void SetHatchStyle(HatchStyle style)
			{
				m_Pen.SetStyle(static_cast<wxPenStyle>(wxWidgets::MapHatchStyle(style)));
			}

			LineJoin GetJoin() const
			{
				return wxWidgets::MapLineJoin(m_Pen.GetJoin());
			}
			void SetJoin(LineJoin join)
			{
				m_Pen.SetJoin(wxWidgets::MapLineJoin(join));
			}

			LineCap GetCap() const
			{
				return wxWidgets::MapLineCap(m_Pen.GetCap());
			}
			void SetCap(LineCap cap)
			{
				m_Pen.SetCap(wxWidgets::MapLineCap(cap));
			}

			bool IsStipple() const
			{
				return m_Pen.GetStyle() == wxPENSTYLE_STIPPLE;
			}
			GDIBitmap GetStipple() const;
			void SetStipple(const GDIBitmap& stipple);

			int GetWidth() const
			{
				return m_Pen.GetWidth();
			}
			void SetWidth(int width)
			{
				m_Pen.SetWidth(width);
			}

			bool IsDash() const
			{
				switch (m_Pen.GetStyle())
				{
					case wxPENSTYLE_DOT:
					case wxPENSTYLE_LONG_DASH:
					case wxPENSTYLE_SHORT_DASH:
					case wxPENSTYLE_DOT_DASH:
					case wxPENSTYLE_USER_DASH:
					{
						return true;
					}
				};
				return false;
			}
			DashStyle GetDashStyle() const
			{
				if (IsDash())
				{
					return wxWidgets::MapDashStyle(static_cast<wxDeprecatedGUIConstants>(m_Pen.GetStyle()));
				}
				return DashStyle::None;
			}
			void SetDashStyle(DashStyle style)
			{
				if (auto wxStyle = wxWidgets::MapDashStyle(style))
				{
					m_Pen.SetStyle(static_cast<wxPenStyle>(*wxStyle));
				}
			}

			size_t GetDashCount() const
			{
				return m_Pen.GetDashCount();
			}
			size_t GetDashes(Dash*& dashes) const
			{
				return m_Pen.GetDashes(&dashes);
			}
			std::vector<Dash> GetDashes() const
			{
				Dash* dashes = nullptr;
				int count = m_Pen.GetDashes(&dashes);
				if (dashes && count != 0)
				{
					return {dashes, dashes + static_cast<size_t>(count)};
				}
				return {};
			}
			void SetDashes(const Dash* dashes, size_t count)
			{
				m_Pen.SetDashes(static_cast<int>(count), dashes);
			}

			template<size_t N>
			void SetDashes(const Dash(&dashes)[N])
			{
				m_Pen.SetDashes(static_cast<int>(N), dashes);
			}

			template<size_t N>
			void SetDashes(const std::array<Dash, N>& dashes)
			{
				m_Pen.SetDashes(static_cast<int>(dashes.size()), dashes.data());
			}

			void SetDashes(const std::vector<Dash>& dashes)
			{
				SetDashes(dashes.data(), dashes.size());
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

			GDIPen& operator=(const GDIPen& other)
			{
				m_Pen = other.m_Pen;

				return *this;
			}
	};
}

namespace kxf::Drawing
{
	KXF_API GDIPen GetStockGDIPen(StockPen pen);
}
