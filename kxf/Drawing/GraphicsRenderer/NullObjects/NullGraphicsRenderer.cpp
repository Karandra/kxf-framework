#include "kxf-pch.h"
#include "NullGraphicsRenderer.h"
#include "NullGraphicsPen.h"
#include "NullGraphicsBrush.h"

namespace kxf::Drawing
{
	// Pen and brush functions
	const IGraphicsPen& NullGraphicsRenderer::GetTransparentPen() const
	{
		return kxf::NullGraphicsPen;
	}
	const IGraphicsBrush& NullGraphicsRenderer::GetTransparentBrush() const
	{
		return kxf::NullGraphicsBrush;
	}
}
