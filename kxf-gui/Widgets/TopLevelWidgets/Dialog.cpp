#include "kxf-pch.h"
#include "Dialog.h"
#include "WXUI/Dialog.h"

namespace kxf::Widgets
{
	Dialog::Dialog()
	{
		InitializeWxWidget();
	}
	Dialog::~Dialog() = default;

	bool Dialog::CreateWidget(std::shared_ptr<IWidget> parent, const String& text, Point pos, Size size)
	{
		return Get()->Create(parent ? parent->GetWxWindow() : nullptr, text, pos, size);
	}
}
