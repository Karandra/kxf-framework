#include "kxf-pch.h"
#include "SearchBox.h"
#include "WXUI/SearchBox.h"

namespace kxf::Widgets
{
	// ComboBox
	SearchBox::SearchBox()
	{
		InitializeWxWidget();
	}
	SearchBox::~SearchBox() = default;

	// IWidget
	bool SearchBox::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent && Get()->Create(parent->GetWxWindow(), label, pos, size))
		{
			m_TextEntryWrapper.Initialize(*Get());
			return true;
		}
		return false;
	}

	// ITextEntry
	String SearchBox::GetHint() const
	{
		return Get()->GetDescriptiveText();
	}
	void SearchBox::SetHint(const String& hint)
	{
		Get()->SetDescriptiveText(hint);
	}

	// ISearchWidget
	bool SearchBox::IsSearchButtonVisible() const
	{
		return Get()->IsSearchButtonVisible();
	}
	void SearchBox::SetSearchButtonVisible(bool isVisible)
	{
		Get()->ShowSearchButton(isVisible);
	}

	bool SearchBox::IsCancelButtonVisible() const
	{
		return Get()->IsCancelButtonVisible();
	}
	void SearchBox::SetCancelButtonVisible(bool isVisible)
	{
		Get()->ShowCancelButton(isVisible);
	}
}
