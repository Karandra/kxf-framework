#include "kxf-pch.h"
#include "WxGraphicsFont.h"
#include "WxGraphicsContext.h"

namespace kxf
{
	FontMetricsF WxGraphicsFont::GetMetrics() const
	{
		if (auto context = m_Renderer->CreateMeasuringContext())
		{
			return context->GetFontMetrics(*this);
		}
		else
		{
			return SizeF(m_Font.GetPixelSize());
		}
	}
}
