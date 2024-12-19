#include "kxf-pch.h"
#include "GDIGraphicsBrush.h"
#include "GDIGraphicsContext.h"
#include "kxf/Drawing/BitmapImage.h"

namespace kxf
{
	GDIGraphicsTextureBrush::GDIGraphicsTextureBrush(GDIGraphicsRenderer& rendrer, const BitmapImage& image, WrapMode wrapMode)
		:GDIGraphicsBrushBase(rendrer), m_WrapMode(wrapMode)
	{
		m_Brush.SetStipple(image.ToWXBitmap());
	}
}
