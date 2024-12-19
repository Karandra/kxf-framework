#include "kxf-pch.h"
#include "GDIGraphicsFont.h"
#include "GDIGraphicsContext.h"
#include "kxf/Drawing/Font.h"

namespace kxf
{
	// IGraphicsFont
	FontMetricsF GDIGraphicsFont::GetMetrics() const
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

	Font GDIGraphicsFont::ToFont() const
	{
		return m_Font.ToFont();
	}
	bool GDIGraphicsFont::FromFont(const Font& font)
	{
		m_Font = font;
		return !m_Font.IsNull();
	}
}
