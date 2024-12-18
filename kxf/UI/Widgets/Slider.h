#pragma once
#include "Common.h"
#include "../ISliderWidget.h"

namespace kxf::WXUI
{
	class Slider;
}

namespace kxf::Widgets
{
	class KXF_API Slider: public RTTI::Implementation<Slider, Private::BasicWxWidget<Slider, WXUI::Slider, ISliderWidget>>
	{
		public:
			Slider();
			~Slider();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// ISliderWidget
			std::pair<int, int> GetRange() const override;
			void SetRange(int min, int max) override;

			std::pair<int, int> GetSelectionRange() const override;
			void SetSelectionRange(int min, int max) = 0;

			int GetValue() const override;
			void SetValue(int value) override;

			int GetThumbLength() const override;
			void SetThumbLength(int length) override;

			Orientation GetOrientation() const override;
			void SetOrientation(Orientation orientation) override;
	};
}
