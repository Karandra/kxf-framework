#pragma once
#include "Common.h"
#include "Geometry.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IImage2D;
	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	class KXF_API IImageHandler: public RTTI::Interface<IImageHandler>
	{
		kxf_RTTI_DeclareIID(IImageHandler, {0xb19d91e1, 0x716d, 0x43b4, {0xba, 0xc5, 0x6f, 0x3e, 0xf3, 0x71, 0x21, 0x15}});

		public:
			virtual ~IImageHandler() = default;

		public:
			virtual std::shared_ptr<IImage2D> CreateImage() = 0;

			virtual size_t GetSubImageCount(IInputStream& stream) = 0;
			virtual bool LoadImage(IImage2D& image, IInputStream& stream, size_t index) = 0;
			virtual bool SaveImage(const IImage2D& image, IOutputStream& stream) const = 0;
	};
}
