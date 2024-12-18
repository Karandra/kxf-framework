#pragma once
#include "Common.h"
#include "../IStatusBarWidget.h"
#include "../IProgressMeterWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::WXUI
{
	class StatusBarEx;
}

namespace kxf::Widgets
{
	class KXF_API StatusBar: public RTTI::Implementation<StatusBar, Private::BasicWxWidget<StatusBar, WXUI::StatusBarEx, IStatusBarWidget>, IProgressMeter, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			StatusBar();
			~StatusBar();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IStatusBarWidget
			size_t GetPaneCount() const override;
			void SetPaneCount(size_t count) override;

			int GetPaneWidth(size_t index) const override;
			void SetPaneWidth(size_t index, int width) override;

			String GetPaneLabel(size_t index) const override;
			void SetPaneLabel(size_t index, const String& label) override;

			// IProgressMeter
			int GetRange() const override;
			void SetRange(int range) override;

			int GetValue() const override;
			void SetValue(int value) override;

			int GetStep() const override;
			void SetStep(int step) override;

			void Pulse() override;
			bool IsPulsing() const override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
