#include "kxf-pch.h"
#include "IScrollableWidget.h"
#include "kxf/Drawing/GraphicsRenderer/IGraphicsContext.h"

namespace kxf
{
	void IScrollableWidget::AdjustGraphicsContext(IGraphicsContext& gc)
	{
		const auto rate = GetScrollRate();
		const auto scale = GetScrollScale();
		const auto start = GetViewStart();

		auto ApplyTransform = [&gc](PointF scrollPos, SizeF rate, SizeF scale, PointF origin = {0.0f, 0.0f})
		{
			gc.TransformTranslate(origin.GetX() - scrollPos.GetX() * rate.GetWidth(), origin.GetY() - scrollPos.GetY() * rate.GetHeight());
			gc.TransformScale(scale.GetWidth(), scale.GetHeight());
		};
		ApplyTransform(start, rate, scale);
	}
}
