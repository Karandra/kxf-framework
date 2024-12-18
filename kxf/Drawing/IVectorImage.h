#pragma once
#include "Common.h"
#include "IImage2D.h"

namespace kxf
{
	class KXF_API IVectorImage: public RTTI::ExtendInterface<IVectorImage, IImage2D>
	{
		kxf_RTTI_DeclareIID(IVectorImage, {0x650f73df, 0x1ba0, 0x4da5, {0xbf, 0x83, 0xe4, 0xe8, 0x51, 0x84, 0xe, 0xc6}});

		public:
			virtual ~IVectorImage() = default;

		public:
			virtual Rect GetBoundingBox() const = 0;
	};
}
