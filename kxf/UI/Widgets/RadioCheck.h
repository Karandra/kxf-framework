#pragma once
#include "Common.h"
#include "../ICheckWidget.h"

namespace kxf::WXUI
{
	class RadioCheck;
}

namespace kxf::Widgets
{
	class KXF_API RadioCheck: public RTTI::Implementation<RadioCheck, Private::BasicWxWidget<RadioCheck, WXUI::RadioCheck, ICheckWidget>>
	{
		public:
			RadioCheck();
			~RadioCheck();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// ICheckWidget
			String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const override;
			void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) override;

			bool IsThirdStateSelectable() const override;
			void SetThirdStateSelectable(bool isSelectable = true) override;

			CheckWidgetValue GetValue() const override;
			void SetValue(CheckWidgetValue value) override;
	};
}
