#include "kxf-pch.h"
#include "Drawing.h"

#include <Windows.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::Utility
{
	void FromWindowsRect(const tagRECT& winRect, Rect& rect) noexcept
	{
		rect.X() = winRect.left;
		rect.Y() = winRect.top;
		rect.Width() = winRect.right - winRect.left;
		rect.Height() = winRect.bottom - winRect.top;
	}
	Rect FromWindowsRect(const tagRECT& winRect) noexcept
	{
		Rect rect;
		FromWindowsRect(winRect, rect);
		return rect;
	}

	void ToWindowsRect(const Rect& rect, tagRECT& winRect) noexcept
	{
		winRect.left = rect.GetX();
		winRect.top = rect.GetY();
		winRect.right = rect.GetX() + rect.GetWidth();
		winRect.bottom = rect.GetY() + rect.GetHeight();
	}
	tagRECT ToWindowsRect(const Rect& rect) noexcept
	{
		tagRECT winRect;
		ToWindowsRect(rect, winRect);
		return winRect;
	}
}
