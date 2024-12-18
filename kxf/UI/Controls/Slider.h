#pragma once
#include "kxf/UI/Common.h"
#include <wx/slider.h>

namespace kxf::UI
{
	enum class SliderStyle: uint32_t
	{
		None = 0,

		Horizontal = wxSL_HORIZONTAL,
		Vertical = wxSL_VERTICAL,
		AutoTicks = wxSL_AUTOTICKS,
		NoThumb = 32, // 32 is free
		Both = wxSL_BOTH,
		SelRange = wxSL_SELRANGE,
		Inverse = wxSL_INVERSE,
		MinMaxLabels = wxSL_MIN_MAX_LABELS,
		ValueLabel = wxSL_VALUE_LABEL,

		Left = wxSL_LEFT,
		Top = wxSL_TOP,
		Right = wxSL_RIGHT,
		Bottom = wxSL_BOTTOM,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::SliderStyle);
}

namespace kxf::UI
{
	class KXF_API Slider: public wxSlider
	{
		public:
			static constexpr FlagSet<SliderStyle> DefaultStyle = SliderStyle::Horizontal;
			static constexpr int DefaultStep = 10;

		private:
			int m_Step = DefaultStep;

		public:
			Slider() = default;
			Slider(wxWindow* parent,
				   wxWindowID id,
				   int value,
				   int minValue,
				   int maxValue,
				   FlagSet<SliderStyle> style = DefaultStyle,
				   const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, value, minValue, maxValue, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						int value,
						int minValue,
						int maxValue,
						FlagSet<SliderStyle> style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			);

		public:
			bool IsInversed() const
			{
				return HasFlag(wxSL_INVERSE);
			}
			int GetSelStart() const
			{
				if (IsInversed())
				{
					return wxSlider::GetMax() - wxSlider::GetSelEnd();
				}
				else
				{
					return wxSlider::GetSelStart();
				}
			}
			int GetSelEnd() const
			{
				if (IsInversed())
				{
					return wxSlider::GetMax() - wxSlider::GetSelStart();
				}
				else
				{
					return wxSlider::GetSelEnd();
				}
			}
			void SetSelection(int min, int max)
			{
				if (IsInversed())
				{
					wxSlider::SetSelection(wxSlider::GetMax() - max, wxSlider::GetMax() - min);
				}
				else
				{
					wxSlider::SetSelection(min, max);
				}
			}

			int GetStep() const
			{
				return m_Step;
			}
			void SetStep(int step)
			{
				m_Step = step;
			}
			void Advance(int value)
			{
				int max = GetMax();
				value += GetValue();
				if (value <= max)
				{
					SetValue(value);
				}
				else
				{
					SetValue(max);
				}
			}
			void StepIt()
			{
				Advance(GetStep());
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(Slider);
	};
}
