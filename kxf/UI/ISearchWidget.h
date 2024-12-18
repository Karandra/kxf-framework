#pragma once
#include "Common.h"
#include "IWidget.h"
#include "ITextWidget.h"

namespace kxf
{
	class KXF_API ISearchWidget: public RTTI::ExtendInterface<ISearchWidget, IWidget, ITextEntry>
	{
		kxf_RTTI_DeclareIID(ISearchWidget, {0xa05e773a, 0xda37, 0x460b, {0x98, 0x62, 0x30, 0xa6, 0x67, 0xd7, 0x13, 0x97}});

		public:
			virtual ~ISearchWidget() = default;

		public:
			virtual bool IsSearchButtonVisible() const = 0;
			virtual void SetSearchButtonVisible(bool isVisible = true) = 0;

			virtual bool IsCancelButtonVisible() const = 0;
			virtual void SetCancelButtonVisible(bool isVisible = true) = 0;
	};
}
