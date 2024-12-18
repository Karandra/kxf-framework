#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class KXF_API IClusterMapWidget: public RTTI::ExtendInterface<IClusterMapWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(IClusterMapWidget, {0x18940418, 0xd84b, 0x4fd4, {0xa8, 0x5f, 0x9d, 0xd2, 0xbe, 0xaf, 0xd3, 0x6f}});

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		public:
			virtual size_t HitTest(const Point& pos) const = 0;
			virtual Rect GetItemRect(const Point& pos) const = 0;
			virtual Rect GetItemRect(size_t index) const = 0;

			virtual size_t GetItemCount() const = 0;
			virtual void SetItemCount(size_t count) = 0;

			virtual int GetItemSize() const = 0;
			virtual void SetItemSize(int size, int spacing = 0) = 0;
	};
}
