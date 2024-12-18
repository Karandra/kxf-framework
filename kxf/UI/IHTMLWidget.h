#pragma once
#include "Common.h"
#include "IWidget.h"
#include "ITextWidget.h"

namespace kxf
{
	class KXF_API IHTMLWidget: public RTTI::ExtendInterface<IHTMLWidget, IWidget, ITextEntry>
	{
		kxf_RTTI_DeclareIID(IHTMLWidget, {0x1425d63b, 0x9d98, 0x4174, {0xb8, 0x2d, 0xf0, 0x2a, 0xcd, 0xca, 0x6d, 0xed}});

		public:
			virtual ~IHTMLWidget() = default;

		public:
			virtual String GetHTML() const = 0;
			virtual void SetHTML(const String& html) = 0;

			virtual bool IsSelectable() const = 0;
			virtual void SetSelectable(bool isSelectable = true) = 0;

			virtual void SetBackgroundImage(const IImage2D& image) = 0;
	};
}
