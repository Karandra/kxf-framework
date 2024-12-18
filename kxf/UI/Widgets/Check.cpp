#include "kxf-pch.h"
#include "Check.h"
#include "WXUI/Check.h"

namespace kxf::Widgets
{
	// Check
	Check::Check()
	{
		InitializeWxWidget();
	}
	Check::~Check() = default;

	// IWidget
	bool Check::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// ICheckWidget
	String Check::GetLabel(FlagSet<WidgetTextFlag> flags) const
	{
		if (flags.Contains(WidgetTextFlag::WithMnemonics))
		{
			return Get()->GetLabel();
		}
		else
		{
			return Get()->GetLabelText();
		}
	}
	void Check::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
	{
		if (flags.Contains(WidgetTextFlag::WithMnemonics))
		{
			Get()->SetLabel(label);
		}
		else
		{
			Get()->SetLabelText(label);
		}
	}

	bool Check::IsThirdStateSelectable() const
	{
		return Get()->Is3rdStateAllowedForUser();
	}
	void Check::SetThirdStateSelectable(bool isSelectable)
	{
		FlagSet<int> style = Get()->GetWindowStyle();
		style.Mod(wxCHK_ALLOW_3RD_STATE_FOR_USER, isSelectable);

		Get()->SetWindowStyle(*style);
	}

	CheckWidgetValue Check::GetValue() const
	{
		switch (Get()->GetCheckState())
		{
			case wxCheckBoxState::wxCHK_CHECKED:
			{
				return CheckWidgetValue::Checked;
			}
			case wxCheckBoxState::wxCHK_UNCHECKED:
			{
				return CheckWidgetValue::Unchecked;
			}
			case wxCheckBoxState::wxCHK_UNDETERMINED:
			{
				return CheckWidgetValue::Indeterminate;
			}
		};
		return CheckWidgetValue::Unchecked;
	}
	void Check::SetValue(CheckWidgetValue value)
	{
		switch (value)
		{
			case CheckWidgetValue::Checked:
			{
				Get()->SetCheckState(wxCheckBoxState::wxCHK_CHECKED);
				break;
			}
			case CheckWidgetValue::Unchecked:
			{
				Get()->SetCheckState(wxCheckBoxState::wxCHK_UNCHECKED);
				break;
			}
			case CheckWidgetValue::Indeterminate:
			{
				Get()->SetCheckState(wxCheckBoxState::wxCHK_UNDETERMINED);
				break;
			}
		};
	}
}
