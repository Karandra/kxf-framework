#pragma once
#include "Common.h"
#include "../../ITextWidget.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/textctrl.h>

namespace kxf::WXUI
{
	class KXF_API TextBox: public EvtHandlerWrapper<TextBox, UI::WindowRefreshScheduler<wxTextCtrl>>
	{
		friend class EvtHandlerWrapper;

		private:
			ITextWidget& m_Widget;

		private:
			bool DoTryBefore(wxEvent& event);

		protected:
			// wxWindow
			WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;

		public:
			TextBox(ITextWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& text,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);
	};
}
