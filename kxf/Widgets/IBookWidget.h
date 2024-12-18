#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class KXF_API IBookWidget: public RTTI::ExtendInterface<IBookWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(IBookWidget, {0x8aecb241, 0xef0e, 0x4d69, {0x9c, 0x42, 0x52, 0x91, 0x27, 0xc, 0xfe, 0xa8}});

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		public:
			virtual size_t InsertPage(IWidget& widget, const String& label, size_t index = npos) = 0;
			virtual void RemovePage(size_t index) = 0;
			virtual std::shared_ptr<IWidget> GetPage(size_t index) const = 0;
			virtual size_t GetPageCount() const = 0;

			virtual String GetPageLabel(size_t index) const = 0;
			virtual void SetPageLabel(size_t index, const String& label) const = 0;

			virtual BitmapImage GetPageImage(size_t index) const = 0;
			virtual void SetPageImage(size_t index, const BitmapImage& image) = 0;

			virtual size_t GetSelectedPage() const = 0;
			virtual size_t SetSelectedPage(size_t index) = 0;

			virtual std::shared_ptr<IWidget> GetSelectedWidget() const = 0;
			virtual size_t GetSelectedPageByWidget(const IWidget& widget) const = 0;
	};
}
