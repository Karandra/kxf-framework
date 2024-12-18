#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class KXF_API ScrollEvent: public SciterEvent
	{
		public:
			kxf_EVENT_MEMBER(ScrollEvent, ScrollHome);
			kxf_EVENT_MEMBER(ScrollEvent, ScrollEnd);
			kxf_EVENT_MEMBER(ScrollEvent, ScrollPosition);

			kxf_EVENT_MEMBER(ScrollEvent, ScrollStepPlus);
			kxf_EVENT_MEMBER(ScrollEvent, ScrollStepMinus);
			kxf_EVENT_MEMBER(ScrollEvent, ScrollPagePlus);
			kxf_EVENT_MEMBER(ScrollEvent, ScrollPageMinus);

			kxf_EVENT_MEMBER(ScrollEvent, ScrollSliderPressed);
			kxf_EVENT_MEMBER(ScrollEvent, ScrollSliderReleased);

			kxf_EVENT_MEMBER(ScrollEvent, ScrollCornerPressed);
			kxf_EVENT_MEMBER(ScrollEvent, ScrollCornerReleased);

		protected:
			Orientation m_Orientation = Orientation::Vertical;
			ScrollSource m_Source = ScrollSource::Unknown;
			int m_Position = Geometry::DefaultCoord;

		public:
			ScrollEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ScrollEvent>(std::move(*this));
			}

			// IEvent
			Orientation GetOrientation() const
			{
				return m_Orientation;
			}
			void SetOrientation(Orientation value)
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

namespace kxf::Sciter
{
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollHome);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollEnd);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollPosition);

	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollStepPlus);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollStepMinus);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollPagePlus);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollPageMinus);

	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollSliderPressed);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollSliderReleased);

	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollCornerPressed);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollCornerReleased);
}
