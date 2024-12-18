#pragma once
#include "kxf/Common.hpp"
#include "kxf/Drawing/Geometry.h"
struct tagRECT;

namespace kxf::Utility
{
	KXF_API void FromWindowsRect(const tagRECT& winRect, Rect& rect) noexcept;
	KXF_API Rect FromWindowsRect(const tagRECT& winRect) noexcept;

	KXF_API void ToWindowsRect(const Rect& rect, tagRECT& winRect) noexcept;
	KXF_API tagRECT ToWindowsRect(const Rect& rect) noexcept;
}
