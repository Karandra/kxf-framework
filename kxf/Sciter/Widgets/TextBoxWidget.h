#pragma once
#include "Common.h"
#include "kxf/EventSystem/EvtHandler.h"

namespace kxf::Sciter
{
	class KX_API TextBoxWidget: public Widget
	{
		KxRTTI_DeclareIID(TextBoxWidget, {0xec599691, 0x8610, 0x4987, {0x99, 0xcb, 0xe3, 0x27, 0x81, 0xf2, 0xc4, 0x2f}});
		KxRTTI_QueryInterface_Extend(TextBoxWidget, Widget);

		private:
			Element m_LabelArea;
			Element m_EditArea;

			EvtHandler m_EditAreaEvtHandler;

		protected:
			// Widget
			void OnAttached() override;

		public:
			TextBoxWidget(Host& host, WidgetFactory& factory, const Element& element)
				:Widget(host, factory, element)
			{
			}
			TextBoxWidget(const TextBoxWidget&) = delete;

		public:
			String GetValue() const;
			void SetValue(StringView value);
			void SetValue(const String& value)
			{
				SetValue(value.GetView());
			}

			String GetLabel() const;
			void SetLabel(const String& label);

		public:
			TextBoxWidget& operator=(const TextBoxWidget&) = delete;
	};
}

namespace kxf::Sciter
{
	class KX_API TextBoxWidgetFactory: public StdWidgetFactory, public WidgetFactoryInstance<TextBoxWidgetFactory>
	{
		public:
			TextBoxWidgetFactory()
				:StdWidgetFactory(wxS("TextBox"))
			{
			}

		public:
			std::unique_ptr<Widget> CreateWidget(Host& host, const Element& element, const String& className) override
			{
				return std::make_unique<TextBoxWidget>(host, *this, element);
			}
			String GetWidgetStylesheet() const override;
	};
}
