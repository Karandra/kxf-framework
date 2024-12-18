#include "kxf-pch.h"
#include "BitmapComboBox.h"

#include <CommCtrl.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(BitmapComboBox, wxBitmapComboBox);

	bool BitmapComboBox::Create(wxWindow* parent,
								wxWindowID id,
								const String& value,
								const Point& pos,
								const Size& size,
								FlagSet<ComboBoxStyle> style,
								const wxValidator& validator
	)
	{
		if (wxBitmapComboBox::Create(parent, id, value, pos, size, 0, nullptr, style.ToInt(), validator))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);

			return true;
		}
		return false;
	}

	void BitmapComboBox::SetVisibleItemsCount(size_t count)
	{
		::SendMessageW(GetHandle(), CB_SETMINVISIBLE, static_cast<WPARAM>(count), 0);
	}
	int BitmapComboBox::InsertItem(const String& label, size_t index, int imageID)
	{
		int newIndex = wxNOT_FOUND;
		if (HasImageList() && imageID != Drawing::InvalidImageIndex)
		{
			newIndex = wxBitmapComboBox::Insert(label, GetImageList()->GetBitmap(imageID).ToWxBitmap(), index);
		}
		else
		{
			newIndex = wxBitmapComboBox::Insert(label, wxNullBitmap, index);
		}

		if (newIndex != wxNOT_FOUND)
		{
			m_ImageIDs[newIndex] = imageID;
		}
		return newIndex;
	}
	int BitmapComboBox::AddItem(const String& label, int imageID)
	{
		return InsertItem(label, GetCount(), imageID);
	}
	void BitmapComboBox::RemoveItem(size_t index)
	{
		wxBitmapComboBox::Delete(index);
		m_ImageIDs.erase(index);
	}
	void BitmapComboBox::Clear()
	{
		wxBitmapComboBox::Clear();
		m_ImageIDs.clear();
	}

	int BitmapComboBox::GetItemImage(size_t index) const
	{
		if (HasImageList() && m_ImageIDs.count(index))
		{
			return m_ImageIDs.at(index);
		}
		return Drawing::InvalidImageIndex;
	}
	void BitmapComboBox::SetItemImage(size_t index, int imageID)
	{
		if (HasImageList())
		{
			if (imageID != Drawing::InvalidImageIndex)
			{
				m_ImageIDs[index] = imageID;
				wxBitmapComboBox::SetItemBitmap(index, GetImageList()->GetBitmap(imageID).ToWxBitmap());
			}
			else
			{
				m_ImageIDs.erase(index);
				wxBitmapComboBox::SetItemBitmap(index, wxNullBitmap);
			}
		}
	}
}
