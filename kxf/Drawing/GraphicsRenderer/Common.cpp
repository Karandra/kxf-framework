#include "kxf-pch.h"
#include "Common.h"
#include "NullObjects/NullGraphicsRenderer.h"

namespace
{
	kxf::Drawing::NullGraphicsRenderer g_NullGraphicsRenderer;
}

namespace kxf::Drawing::Private
{
	IGraphicsRenderer& GetNullGraphicsRenderer()
	{
		return g_NullGraphicsRenderer;
	}
}
