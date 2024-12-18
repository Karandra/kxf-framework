#pragma once
#include "Common.h"

namespace kxf
{
	class IGraphicsContext;

	enum class ScrollbarVisibility
	{
		ShowDefault = -1,
		ShowAlways = 1,
		ShowNever = 0
	};
}

namespace kxf
{
	class KXF_API IScrollableWidget: public RTTI::Interface<IScrollableWidget>
	{
		kxf_RTTI_DeclareIID(IScrollableWidget, {0x2dbba444, 0xbab2, 0x4239, {0xa9, 0x45, 0xe0, 0x72, 0xa0, 0x2e, 0x84, 0xe8}});

		public:
			virtual ~IScrollableWidget() = default;

		public:
			virtual bool IsKeyboardScrollingEnabled() const = 0;
			virtual void SetKeyboardScrollingEnabled(bool enabled = true) = 0;

			virtual Point CalcScrolledPosition(const Point& point) const = 0;
			virtual Point CalcUnscrolledPosition(const Point& point) const = 0;

			virtual void SetupScrollbars(const Size& pixelPerUnit, const Size& unitCount) = 0;
			virtual void ShowScrollbars(ScrollbarVisibility x, ScrollbarVisibility y) = 0;

			virtual Size GetScrollRate() const = 0;
			virtual void SetScrollRate(const Size& scrollRate) = 0;

			virtual Point GetViewStart() const = 0;
			virtual void ScrollTo(const Point& point) = 0;

			virtual SizeF GetScrollScale() const = 0;
			virtual void SetScrollScale(const SizeF& scale) = 0;

			virtual void AdjustGraphicsContext(IGraphicsContext& gc);
	};
}
