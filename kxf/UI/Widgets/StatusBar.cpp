#include "kxf-pch.h"
#include "StatusBar.h"
#include "WXUI/StatusBarEx.h"
#include "kxf/Drawing/GraphicsRenderer.h"

namespace kxf::Widgets
{
	// StatusBar
	StatusBar::StatusBar()
	{
		InitializeWxWidget();
	}
	StatusBar::~StatusBar() = default;

	// IWidget
	bool StatusBar::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IStatusBarWidget
	size_t StatusBar::GetPaneCount() const
	{
		return Get()->GetPaneCount();
	}
	void StatusBar::SetPaneCount(size_t count)
	{
		Get()->SetPaneCount(count);
	}

	int StatusBar::GetPaneWidth(size_t index) const
	{
		return Get()->GetPaneWidth(index);
	}
	void StatusBar::SetPaneWidth(size_t index, int width)
	{
		Get()->SetPaneWidth(index, width);
	}

	String StatusBar::GetPaneLabel(size_t index) const
	{
		return Get()->GetPaneLabel(index);
	}
	void StatusBar::SetPaneLabel(size_t index, const String& label)
	{
		Get()->SetPaneLabel(index, label);
	}

	// IProgressMeter
	int StatusBar::GetRange() const
	{
		return Get()->GetRange();
	}
	void StatusBar::SetRange(int range)
	{
		Get()->SetRange(range);
	}

	int StatusBar::GetValue() const
	{
		return Get()->GetValue();
	}
	void StatusBar::SetValue(int value)
	{
		Get()->SetValue(value);
	}

	int StatusBar::GetStep() const
	{
		return Get()->GetStep();
	}
	void StatusBar::SetStep(int step)
	{
		Get()->SetValue(step);
	}

	void StatusBar::Pulse()
	{
	}
	bool StatusBar::IsPulsing() const
	{
		return false;
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> StatusBar::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
