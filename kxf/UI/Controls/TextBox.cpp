#include "kxf-pch.h"
#include "TextBox.h"
#include "kxf/System/Win32Error.h"

#include <CommCtrl.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(TextBox, wxTextCtrl);

	bool TextBox::Create(wxWindow* parent,
						 wxWindowID id,
						 const String& value,
						 const Point& pos,
						 const Size& size,
						 FlagSet<TextBoxStyle> style,
						 const wxValidator& validator
	)
	{
		if (wxTextCtrl::Create(parent, id, value, pos, size, style.ToInt(), validator))
		{
			SetDoubleBuffered(true);
			SetTabWidth();

			return true;
		}
		return false;
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

	bool TextBox::SetTabWidth(size_t width)
	{
		ScheduleRefresh();
		return ::SendMessageW(GetHandle(), EM_SETTABSTOPS, 1, (LPARAM)&width) != 0;
	}
	void TextBox::SetLimit(size_t max)
	{
		ScheduleRefresh();
		::SendMessageW(GetHandle(), EM_LIMITTEXT, max, 0);
	}

	bool TextBox::SetPasswordChar(const UniChar& c)
	{
		if (c)
		{
			ScheduleRefresh();
			::SendMessageW(GetHandle(), EM_SETPASSWORDCHAR, c.GetValue(), 0);
			return Win32Error::GetLastError().IsSuccess();
		}
		return false;
	}
	bool TextBox::SetHint(const wxString& label)
	{
		wxTextCtrl::SetHint(label);
		return ::SendMessageW(GetHandle(), EM_SETCUEBANNER, TRUE, (LPARAM)label.wc_str());
	}

	void TextBox::SetValue(const wxString& value)
	{
		wxTextCtrl::ChangeValue(value);
	}
	void TextBox::SetValueEvent(const String& value)
	{
		wxTextCtrl::SetValue(value);
	}

	void TextBox::Clear()
	{
		wxEventBlocker block(this, wxEVT_TEXT);
		wxTextCtrl::Clear();
	}
	void TextBox::ClearEvent()
	{
		wxTextCtrl::Clear();
	}
}
