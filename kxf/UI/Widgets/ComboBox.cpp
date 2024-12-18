#include "kxf-pch.h"
#include "ComboBox.h"
#include "WXUI/ComboBox.h"

namespace kxf::Widgets
{
	// ComboBox
	ComboBox::ComboBox()
	{
		InitializeWxWidget();
	}
	ComboBox::~ComboBox() = default;

	// IWidget
	bool ComboBox::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent && Get()->Create(parent->GetWxWindow(), label, pos, size))
		{
			m_TextEntryWrapper.Initialize(*Get());
			return true;
		}
		return false;
	}

	// IComboBoxWidget
	void ComboBox::ShowDropdown()
	{
		Get()->Popup();
	}
	void ComboBox::DismissDropdown()
	{
		Get()->Dismiss();
	}

	size_t ComboBox::InsertItem(size_t index, const String& label, void* data)
	{
		const auto count = Get()->GetCount();
		int newIndex = Get()->Insert(label, index <= count ? index : count - 1, data);

		return newIndex >= 0 ? newIndex : npos;
	}
	void ComboBox::RemoveItem(size_t index)
	{
		Get()->Delete(index);
	}
	void ComboBox::ClearItems()
	{
		Get()->Clear();
	}

	String ComboBox::GetItemLabel(size_t index) const
	{
		return Get()->GetString(index);
	}
	void ComboBox::SetItemLabel(size_t index, const String& label)
	{
		Get()->SetString(index, label);
	}

	void* ComboBox::GetItemData(size_t index) const
	{
		return Get()->GetClientData(index);
	}
	void ComboBox::SetItemData(size_t index, void* data)
	{
		Get()->SetClientData(index, data);
	}

	size_t ComboBox::GetItemCount() const
	{
		return Get()->GetCount();
	}
	void ComboBox::SetVisibleItemCount(size_t count)
	{
		Get()->SetVisibleItemCount(count);
	}

	size_t ComboBox::GetSelectedItem() const
	{
		auto index = Get()->GetSelection();
		return index >= 0 ? index : npos;
	}
	void ComboBox::SetSelectedItem(size_t index)
	{
		Get()->SetSelection(index != npos ? index : -1);
	}
}
