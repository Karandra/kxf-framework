#pragma once
#include "Common.h"
#include "../../ICheckWidget.h"
#include <wx/radiobut.h>

namespace kxf::WXUI
{
	class KXF_API RadioCheck: public EvtHandlerWrapper<RadioCheck, wxRadioButton>
	{
		friend class EvtHandlerWrapper;

		private:
			ICheckWidget& m_Widget;

		private:
			bool DoTryBefore(wxEvent& event);

		public:
			RadioCheck(ICheckWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);
	};
}
