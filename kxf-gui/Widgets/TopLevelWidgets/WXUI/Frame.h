#pragma once
#include "Common.h"
#include "kxf-gui/Widgets/ITopLevelWidget.h"
#include <wx/frame.h>

namespace kxf::WXUI
{
	class KXF_API Frame: public EvtHandlerWrapper<Frame, wxFrame>
	{
		private:
			ITopLevelWidget& m_Widget;

		public:
			Frame(ITopLevelWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& title,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			// wxTopLevelWindow
			bool ShouldPreventAppExit() const override
			{
				return m_Widget.ShouldPreventApplicationExit();
			}
	};
}
