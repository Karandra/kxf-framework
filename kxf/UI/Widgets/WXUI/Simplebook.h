#pragma once
#include "Common.h"
#include "../../IBookWidget.h"
#include <wx/simplebook.h>

namespace kxf::WXUI
{
	class KXF_API Simplebook: public EvtHandlerWrapper<Simplebook, wxSimplebook>
	{
		friend class EvtHandlerWrapper;

		private:
			IBookWidget& m_Widget;

		private:
			bool DoTryBefore(wxEvent& event);

		public:
			Simplebook(IBookWidget& widget)
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
			bool ShouldInheritColours() const override
			{
				return true;
			}
	};
}
