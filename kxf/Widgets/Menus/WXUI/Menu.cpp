#include "kxf-pch.h"
#include "Menu.h"
#include "../MenuWidget.h"

namespace kxf::WXUI
{
	Menu::Menu(Widgets::MenuWidget& widget) noexcept
		:m_Widget(widget)
	{
	}
	Menu::~Menu()
	{
		m_Widget.OnWXMenuDestroyed();
	}
}
