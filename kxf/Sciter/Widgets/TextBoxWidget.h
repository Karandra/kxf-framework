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
			bool HasValue() const
			{
				return m_EditArea.HasValue();
			}
			String GetValue() const
			{
				return m_EditArea.GetValue().GetString();
			}
			void SetValue(StringView value)
			{
				m_EditArea.SetValue(value);
			}
			void SetValue(const String& value)
			{
				SetValue(value.view());
			}

			bool HasLabel() const
			{
				return m_LabelArea.HasValue();
			}
			String GetLabel() const
			{
				return m_LabelArea.GetValue().GetString();
			}
			void SetLabel(const String& label)
			{
				m_LabelArea.SetValue(label);
			}
			void SetLabel(StringView value)
			{
				m_EditArea.SetValue(value);
			}

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
				:StdWidgetFactory("TextBox")
			{
			}

		public:
			std::unique_ptr<Widget> CreateWidget(Host& host, const Element& element) override
			{
				return std::make_unique<TextBoxWidget>(host, *this, element);
			}
			String GetWidgetStylesheet() const override;
	};
}
