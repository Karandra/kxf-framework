#pragma once
#include "kxf/Common.hpp"
#include "ImageDefines.h"

namespace kxf
{
	class IImage2D;
	class IInputStream;
}

namespace kxf::Drawing
{
	KXF_API void InitalizeImageHandlers();

	KXF_API std::shared_ptr<IImage2D> LoadImage(IInputStream& stream, size_t index = std::numeric_limits<size_t>::max());
}
