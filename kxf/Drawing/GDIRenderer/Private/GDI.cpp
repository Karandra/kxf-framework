#include "kxf-pch.h"
#include "GDI.h"
#include "../GDIBitmap.h"
#include <Windows.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::Drawing::Private
{
	GDIBitmap BitmapFromMemoryLocation(const void* data)
	{
		const BITMAPINFO& bitmapInfo = *static_cast<const BITMAPINFO*>(data);
		const BITMAPINFOHEADER& bitmapHeader = bitmapInfo.bmiHeader;

		// We don't support other color depths here
		const ColorDepth colorDepth = bitmapHeader.biBitCount;
		if (colorDepth == ColorDepthDB::BPP24 || colorDepth == ColorDepthDB::BPP32)
		{
			// TODO: Check the correctness of this pointer
			const auto bitmapData = reinterpret_cast<const uint8_t*>(data) + sizeof(BITMAPINFO);

			GDIBitmap bitmap;
			bitmap.AttachHandle(::CreateBitmap(bitmapHeader.biWidth, bitmapHeader.biHeight, bitmapHeader.biPlanes, bitmapHeader.biBitCount, bitmapData));
			return bitmap;
		}
		return {};
	}
}
