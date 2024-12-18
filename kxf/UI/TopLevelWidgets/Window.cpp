#include "kxf-pch.h"
#include "Window.h"
#include "WXUI/Frame.h"

namespace kxf::Widgets
{
	Window::Window()
	{
		InitializeWxWidget();
	}
	Window::~Window() = default;

	bool Window::CreateWidget(std::shared_ptr<IWidget> parent, const String& text, Point pos, Size size)
	{
		return Get()->Create(parent ? parent->GetWxWindow() : nullptr, text, pos, size);
	}
}
