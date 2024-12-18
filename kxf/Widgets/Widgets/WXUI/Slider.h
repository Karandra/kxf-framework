#include "Common.h"
#include "../../ISliderWidget.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include <wx/slider.h>
#include <wx/systhemectrl.h>

namespace kxf::WXUI
{
	class KXF_API Slider: public EvtHandlerWrapper<Slider, UI::WindowRefreshScheduler<wxSystemThemedControl<wxSlider>>>
	{
		friend class EvtHandlerWrapper;

		private:
			ISliderWidget& m_Widget;

		private:
			bool DoTryBefore(wxEvent& event);

		public:
			Slider(ISliderWidget& widget)
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
	};
}
