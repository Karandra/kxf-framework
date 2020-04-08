#pragma once
#include "Event.h"

namespace KxFramework::Sciter
{
	class KX_API ScrollEvent: public Event
	{
		public:
			KxEVENT_MEMBER(ScrollEvent, ScrollHome);
			KxEVENT_MEMBER(ScrollEvent, ScrollEnd);
			KxEVENT_MEMBER(ScrollEvent, ScrollPosition);

			KxEVENT_MEMBER(ScrollEvent, ScrollStepPlus);
			KxEVENT_MEMBER(ScrollEvent, ScrollStepMinus);
			KxEVENT_MEMBER(ScrollEvent, ScrollPagePlus);
			KxEVENT_MEMBER(ScrollEvent, ScrollPageMinus);

			KxEVENT_MEMBER(ScrollEvent, ScrollSliderPressed);
			KxEVENT_MEMBER(ScrollEvent, ScrollSliderReleased);

			KxEVENT_MEMBER(ScrollEvent, ScrollCornerPressed);
			KxEVENT_MEMBER(ScrollEvent, ScrollCornerReleased);

		protected:
			wxOrientation m_Orientation = wxOrientation::wxVERTICAL;
			int m_Position = wxDefaultCoord;
			ScrollSource m_Source = ScrollSource::Unknown;

		public:
			ScrollEvent(Host& host)
				:Event(host)
			{
			}

		public:
			ScrollEvent* Clone() const override
			{
				return new ScrollEvent(*this);
			}

			wxOrientation GetOrientation() const
			{
				return m_Orientation;
			}
			void SetOrientation(wxOrientation value)
			{
				m_Orientation = value;
			}
			
			int GetPosition() const
			{
				return m_Position;
			}
			void SetPosition(int value)
			{
				m_Position = value;
			}
			
			ScrollSource GetSource() const
			{
				return m_Source;
			}
			void SetSource(ScrollSource value)
			{
				m_Source = value;
			}
	};
}

namespace KxFramework::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollHome);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollEnd);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollPosition);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollStepPlus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollStepMinus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollPagePlus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollPageMinus);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollSliderPressed);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollSliderReleased);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollCornerPressed);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollCornerReleased);
}
