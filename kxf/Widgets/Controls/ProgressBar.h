#pragma once
#include "kxf/Widgets/Common.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include "IProgressMeter.h"
#include <wx/gauge.h>

namespace kxf::UI
{
	enum class ProgressBarStyle: uint32_t
	{
		None = 0,

		Horizontal = wxGA_HORIZONTAL,
		Vertical = wxGA_VERTICAL,
		Smooth = wxGA_SMOOTH,
		Label = wxGA_TEXT,
		ReflectOnTaskbar = wxGA_PROGRESS,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::ProgressBarStyle);
}

namespace kxf::UI
{
	class KXF_API ProgressBar: public WindowRefreshScheduler<wxGauge>, public IProgressMeter
	{
		friend class ProgressBarWrapper;

		public:
			static constexpr FlagSet<ProgressBarStyle> DefaultStyle = ProgressBarStyle::Horizontal|ProgressBarStyle::Smooth;
			static constexpr int DefaultStep = 10;
			static constexpr int DefaultRange = 100;

		private:
			int m_Step = DefaultStep;
			bool m_InPulseMode = false;

		private:
			void OnPaint(wxPaintEvent& event);

		protected:
			int DoGetRange() const override
			{
				return wxGauge::GetRange();
			}
			void DoSetRange(int range) override
			{
				wxGauge::SetRange(range);
			}

			int DoGetValue() const override
			{
				return wxGauge::GetValue();
			}
			void DoSetValue(int value) override
			{
				m_InPulseMode = false;
				wxGauge::SetValue(value);
			}

			int DoGetStep() const override
			{
				return m_Step;
			}
			void DoSetStep(int step) override
			{
				const int range = DoGetRange();
				m_Step = step <= range ? step : range;
			}

			void DoPulse() override
			{
				if (!m_InPulseMode)
				{
					wxGauge::Pulse();
				}
				m_InPulseMode = true;
			}
			bool DoIsPulsing() const override
			{
				return m_InPulseMode;
			}

		public:
			ProgressBar() {}
			ProgressBar(wxWindow* parent,
						wxWindowID id,
						int range = DefaultRange,
						FlagSet<ProgressBarStyle> style = DefaultStyle
			)
			{
				Create(parent, id, range, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						int range = DefaultRange,
						FlagSet<ProgressBarStyle> style = DefaultStyle
			);

		public:
			// Resolve ambiguity
			void Pulse() override
			{
				IProgressMeter::Pulse();
			}

			int GetRange() const
			{
				return IProgressMeter::GetRange();
			}
			void SetRange(int range)
			{
				IProgressMeter::SetRange(range);
			}

			int GetValue() const
			{
				return IProgressMeter::GetValue();
			}
			void SetValue(int value)
			{
				IProgressMeter::SetValue(value);
			}
			void SetValue(int64_t current, int64_t max)
			{
				IProgressMeter::SetValue(current, max);
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(ProgressBar);
	};
}

namespace kxf::UI
{
	class KXF_API ProgressBarWrapper: public IProgressMeter
	{
		private:
			ProgressBar* m_ProgressBar = nullptr;

		protected:
			ProgressBar* GetProgressBar() const
			{
				return m_ProgressBar;
			}
			void SetProgressBar(ProgressBar* progressBar)
			{
				m_ProgressBar = progressBar;
			}

		protected:
			int DoGetRange() const override
			{
				return m_ProgressBar->DoGetRange();
			}
			void DoSetRange(int range) override
			{
				m_ProgressBar->DoSetRange(range);
			}

			int DoGetValue() const override
			{
				return m_ProgressBar->DoGetValue();
			}
			void DoSetValue(int value) override
			{
				m_ProgressBar->DoSetValue(value);
			}

			int DoGetStep() const override
			{
				return m_ProgressBar->DoGetStep();
			}
			void DoSetStep(int step) override
			{
				m_ProgressBar->DoSetStep(step);
			}

			void DoPulse() override
			{
				m_ProgressBar->DoPulse();
			}
			bool DoIsPulsing() const override
			{
				return m_ProgressBar->DoIsPulsing();
			}

		public:
			ProgressBarWrapper(ProgressBar* progressBar = nullptr)
				:m_ProgressBar(progressBar)
			{
			}
	};
}
