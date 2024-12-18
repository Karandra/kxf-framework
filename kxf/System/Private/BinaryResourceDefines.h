#pragma once
#include <Windows.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::System::Private
{
	#pragma pack(push)
	#pragma pack(2)
	struct IconGroupEntry final
	{
		BYTE bWidth; // Width, in pixels, of the image
		BYTE bHeight; // Height, in pixels, of the image
		BYTE bColorCount; // Number of colors in image (0 if >=8bpp)
		BYTE bReserved; // Reserved
		WORD wPlanes; // Color Planes
		WORD wBitCount; // Bits per pixel
		DWORD dwBytesInRes; // how many bytes in this resource?
		WORD id; // the ID
	};
	#pragma pack(pop)

	#pragma pack(push)
	#pragma pack(2)
	struct IconGroupDirectory final
	{
		WORD idReserved; // Reserved (must be 0)
		WORD idType; // Resource type (1 for icons)
		WORD idCount; // How many images?
		IconGroupEntry idEntries[1]; // The entries for each image
	};
	#pragma pack(pop)
}
