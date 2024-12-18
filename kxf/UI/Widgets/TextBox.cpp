#include "kxf-pch.h"
#include "TextBox.h"
#include "WXUI/TextBox.h"

namespace kxf::Widgets
{
	// ComboBox
	TextBox::TextBox()
	{
		InitializeWxWidget();
	}
	TextBox::~TextBox() = default;

	// IWidget
	bool TextBox::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent && Get()->Create(parent->GetWxWindow(), label, pos, size))
		{
			m_TextCtrlWrapper.Initialize(*Get());
			return true;
		}
		return false;
	}

	Size TextBox::GetTextExtent() const
	{
		return Size(Get()->GetTextExtent(Get()->GetValue()));
	}
	Size TextBox::GetTextExtent(const String& text) const
	{
		return Size(Get()->GetTextExtent(text));
	}
}
