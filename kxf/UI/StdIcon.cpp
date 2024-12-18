#include "kxf-pch.h"
#include "StdIcon.h"
#include <wx/defs.h>

namespace kxf::UI
{
	FlagSet<StdIcon> FromWxStdIcon(int stdWxIcon) noexcept
	{
		FlagSet<StdIcon> stdIcon;
		stdIcon.Add(StdIcon::Error, stdWxIcon & wxICON_ERROR);
		stdIcon.Add(StdIcon::Warning, stdWxIcon & wxICON_WARNING);
		stdIcon.Add(StdIcon::Question, stdWxIcon & wxICON_QUESTION);
		stdIcon.Add(StdIcon::Information, stdWxIcon & wxICON_INFORMATION);
		stdIcon.Add(StdIcon::Authentication, stdWxIcon & wxICON_AUTH_NEEDED);

		return stdIcon;
	}
	FlagSet<int> ToWxStdIcon(FlagSet<StdIcon> stdIcon) noexcept
	{
		FlagSet<int> wxStdIcon = wxICON_NONE;
		wxStdIcon.Add(wxICON_ERROR, stdIcon & StdIcon::Error);
		wxStdIcon.Add(wxICON_WARNING, stdIcon & StdIcon::Warning);
		wxStdIcon.Add(wxICON_QUESTION, stdIcon & StdIcon::Question);
		wxStdIcon.Add(wxICON_INFORMATION, stdIcon & StdIcon::Information);
		wxStdIcon.Add(wxICON_AUTH_NEEDED, stdIcon & StdIcon::Authentication);

		return wxStdIcon;
	}
}
