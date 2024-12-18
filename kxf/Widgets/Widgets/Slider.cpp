#include "kxf-pch.h"
#include "Slider.h"
#include "WXUI/Slider.h"

namespace kxf::Widgets
{
	// Slider
	Slider::Slider()
	{
		InitializeWxWidget();
	}
	Slider::~Slider() = default;

	// IWidget
	bool Slider::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// ISliderWidget
	std::pair<int, int> Slider::GetRange() const
	{
		return {Get()->GetMin(), Get()->GetMax()};
	}
	void Slider::SetRange(int min, int max)
	{
		Get()->SetRange(min, max);
	}

	std::pair<int, int> Slider::GetSelectionRange() const
	{
		return {Get()->GetSelStart(), Get()->GetSelEnd()};
	}
	void Slider::SetSelectionRange(int min, int max)
	{
		if (min == max)
		{
			Get()->ClearSel();
		}
		else
		{
			Get()->SetSelection(min, max);
		}
	}

	int Slider::GetValue() const
	{
		return Get()->GetValue();
	}
	void Slider::SetValue(int value)
	{
		Get()->SetValue(value);
	}

	int Slider::GetThumbLength() const
	{
		return Get()->GetThumbLength();
	}
	void Slider::SetThumbLength(int length)
	{
		Get()->SetThumbLength(length);
	}

	Orientation Slider::GetOrientation() const
	{
		return Get()->HasFlag(wxSL_VERTICAL) ? Orientation::Vertical : Orientation::Horizontal;
	}
	void Slider::SetOrientation(Orientation orientation)
	{
		FlagSet style = Get()->GetWindowStyle();
		style.Mod(wxSL_VERTICAL, orientation == Orientation::Vertical);
		style.Mod(wxSL_HORIZONTAL, orientation == Orientation::Horizontal);

		Get()->SetWindowStyle(*style);
	}
}
