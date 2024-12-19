#include "kxf-pch.h"
#include "ProgressBar.h"
#include "WXUI/ProgressBar.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"

namespace kxf::Widgets
{
	// ProgressBar
	ProgressBar::ProgressBar()
	{
		InitializeWxWidget();
	}
	ProgressBar::~ProgressBar() = default;

	// IWidget
	bool ProgressBar::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IProgressMeter
	int ProgressBar::GetRange() const
	{
		return Get()->GetRange();
	}
	void ProgressBar::SetRange(int range)
	{
		Get()->SetRange(range);
	}

	int ProgressBar::GetValue() const
	{
		return Get()->GetValue();
	}
	void ProgressBar::SetValue(int value)
	{
		Get()->SetValue(value);
	}

	int ProgressBar::GetStep() const
	{
		return Get()->GetStep();
	}
	void ProgressBar::SetStep(int step)
	{
		Get()->SetValue(step);
	}

	void ProgressBar::Pulse()
	{
		Get()->Pulse();
	}
	bool ProgressBar::IsPulsing() const
	{
		return Get()->IsPulsing();
	}

	ProgressMeterState ProgressBar::GetState() const
	{
		return Get()->GetState();
	}
	void ProgressBar::SetState(ProgressMeterState state)
	{
		Get()->SetState(state);
	}

	// IProgressMeterWidget
	String ProgressBar::GetLabel(FlagSet<WidgetTextFlag> flags) const
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
	void ProgressBar::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
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

	Orientation ProgressBar::GetOrientation() const
	{
		return Get()->IsVertical() ? Orientation::Vertical : Orientation::Horizontal;
	}
	void ProgressBar::SetOrientation(Orientation orientation)
	{
		FlagSet style = Get()->GetWindowStyle();
		style.Mod(wxGA_VERTICAL, orientation == Orientation::Vertical);
		style.Mod(wxGA_HORIZONTAL, orientation == Orientation::Horizontal);

		Get()->SetWindowStyle(*style);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> ProgressBar::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
