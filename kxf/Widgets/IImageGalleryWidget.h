#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class KXF_API IImageGalleryWidget: public RTTI::ExtendInterface<IImageGalleryWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(IImageGalleryWidget, {0x4ed09bab, 0xf44e, 0x4d2e, {0x9e, 0xc0, 0x5c, 0xf6, 0x2d, 0x52, 0xf3, 0xd}});

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		public:
			virtual size_t GetItemCount() const = 0;
			virtual size_t AddItem(const IImage2D& image) = 0;
			virtual void RemoveItem(size_t index) = 0;
			virtual void ClearItems() = 0;

			virtual size_t GetSelectedItem() const = 0;
			virtual void SetSelectedItem(size_t index) = 0;

			virtual Size GetItemSize() const = 0;
			virtual void SetItemSize(const Size& size) = 0;

			virtual Size GetSpacing() const = 0;
			virtual void SetSpacing(const Size& spacing) = 0;
	};
}
