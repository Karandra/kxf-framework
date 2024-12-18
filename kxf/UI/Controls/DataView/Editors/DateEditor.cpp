#include "kxf-pch.h"
#include "DateEditor.h"
#include "kxf/Utility/Common.h"
#include <wx/datectrl.h>

namespace
{
	kxf::FlagSet<int> ConvertControlStyle(const kxf::UI::DataView::DateTimeValue& value)
	{
		using namespace kxf;
		using namespace kxf::UI::DataView;

		FlagSet<int> style;
		style.Add(wxDP_SPIN, value.ContainsOption(DateEditorOption::Spin));
		style.Add(wxDP_DROPDOWN, value.ContainsOption(DateEditorOption::Dropdown));
		style.Add(wxDP_ALLOWNONE, value.ContainsOption(DateEditorOption::AllowNone));
		style.Add(wxDP_SHOWCENTURY, value.ContainsOption(DateEditorOption::ShowCentury));

		return style;
	}
}

namespace kxf::UI::DataView
{
	bool DateTimeValue::FromAny(Any value)
	{
		if (value.GetAs(m_Value) || value.GetAs(*this))
		{
			return true;
		}
		else if (SYSTEMTIME systemTime; value.CheckType<SYSTEMTIME>() && value.GetAs(systemTime))
		{
			m_Value.SetSystemTime(systemTime);
			return true;
		}
		else if (FILETIME fileTime; value.CheckType<FILETIME>() && value.GetAs(fileTime))
		{
			if (::FileTimeToSystemTime(&fileTime, &systemTime))
			{
				m_Value.SetSystemTime(systemTime);
				return true;
			}
		}
		else if (time_t unixTime = 0; value.CheckType<time_t>() && value.GetAs(unixTime))
		{
			m_Value.SetUnixTime(unixTime);
			return true;
		}
		else if (String string; std::move(value).GetAs(string))
		{
			return m_Value.ParseISOCombined(string) || m_Value.ParseISOCombined(string, ' ') || m_Value.ParseTime(string);
		}
		return false;
	}
}

namespace kxf::UI::DataView
{
	wxWindow* DateEditor::CreateControl(wxWindow& parent, const Rect& cellRect, Any value)
	{
		const DateTimeValue dateTimeValue = Renderer::FromAnyUsing<DateTimeValue>(value);
		const auto style = ConvertControlStyle(dateTimeValue);

		wxDatePickerCtrl* editor = new wxDatePickerCtrl(&parent,
														wxID_NONE,
														dateTimeValue.GetDateTime(),
														cellRect.GetPosition(),
														cellRect.GetSize(),
														*style,
														GetValidator()
		);

		if (dateTimeValue.HasDateRange())
		{
			auto [lower, upper] = dateTimeValue.GetDateTimeRange();
			editor->SetRange(lower, upper);
		}
		return editor;
	}
	Any DateEditor::GetValue(wxWindow& control) const
	{
		wxDatePickerCtrl& editor = static_cast<wxDatePickerCtrl&>(control);
		return DateTime(editor.GetValue().ResetTime());
	}
}
