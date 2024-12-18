#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class KXF_API IStatusBarWidget: public RTTI::ExtendInterface<IStatusBarWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(IStatusBarWidget, {0xbf67357, 0x2d0f, 0x480e, {0xaa, 0xdb, 0x7, 0xf4, 0xd6, 0xe0, 0xf4, 0xd1}});

		public:
			virtual size_t GetPaneCount() const = 0;
			virtual void SetPaneCount(size_t count) = 0;

			virtual String GetPaneLabel(size_t index) const = 0;
			virtual void SetPaneLabel(size_t index, const String& text) = 0;

			virtual int GetPaneWidth(size_t index) const = 0;
			virtual void SetPaneWidth(size_t index, int width) = 0;
	};
}
