#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/FlagSet.h"

namespace kxf
{
	enum class UnitNameFlag: uint32_t
	{
		None = 0,

		Abbreviated = FlagSetValue<UnitNameFlag>(0),
		GenetiveCase = FlagSetValue<UnitNameFlag>(1)
	};
	kxf_FlagSet_Declare(UnitNameFlag);

	enum class ClockDirection
	{
		Clockwise = 1,
		CounterClockwise = -1
	};

	// Keyboard NumPad layout
	enum class CornerAlignment
	{
		None = 0,

		TopLeft = 7,
		TopCenter = 8,
		TopRight = 9,

		MiddleLeft = 4,
		MiddleCenter = 5,
		MiddleRight = 6,

		BottomLeft = 1,
		BottomCenter = 2,
		BottomRight = 3,
	};

	enum class EllipsizeMode
	{
		None = 0,

		Start,
		Middle,
		End
	};

	enum class EllipsizeFlag: uint32_t
	{
		None = 0,

		ExpandTabs = FlagSetValue<EllipsizeFlag>(0),
		ProcessMnemonics = FlagSetValue<EllipsizeFlag>(1)
	};
	kxf_FlagSet_Declare(EllipsizeFlag);

	enum class Orientation: uint32_t
	{
		None = 0,

		Horizontal = FlagSetValue<Orientation>(0),
		Vertical = FlagSetValue<Orientation>(1),
		Both = Horizontal|Vertical
	};
	kxf_FlagSet_Declare(Orientation);

	enum class Alignment: uint32_t
	{
		None = 0,
		Invalid = std::numeric_limits<std::underlying_type_t<Alignment>>::max(),

		Left = None,
		Right = FlagSetValue<Alignment>(1),
		Top = None,
		Bottom = FlagSetValue<Alignment>(3),

		CenterVertical = FlagSetValue<Alignment>(7),
		CenterHorizontal = FlagSetValue<Alignment>(8),
		Center = CenterVertical|CenterHorizontal,
	};
	kxf_FlagSet_Declare(Alignment);

	enum class Direction: uint32_t
	{
		None = 0,

		Left = FlagSetValue<Direction>(0),
		Right = FlagSetValue<Direction>(1),
		Up = FlagSetValue<Direction>(2),
		Down = FlagSetValue<Direction>(3),

		All = Left|Right|Up|Down
	};
	kxf_FlagSet_Declare(Direction);

	enum class LineBreakFormat
	{
		None = -1,

		LF,
		CRLF,
		CR,

		Unix = LF,
		Windows = CRLF,
		Macintosh = CR
	};

	enum class MemoryProtection: uint32_t
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		Execute = 1 << 2,
		CopyOnWrite = 1 << 3,

		RW = Read|Write,
		RX = Read|Execute,
		RWX = Read|Write|Execute,
	};
	kxf_FlagSet_Declare(MemoryProtection);

	enum class SortOrder
	{
		None = -1,
		Ascending,
		Descending
	};
}
