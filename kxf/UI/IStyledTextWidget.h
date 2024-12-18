#pragma once
#include "Common.h"
#include "IWidget.h"
#include "ITextWidget.h"

namespace kxf
{
	class KXF_API IStyledTextWidget: public RTTI::ExtendInterface<IStyledTextWidget, ITextWidget>
	{
		kxf_RTTI_DeclareIID(IStyledTextWidget, {0xa19a3c86, 0x4328, 0x43c3, {0x9c, 0x64, 0xf8, 0x16, 0x64, 0x9e, 0x1c, 0x53}});

		public:
			virtual ~IStyledTextWidget() = default;

		public:
			virtual bool IsIndentationGuidesVisible() const = 0;
			virtual void SetIndentationGuidesVisible(bool isVisible = true) = 0;
	};
}
