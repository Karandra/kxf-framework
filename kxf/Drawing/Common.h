#pragma once
#include "kxf/Common.hpp"
#include "Private/Defines.h"

namespace kxf
{
	class GDIBitmap;
	class GDICursor;
	class GDIIcon;

	class IImage2D;
	class SVGImage;
	class BitmapImage;

	class IInputStream;
	class IOutputStream;
}

namespace kxf::Drawing
{
	KXF_API void InitalizeImageHandlers();

	KXF_API std::unique_ptr<IImage2D> LoadImage(IInputStream& stream, size_t index = std::numeric_limits<size_t>::max());
}
