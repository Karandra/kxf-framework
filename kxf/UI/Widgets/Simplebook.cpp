#include "kxf-pch.h"
#include "Simplebook.h"
#include "WXUI/Simplebook.h"

namespace kxf::Widgets
{
	// Simplebook
	Simplebook::Simplebook()
	{
		InitializeWxWidget();
	}
	Simplebook::~Simplebook() = default;

	// IWidget
	bool Simplebook::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent && Get()->Create(parent->GetWxWindow(), label, pos, size))
		{
			m_BookCtrlWrapper.Initialize(*Get());
			return true;
		}
		return false;
	}
}
