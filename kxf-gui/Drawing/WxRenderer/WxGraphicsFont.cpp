#include "kxf-pch.h"
#include "WxGraphicsFont.h"
#include "WxGraphicsContext.h"
#include "kxf/Drawing/Font.h"

namespace kxf
{
	// IGraphicsFont
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

	Font WxGraphicsFont::ToFont() const
	{
		return m_Font.ToFont();
	}
	bool WxGraphicsFont::FromFont(const Font& font)
	{
		m_Font = font;
		Invalidate();

		return !m_Font.IsNull();
	}
}
