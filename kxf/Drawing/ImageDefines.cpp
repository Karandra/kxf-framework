#include "kxf-pch.h"
#include "ImageDefines.h"
#include <wx/image.h>

namespace kxf::ImageOption
{
	const XChar* FileName = kxS("FileName");
	const XChar* Quality = kxS("quality");

	const XChar* DPI = kxS("DPI");
	const XChar* Resolution = kxS("Resolution");
	const XChar* ResolutionX = kxS("ResolutionX");
	const XChar* ResolutionY = kxS("ResolutionY");
	const XChar* ResolutionUnit = kxS("ResolutionUnit");

	const XChar* DesiredWidth = kxS("MaxWidth");
	const XChar* DesiredHeight = kxS("MaxHeight");

	const XChar* OriginalWidth = kxS("OriginalWidth");
	const XChar* OriginalHeight = kxS("OriginalHeight");

	namespace PNG
	{
		const XChar* Format = wxIMAGE_OPTION_PNG_FORMAT;
		const XChar* BitDepth = wxIMAGE_OPTION_PNG_BITDEPTH;
		const XChar* Filter = wxIMAGE_OPTION_PNG_FILTER;

		const XChar* CompressionLevel = wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL;
		const XChar* CompressionMemoryLevel = wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL;
		const XChar* CompressionStrategy = wxIMAGE_OPTION_PNG_COMPRESSION_STRATEGY;
		const XChar* CompressionBufferSize = wxIMAGE_OPTION_PNG_COMPRESSION_BUFFER_SIZE;
	}
	namespace GIF
	{
		const XChar* Comment = wxIMAGE_OPTION_GIF_COMMENT;
		const XChar* Transparency = wxIMAGE_OPTION_GIF_TRANSPARENCY;
		const XChar* TransparencyHighlight = wxIMAGE_OPTION_GIF_TRANSPARENCY_HIGHLIGHT;
		const XChar* TransparencyUnchanged = wxIMAGE_OPTION_GIF_TRANSPARENCY_UNCHANGED;
	}
	namespace SVG
	{
		const XChar* CurrentTime = kxS("CurrentTime");
		const XChar* HasAnimation = kxS("HasAnimation");
		const XChar* AnimationDuration = kxS("AnimationDuration");
	}
	namespace TIFF
	{
		const XChar* BitsPerSample = kxS("BitsPerSample");
		const XChar* SamplesPerPixel = kxS("SamplesPerPixel");
		const XChar* Compression = kxS("Compression");
		const XChar* Photometric = kxS("Photometric");
		const XChar* ImageDescriptor = kxS("ImageDescriptor");
	}
	namespace Cursor
	{
		const XChar* HotSpotX = wxIMAGE_OPTION_CUR_HOTSPOT_X;
		const XChar* HotSpotY = wxIMAGE_OPTION_CUR_HOTSPOT_Y;
	}
}
