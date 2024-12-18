#include "kxf-pch.h"
#include "TextBox.h"
#include "../../Private/WxTextCtrlWrapper.h"

namespace kxf::WXUI
{
	bool TextBox::DoTryBefore(wxEvent& event)
	{
		return TranslateTextEvent(m_Widget, event);
	}
	WXLRESULT TextBox::MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		// Ctrl + A (A is 1)
		if (msg == WM_CHAR && wParam == 1)
		{
			::SendMessageW(GetHandle(), EM_SETSEL, 0, -1);
			return 1;
		}
		return wxTextCtrl::MSWWindowProc(msg, wParam, lParam);
	}

	bool TextBox::Create(wxWindow* parent,
						 const String& text,
						 const Point& pos,
						 const Size& size
	)
	{
		if (wxTextCtrl::Create(parent, wxID_NONE, text, pos, size, wxTE_LEFT|wxTE_BESTWRAP))
		{
			SetDoubleBuffered(true);

			Private::WxTextCtrlWrapper wrapper(*this);
			wrapper.SetTabWidth(16);

			return true;
		}
		return false;
	}
}
