#include "kxf-pch.h"
#include "WxGraphicsPath.h"
#include "WxGraphicsContext.h"
#include "kxf/wxWidgets/MapDrawing.h"

namespace kxf
{
	// IGraphicsPath
	bool WxGraphicsPath::ContainsPoint(const PointF& point, PolygonFillMode fill) const
	{
		if (auto fillMode = wxWidgets::MapPolygonFillMode(fill))
		{
			return m_Graphics.Contains(point, *fillMode);
		}
		return false;
	}
}
