#include "kxf-pch.h"
#include "PaintEvent.h"
#include "../GraphicsRenderer/GraphicsContext.h"

namespace kxf::Sciter
{
	GraphicsContext PaintEvent::GetGraphicsContext() const
	{
		return m_GraphicsContext;
	}
	void PaintEvent::SetGraphicsContext(GraphicsContextHandle* handle)
	{
		m_GraphicsContext = handle;
	}
}
