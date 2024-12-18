#pragma once
#include "Common.h"
#include "IWidget.h"
#include "IComboBoxWidget.h"

namespace kxf
{
	class KXF_API IImageComboBoxWidget: public RTTI::ExtendInterface<IImageComboBoxWidget, IComboBoxWidget>
	{
		kxf_RTTI_DeclareIID(IImageComboBoxWidget, {0x4d3ddc9f, 0xa781, 0x4686, {0x96, 0xf2, 0x8e, 0x7b, 0xc9, 0x9b, 0x53, 0x2b}});

		public:
			using IComboBoxWidget::InsertItem;
			virtual size_t InsertItem(size_t index, const String& label, const BitmapImage& image, void* data = nullptr) = 0;

			using IComboBoxWidget::AddItem;
			size_t AddItem(const String& label, const BitmapImage& image, void* data = nullptr)
			{
				return InsertItem(npos, label, image, data);
			}

			virtual BitmapImage GetItemImage(size_t index) const = 0;
			virtual void SetItemImage(size_t index, const BitmapImage& image) = 0;
	};
}
