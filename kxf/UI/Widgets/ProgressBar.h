#pragma once
#include "Common.h"
#include "../IProgressMeterWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::WXUI
{
	class ProgressBar;
}

namespace kxf::Widgets
{
	class KXF_API ProgressBar: public RTTI::Implementation<ProgressBar, Private::BasicWxWidget<ProgressBar, WXUI::ProgressBar, IProgressMeterWidget>, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			ProgressBar();
			~ProgressBar();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IProgressMeter
			int GetRange() const override;
			void SetRange(int range) override;

			int GetValue() const override;
			void SetValue(int value) override;

			int GetStep() const override;
			void SetStep(int step) override;

			void Pulse() override;
			bool IsPulsing() const override;

			ProgressMeterState GetState() const override;
			void SetState(ProgressMeterState state) override;

			// IProgressMeterWidget
			String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const override;
			void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) override;

			Orientation GetOrientation() const override;
			void SetOrientation(Orientation orientation) override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
