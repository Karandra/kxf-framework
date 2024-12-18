#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class KXF_API ISliderWidget: public RTTI::ExtendInterface<ISliderWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(ISliderWidget, {0x9f0f249a, 0xaf5d, 0x428e, {0x82, 0x71, 0x40, 0x42, 0x3, 0x42, 0x9, 0x3f}});

		public:
			virtual std::pair<int, int> GetRange() const = 0;
			virtual void SetRange(int min, int max) = 0;

			virtual std::pair<int, int> GetSelectionRange() const = 0;
			virtual void SetSelectionRange(int min, int max) = 0;

			virtual int GetValue() const = 0;
			virtual void SetValue(int value) = 0;

			virtual int GetThumbLength() const = 0;
			virtual void SetThumbLength(int length) = 0;

			virtual Orientation GetOrientation() const = 0;
			virtual void SetOrientation(Orientation orientation) = 0;
	};
}
