#pragma once
#include "kxf/Drawing/Common.h"
#include "kxf/Drawing/Font.h"
#include "kxf/Drawing/Angle.h"
#include "kxf/Drawing/Color.h"
#include "kxf/Drawing/ColorDepth.h"
#include "kxf/Drawing/FontMetrics.h"
#include "kxf/Drawing/AffineMatrix.h"
#include "kxf/Drawing/ImageDefines.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/RTTI/RTTI.h"
#include "../GradientStops.h"

namespace kxf
{
	class IGraphicsRenderer;
}

namespace kxf::Drawing::Private
{
	KXF_API IGraphicsRenderer& GetNullGraphicsRenderer();
}
