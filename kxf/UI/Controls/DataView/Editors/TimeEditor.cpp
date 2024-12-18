#include "kxf-pch.h"
#include "TimeEditor.h"
#include "DateEditor.h"
#include <wx/timectrl.h>

namespace
{
	kxf::DateTime ResetDate(kxf::DateTime date)
	{
		using namespace kxf;

		date.SetDay(0);
		date.SetMonth(Month::None);
		date.SetYear(DateTime::InvalidYear);

		return date;
	}
}

namespace kxf::UI::DataView
{
	wxWindow* TimeEditor::CreateControl(wxWindow& parent, const Rect& cellRect, Any value)
	{
		const DateTimeValue dateValue = Renderer::FromAnyUsing<DateTimeValue>(value);

		return new wxTimePickerCtrl(&parent,
									wxID_NONE,
									ResetDate(dateValue.GetDateTime()),
									cellRect.GetPosition(),
									cellRect.GetSize(),
									0,
									GetValidator()
		);
	}
	Any TimeEditor::GetValue(wxWindow& control) const
	{
		wxTimePickerCtrl& editor = static_cast<wxTimePickerCtrl&>(control);
		return ResetDate(editor.GetValue());
	}
}
