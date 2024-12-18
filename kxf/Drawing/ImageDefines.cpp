#include "kxf-pch.h"
#include "ImageDefines.h"
#include <wx/image.h>

namespace kxf::ImageOption
{
	const XChar* FileName = kxfS("FileName");
	const XChar* Quality = kxfS("quality");

	const XChar* DPI = kxfS("DPI");
	const XChar* Resolution = kxfS("Resolution");
	const XChar* ResolutionX = kxfS("ResolutionX");
	const XChar* ResolutionY = kxfS("ResolutionY");
	const XChar* ResolutionUnit = kxfS("ResolutionUnit");

	const XChar* DesiredWidth = kxfS("MaxWidth");
	const XChar* DesiredHeight = kxfS("MaxHeight");

	const XChar* OriginalWidth = kxfS("OriginalWidth");
	const XChar* OriginalHeight = kxfS("OriginalHeight");

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
		const XChar* CurrentTime = kxfS("CurrentTime");
		const XChar* HasAnimation = kxfS("HasAnimation");
		const XChar* AnimationDuration = kxfS("AnimationDuration");
	}
	namespace TIFF
	{
		const XChar* BitsPerSample = kxfS("BitsPerSample");
		const XChar* SamplesPerPixel = kxfS("SamplesPerPixel");
		const XChar* Compression = kxfS("Compression");
		const XChar* Photometric = kxfS("Photometric");
		const XChar* ImageDescriptor = kxfS("ImageDescriptor");
	}
	namespace Cursor
	{
		const XChar* HotSpotX = wxIMAGE_OPTION_CUR_HOTSPOT_X;
		const XChar* HotSpotY = wxIMAGE_OPTION_CUR_HOTSPOT_Y;
	}
}
