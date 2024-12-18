#pragma once
#include "kxf/Common.hpp"
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/fontenc.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf
{
	// https://keithp.com/~keithp/porterduff/p253-porter.pdf
	// The description give a short equation on how the values of a resulting pixel are calculated.
	// R = Result, S = Source, D = Destination, colors premultiplied with alpha Ra, Sa, Da their alpha components
	enum class CompositionMode
	{
		None = -1,

		Clear, // R = 0
		Source, // R = S
		Over, // R = S + D*(1 - Sa)
		In, // R = S*Da
		Out, // R = S*(1 - Da)
		Atop, // R = S*Da + D*(1 - Sa)
		Dest, // R = D (essentially a no-op)
		DestOver, // R = S*(1 - Da) + D
		DestIn, // R = D*Sa
		DestOut, // R = D*(1 - Sa)
		DestAtop, // R = S*(1 - Da) + D*Sa
		Xor, // R = S*(1 - Da) + D*(1 - Sa)
		Add, // R = S + D
	};
	enum class InterpolationQuality
	{
		None = 0,
		FastestAvailable = -1,
		BestAvailable = -2,

		Default = 1,
		NearestNeighbor,
		Bilinear,
		Bicubic,
		BoxAverage
	};
	enum class AntialiasMode
	{
		None = -1,

		Default,
		BestAvailable = -2
	};

	// https://docs.microsoft.com/en-us/windows/win32/api/gdiplusenums/ne-gdiplusenums-hatchstyle
	enum class PolygonFill
	{
		OddEvenRule,
		WindingRule
	};
	enum class FloodFill
	{
		Surface,
		Border
	};
	enum class WrapMode
	{
		None = -1,

		Tile,
		FlipX,
		FlipY,
		FlipBoth
	};
	enum class HatchStyle
	{
		None = -1,

		Horizontal,
		Vertical,
		ForwardDiagonal,
		BackwardDiagonal,
		Cross,
		DiagonalCross,
		Percent05,
		Percent10,
		Percent20,
		Percent25,
		Percent30,
		Percent40,
		Percent50,
		Percent60,
		Percent70,
		Percent75,
		Percent80,
		Percent90,
		LightDownwardDiagonal,
		LightUpwardDiagonal,
		DarkDownwardDiagonal,
		DarkUpwardDiagonal,
		WideDownwardDiagonal,
		WideUpwardDiagonal,
		LightVertical,
		LightHorizontal,
		NarrowVertical,
		NarrowHorizontal,
		DarkVertical,
		DarkHorizontal,
		DashedDownwardDiagonal,
		DashedUpwardDiagonal,
		DashedHorizontal,
		DashedVertical,
		SmallConfetti,
		LargeConfetti,
		ZigZag,
		Wave,
		DiagonalBrick,
		HorizontalBrick,
		Weave,
		Plaid,
		Divot,
		DottedGrid,
		DottedDiamond,
		Shingle,
		Trellis,
		Sphere,
		SmallGrid,
		SmallCheckerBoard,
		LargeCheckerBoard,
		OutlinedDiamond,
		SolidDiamond
	};
	enum class DashStyle
	{
		None = -1,

		Dot,
		Dash,
		DashDot,
		DashDotDot,
		Custom
	};

	enum class PenStyle
	{
		None = -1,

		Solid,
		Hatch,
		Dash,
		Texture
	};
	enum class LineCap
	{
		None = -1,

		Flat,
		Round,
		Square
	};
	enum class LineJoin
	{
		None = -1,

		Miter,
		Bevel,
		Round
	};
	enum class LineAlignment
	{
		Center,
		Inset
	};

	enum class StockPen
	{
		Transparent,

		Black,
		BlackDashed,
		White,
		Cyan,
		Blue,
		Red,
		Green,
		Yellow,
		Gray,
		LightGray,
		MediumGray
	};
	enum class StockBrush
	{
		Transparent,

		Black,
		White,
		Cyan,
		Blue,
		Red,
		Green,
		Yellow,
		Gray,
		LightGray,
		MediumGray
	};
}

namespace kxf
{
	enum class FontEncoding
	{
		None = std::numeric_limits<std::underlying_type_t<FontEncoding>>::max(),
		System = -1,
		Default = 0,

		UTF7 = 42,
		UTF8 = 43,
		UTF16LE = 46,
		UTF16BE = 45,
		UTF32LE = 48,
		UTF32BE = 47

		// TODO: Map this properly
	};
	enum class FontFamily
	{
		None = -1,

		Default,
		Fantasy,
		Serif,
		SansSerif,
		Cursive,
		FixedWidth
	};
	enum class FontWeight
	{
		None = -1,

		Thin = 1,
		ExtraLight,
		Light,
		Normal,
		Medium,
		SemiBold,
		Bold,
		ExtraBold,
		Heavy,
		ExtraHeavy,
	};
	enum class FontSymbolicSize
	{
		Normal = 0,

		ExtraSmall = -3,
		VerySmall = -2,
		Small = -1,

		Large = 1,
		VeryLarge = 2,
		ExtraLarge = 3,
	};
	enum class FontStyle: uint32_t
	{
		Normal = 0,

		Italic = 1 << 1,
		Oblique = 1 << 2,
		Underline = 1 << 3,
		Strikethrough = 1 << 4,
	};
	kxf_FlagSet_Declare(FontStyle);
}

namespace kxf::Drawing
{
	constexpr int InvalidImageIndex = -1;
}
