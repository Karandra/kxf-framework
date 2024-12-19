#pragma once
#include "Common.h"
#include "../../IProgressMeterWidget.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
#include "kxf-gui/Drawing/IRendererNative.h"
#include <wx/gauge.h>
#include <wx/systhemectrl.h>

namespace kxf::WXUI
{
	class KXF_API ProgressBar: public EvtHandlerWrapper<ProgressBar, UI::WindowRefreshScheduler<wxSystemThemedControl<wxGauge>>>
	{
		private:
			IProgressMeterWidget& m_Widget;
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;

			int m_Step = 0;
			bool m_InPulseMode = false;

		private:
			void OnPaint(wxPaintEvent& event);

		public:
			ProgressBar(IProgressMeterWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			// wxWindow
			void SetLabel(const wxString& label) override;

			// wxGauge
			int GetRange() const override
			{
				return wxGauge::GetRange();
			}
			void SetRange(int range) override
			{
				wxGauge::SetRange(std::clamp(range, 0, std::numeric_limits<int>::max()));
			}

			int GetValue() const override
			{
				return wxGauge::GetValue();
			}
			void SetValue(int value) override
			{
				m_InPulseMode = false;
				wxGauge::SetValue(std::clamp(value, 0, GetRange()));
			}

			// ProgressBar
			int GetStep() const
			{
				return m_Step;
			}
			void SetStep(int step)
			{
				m_Step = std::clamp(step, 0, GetRange());
			}

			void Pulse()
			{
				if (!m_InPulseMode)
				{
					wxGauge::Pulse();
				}
				m_InPulseMode = true;
			}
			bool IsPulsing() const
			{
				return m_InPulseMode;
			}

			ProgressMeterState GetState() const;
			void SetState(ProgressMeterState state);
	};
}
