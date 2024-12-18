#pragma once
#include "Common.h"
#include "../ICheckWidget.h"

namespace kxf::WXUI
{
	class Check;
}

namespace kxf::Widgets
{
	class KXF_API Check: public RTTI::Implementation<Check, Private::BasicWxWidget<Check, WXUI::Check, ICheckWidget>>
	{
		public:
			Check();
			~Check();

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
