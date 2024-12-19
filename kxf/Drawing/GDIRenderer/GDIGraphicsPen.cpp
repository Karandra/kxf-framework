#include "kxf-pch.h"
#include "GDIGraphicsPen.h"
#include "GDIBitmap.h"
#include "../BitmapImage.h"
#include "../GraphicsRenderer/IGraphicsBrush.h"
#include "../GraphicsRenderer/IGraphicsTexture.h"

namespace kxf
{
	void GDIGraphicsPen::AssignStippleFromBrush()
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

	PenStyle GDIGraphicsPen::GetStyle() const
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
	void GDIGraphicsPen::SetStyle(PenStyle style)
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
	}
}
