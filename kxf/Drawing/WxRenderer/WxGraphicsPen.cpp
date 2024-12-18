#include "kxf-pch.h"
#include "WxGraphicsPen.h"
#include "../BitmapImage.h"
#include "../GraphicsRenderer/IGraphicsBrush.h"
#include "../GraphicsRenderer/IGraphicsTexture.h"

namespace kxf
{
	void WxGraphicsPen::Initialize()
	{
		if (!m_Initialized)
		{
			if (m_Pen)
			{
				wxGraphicsPenInfo penInfo;
				penInfo.Colour(m_Pen.GetColor());
				penInfo.Width(m_PenWidth);
				penInfo.Join(m_Pen.ToWxPen().GetJoin());
				penInfo.Cap(m_Pen.ToWxPen().GetCap());
				penInfo.Style(m_Pen.ToWxPen().GetStyle());
				if (m_Pen.IsStipple())
				{
					penInfo.Stipple(m_Pen.GetStipple().ToWxBitmap());
				}

				GDIPen::Dash* dashes = nullptr;
				size_t dashCount = m_Pen.GetDashes(dashes);
				penInfo.Dashes(dashCount, dashes);

				m_Graphics = m_Renderer->Get().CreatePen(penInfo);
			}
			else
			{
				m_Graphics = {};
			}
			m_Initialized = true;
		}
	}
	void WxGraphicsPen::AssignBrushData()
	{
		if (m_Brush)
		{
			if (auto textureBrush = m_Brush->QueryInterface<IGraphicsTextureBrush>())
			{
				if (auto texture = textureBrush->GetTexture())
				{
					m_Pen.SetStipple(texture->ToBitmapImage().ToGDIBitmap());
				}
			}
			else if (auto hatchBrush = m_Brush->QueryInterface<IGraphicsHatchBrush>())
			{
				m_Pen.SetHatchStyle(hatchBrush->GetHatchStyle());
			}
		}
		else
		{
			m_Pen.SetStipple({});
			m_Pen.SetTransparent();
		}
	}

	PenStyle WxGraphicsPen::GetStyle() const
	{
		if (m_Pen.IsSolid())
		{
			return PenStyle::Solid;
		}
		else if (m_Pen.IsHatch())
		{
			return PenStyle::Hatch;
		}
		else if (m_Pen.IsDash())
		{
			return PenStyle::Dash;
		}
		else if (m_Pen.GetStipple())
		{
			return PenStyle::Texture;
		}
		return PenStyle::None;
	}
	void WxGraphicsPen::SetStyle(PenStyle style)
	{
		switch (style)
		{
			case PenStyle::Solid:
			{
				m_Pen.SetSolid();
				break;
			}
			case PenStyle::Hatch:
			{
				m_Pen.SetHatchStyle(HatchStyle::Horizontal);
				break;
			}
			case PenStyle::Dash:
			{
				m_Pen.SetDashStyle(DashStyle::Dash);
				break;
			}
			case PenStyle::Texture:
			{
				m_Pen.ToWxPen().SetStyle(wxPENSTYLE_STIPPLE);
				break;
			}
			default:
			{
				m_Pen.SetTransparent();
				break;
			}
		};
		Invalidate();
	}
}
