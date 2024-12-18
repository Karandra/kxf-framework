#pragma once
#include "kxf/Common.hpp"

namespace kxf::Sciter
{
	enum class FontSmoothing
	{
		None = 0,
		SystemDefault,
		Standard,
		ClearType
	};
	enum class WindowFrame
	{
		Standard = 0,
		Solid,
		SolidWithShadow,
		Extended,
		Transparent
	};
	enum class WindowBlurBehind
	{
		None = 0,
		Auto,

		Light,
		UltraLight,

		Dark,
		UltraDark
	};
	enum class WindowRenderer
	{
		Default = 0,
		DirectX
	};

	enum class ElementState: uint32_t
	{
		None = 0,

		Link = 1 << 0,
		Hover = 1 << 1,
		Active = 1 << 2,
		Focus = 1 << 3,
		Visited = 1 << 4,
		Current = 1 << 5,
		Checked = 1 << 6,
		Disabled = 1 << 7,
		ReadOnly = 1 << 8,
		Expanded = 1 << 9,
		Collapsed = 1 << 10,
		Incomplete = 1 << 11,
		Animating = 1 << 12,
		Focusable = 1 << 12,
		Anchor = 1 << 13,
		Syntetic = 1 << 14,
		OwnsPopup = 1 << 15,
		TabFocus = 1 << 16,
		Empty = 1 << 17,
		Busy = 1 << 18,
		DragOver = 1 << 19,
		DragSource = 1 << 20,
		DropTarget = 1 << 21,
		DropMarker = 1 << 22,
		Moving = 1 << 23,
		Copying = 1 << 24,
		Pressed = 1 << 25,
		Popup = 1 << 26,
		LayoutLTR = 1 << 27,
		LayoutRTL = 1 << 28,
	};
	enum class ElementInnerHTML
	{
		Replace,
		InsertBefore,
		InsertAfter,
	};
	enum class ElementOuterHTML
	{
		Replace,
		InsertBefore,
		InsertAfter,
	};

	enum class SizeUnit
	{
		None = 0,

		// Absolute
		cm, // Centimeter
		mm, // Millimeter
		in, // Inch
		px, // Pixel
		pt, // Point
		pc, // Pica
		dip, // Device-independent pixel

		// Relative
		em, // Relative to the font-size of the element (2em means 2 times the size of the current font)
		ex, // Relative to the x-height of the current font (rarely used)
		ch, // Relative to width of the "0" (zero)
		rem, // Relative to font-size of the root element
		vw, // Relative to 1% of the width of the viewport
		vh, // Relative to 1% of the height of the viewport
		vmin, // Relative to 1% of viewport's smaller dimension
		vmax, // Relative to 1% of viewport's larger dimension
		pr, // %, relative to the parent element
		sp, // %% "springs", a.k.a. flex units
	};

	enum class ScrollSource
	{
		Unknown = 0,
		Keyboard,
		Scrollbar,
		Animator,
		Wheel
	};

	struct TextMetrics final
	{
		double MinWidth = 0;
		double MaxWidth = 0;
		double Height = 0;
		double Ascent = 0;
		double Descent = 0;
		int LineCount = 0;
	};
}

namespace kxf
{
	kxf_FlagSet_Declare(Sciter::ElementState);
}
