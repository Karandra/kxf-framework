#include "kxf-pch.h"
#include "Common.h"
#include "NullObjects/NullGraphicsRenderer.h"

namespace kxf::Drawing::Private
{
	IGraphicsRenderer& GetNullGraphicsRenderer()
	{
		static kxf::Drawing::NullGraphicsRenderer instance;
		return instance;
	}
}
