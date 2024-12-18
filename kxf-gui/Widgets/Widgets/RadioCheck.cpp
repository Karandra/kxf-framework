#include "kxf-pch.h"
#include "RadioCheck.h"
#include "WXUI/RadioCheck.h"

namespace kxf::Widgets
{
	// RadioCheck
	RadioCheck::RadioCheck()
	{
		InitializeWxWidget();
	}
	RadioCheck::~RadioCheck() = default;

	// IWidget
	bool RadioCheck::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// ICheckWidget
	String RadioCheck::GetLabel(FlagSet<WidgetTextFlag> flags) const
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
	void RadioCheck::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
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

	bool RadioCheck::IsThirdStateSelectable() const
	{
		return false;
	}
	void RadioCheck::SetThirdStateSelectable(bool isSelectable)
	{
	}

	CheckWidgetValue RadioCheck::GetValue() const
	{
		return Get()->GetValue() ? CheckWidgetValue::Checked : CheckWidgetValue::Unchecked;
	}
	void RadioCheck::SetValue(CheckWidgetValue value)
	{
		Get()->SetValue(value == CheckWidgetValue::Checked);
	}
}
