#include "kxf-pch.h"
#include "GDIGraphicsFont.h"
#include "GDIGraphicsContext.h"

namespace kxf
{
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
}
