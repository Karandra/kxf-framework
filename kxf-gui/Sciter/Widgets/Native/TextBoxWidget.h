#pragma once
#include "Common.h"
#include "kxf-gui/Widgets/Controls/TextBox.h"

namespace kxf::Sciter
{
	class KXF_API NativeTextBoxWidget: public NativeWidget, public UI::TextBox
	{
		kxf_RTTI_DeclareIID(NativeTextBoxWidget, {0x7edd9ea0, 0x16ca, 0x427e, {0x81, 0x27, 0xbb, 0x5d, 0x8c, 0x11, 0xdd, 0x55}});
		kxf_RTTI_QueryInterface_Extend(NativeTextBoxWidget, NativeWidget);

		protected:
			// NativeWidget
			wxWindow& GetNativeWindow() override
			{
				return *this;
			}

			// Widget
			void OnAttached() override;

		public:
			NativeTextBoxWidget(Host& host, WidgetFactory& factory, const Element& element)
				:NativeWidget(host, factory, element)
			{
			}
			NativeTextBoxWidget(const NativeTextBoxWidget&) = delete;

		public:
			void ChangeValue(const wxString& value) override;

		public:
			NativeTextBoxWidget& operator=(const NativeTextBoxWidget&) = delete;
	};
}

namespace kxf::Sciter
{
	class KXF_API NativeTextBoxWidgetFactory: public StdWidgetFactory, public WidgetFactoryInstance<NativeTextBoxWidgetFactory>
	{
		public:
			NativeTextBoxWidgetFactory()
				:StdWidgetFactory("NativeTextBox")
			{
			}

		public:
			std::unique_ptr<Widget> CreateWidget(Host& host, const Element& element) override
			{
				return std::make_unique<NativeTextBoxWidget>(host, *this, element);
			}
			String GetWidgetStylesheet() const override;
	};
}
