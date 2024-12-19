#include "kxf-pch.h"
#include "WxGraphicsBrush.h"
#include "WxGraphicsContext.h"
#include "kxf/Drawing/BitmapImage.h"
#include "kxf/wxWidgets/MapDrawing.h"

namespace kxf
{
	WxGraphicsTextureBrush::WxGraphicsTextureBrush(WxGraphicsRenderer& rendrer, const BitmapImage& image, WrapMode wrapMode)
		:WxGraphicsBrushBase(rendrer), m_WrapMode(wrapMode)
	{
		m_Brush.SetStipple(image.ToWXBitmap());
	}

	// IGraphicsTextureBrush
	std::shared_ptr<IGraphicsTexture> WxGraphicsTextureBrush::GetTexture() const
	{
		return std::make_shared<WxGraphicsTexture>(*m_Renderer, m_Brush.GetStipple().AsWXBitmap());
	}
	void WxGraphicsTextureBrush::SetTexture(std::shared_ptr<IGraphicsTexture> texture)
	{
		if (texture && *texture)
		{
			m_Brush.SetStipple(texture->QueryInterface<WxGraphicsTexture>()->GetImage().ToWXBitmap());
		}
		else
		{
			m_Brush.SetStipple({});
		}
		Invalidate();
	}
}
