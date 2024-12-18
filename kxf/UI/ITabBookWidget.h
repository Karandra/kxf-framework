#pragma once
#include "Common.h"
#include "IWidget.h"
#include "IBookWidget.h"

namespace kxf
{
	class KXF_API ITabBookWidget: public RTTI::ExtendInterface<ITabBookWidget, IBookWidget>
	{
		kxf_RTTI_DeclareIID(ITabBookWidget, {0xe2c6a242, 0xcb3e, 0x4d73, {0xa6, 0xaf, 0x54, 0x11, 0x84, 0xb6, 0xe6, 0xab}});

		public:
			virtual Direction GetTabPosition() const = 0;
			virtual void SetTabPosition(Direction position) = 0;

			virtual String GetTabTooltip(size_t index) const = 0;
			virtual void SetTabTooltip(size_t index, const String& tooltip) = 0;

			virtual bool IsCloseButtonVisible() const = 0;
			virtual void SetCloseButtonVisible(bool isVisible = true) = 0;
	};
}
