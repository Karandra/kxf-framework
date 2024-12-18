#pragma once
#include "Common.h"
#include "../IButtonWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::WXUI
{
	class Button;
}

namespace kxf::Widgets
{
	class KXF_API Button: public RTTI::Implementation<Button, Private::BasicWxWidget<Button, WXUI::Button, IButtonWidget>, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			Button();
			~Button();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IButtonWidget
			String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const override;
			void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) override;

			BitmapImage GetIcon() const override;
			void SetIcon(const BitmapImage& icon, FlagSet<Direction> direction = {}) override;
			void SetStdIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction = {}) override;

			bool IsDefault() const override;
			std::shared_ptr<IButtonWidget> SetDefault() override;

			bool IsDropdownEnabled() const override;
			void SetDropdownEnbled(bool enabled = true) override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
