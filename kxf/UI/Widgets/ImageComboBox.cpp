#include "kxf-pch.h"
#include "ImageComboBox.h"
#include "WXUI/ImageComboBox.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"

namespace kxf::Widgets
{
	// ComboBox
	ImageComboBox::ImageComboBox()
	{
		InitializeWxWidget();
	}
	ImageComboBox::~ImageComboBox() = default;

	// IWidget
	bool ImageComboBox::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent && Get()->Create(parent->GetWxWindow(), label, pos, size))
		{
			m_TextEntryWrapper.Initialize(*Get());
			return true;
		}
		return false;
	}

	// IComboBoxWidget
	void ImageComboBox::ShowDropdown()
	{
		Get()->Popup();
	}
	void ImageComboBox::DismissDropdown()
	{
		Get()->Dismiss();
	}

	size_t ImageComboBox::InsertItem(size_t index, const String& label, void* data)
	{
		const auto count = Get()->GetCount();
		int newIndex = Get()->Insert(label, {}, index <= count ? index : count - 1, data);

		return newIndex >= 0 ? newIndex : npos;
	}
	void ImageComboBox::RemoveItem(size_t index)
	{
		Get()->Delete(index);
	}
	void ImageComboBox::ClearItems()
	{
		Get()->Clear();
	}

	String ImageComboBox::GetItemLabel(size_t index) const
	{
		return Get()->GetString(index);
	}
	void ImageComboBox::SetItemLabel(size_t index, const String& label)
	{
		Get()->SetString(index, label);
	}

	void* ImageComboBox::GetItemData(size_t index) const
	{
		return Get()->GetClientData(index);
	}
	void ImageComboBox::SetItemData(size_t index, void* data)
	{
		Get()->SetClientData(index, data);
	}

	size_t ImageComboBox::GetItemCount() const
	{
		return Get()->GetCount();
	}
	void ImageComboBox::SetVisibleItemCount(size_t count)
	{
		Get()->SetVisibleItemCount(count);
	}

	size_t ImageComboBox::GetSelectedItem() const
	{
		auto index = Get()->GetSelection();
		return index >= 0 ? index : npos;
	}
	void ImageComboBox::SetSelectedItem(size_t index)
	{
		Get()->SetSelection(index != npos ? index : -1);
	}

	// IImageComboBoxWidget
	size_t ImageComboBox::InsertItem(size_t index, const String& label, const BitmapImage& image, void* data)
	{
		const auto count = Get()->GetCount();
		int newIndex = Get()->Insert(label, image.ToGDIBitmap().ToWxBitmap(), index <= count ? index : count - 1, data);

		return newIndex >= 0 ? newIndex : npos;
	}

	BitmapImage ImageComboBox::GetItemImage(size_t index) const
	{
		return GDIBitmap(Get()->GetItemBitmap(index));
	}
	void ImageComboBox::SetItemImage(size_t index, const BitmapImage& image)
	{
		Get()->SetItemBitmap(index, image.ToGDIBitmap().ToWxBitmap());
	}
}
