#pragma once
#include "Common.h"
#include "../../ISearchWidget.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
#include <wx/srchctrl.h>

namespace kxf::WXUI
{
	class KXF_API SearchBox: public EvtHandlerWrapper<SearchBox, UI::WindowRefreshScheduler<wxSearchCtrl>>
	{
		friend class EvtHandlerWrapper;

		private:
			ISearchWidget& m_Widget;

			wxEvtHandler m_EvtHandler;
			String m_Value;

		private:
			void OnText(wxCommandEvent& event);
			void OnEnter(wxCommandEvent& event);
			void OnKillFocus(wxFocusEvent& event);
			void OnCancelSearch(wxCommandEvent& event);

		private:
			bool DoTryBefore(wxEvent& event);

		protected:
			// wxWindow
			WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;

		public:
			SearchBox(ISearchWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}
			~SearchBox()
			{
				if (m_EvtHandler.GetClientData() == this)
				{
					PopEventHandler();
				}
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			// wxWindow
			void SetLabel(const wxString& label) override
			{
				m_Value = label;
				wxSearchCtrl::SetLabel(label);
			}
	};
}
