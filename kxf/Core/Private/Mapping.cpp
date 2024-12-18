#include "kxf-pch.h"
#include "Mapping.h"

namespace kxf::Private
{
	FlagSet<wxDirection> MapDirection(FlagSet<Direction> value) noexcept
	{
		FlagSet<wxDirection> wx;
		wx.Add(wxDirection::wxLEFT, value.Contains(Direction::Left));
		wx.Add(wxDirection::wxRIGHT, value.Contains(Direction::Right));
		wx.Add(wxDirection::wxUP, value.Contains(Direction::Up));
		wx.Add(wxDirection::wxDOWN, value.Contains(Direction::Down));

		return wx;
	}
	FlagSet<Direction> MapDirection(FlagSet<wxDirection> wx) noexcept
	{
		FlagSet<Direction> value;
		value.Add(Direction::Left, wx.Contains(wxDirection::wxLEFT));
		value.Add(Direction::Right, wx.Contains(wxDirection::wxRIGHT));
		value.Add(Direction::Up, wx.Contains(wxDirection::wxUP));
		value.Add(Direction::Down, wx.Contains(wxDirection::wxDOWN));

		return value;
	}

	FlagSet<wxOrientation> MapOrientation(FlagSet<Orientation> value) noexcept
	{
		FlagSet<wxOrientation> wx;
		wx.Add(wxOrientation::wxHORIZONTAL, value & Orientation::Horizontal);
		wx.Add(wxOrientation::wxVERTICAL, value & Orientation::Vertical);

		return wx;
	}
	FlagSet<Orientation> MapOrientation(FlagSet<wxOrientation> wx) noexcept
	{
		FlagSet<Orientation> value;
		value.Add(Orientation::Horizontal, value & wxOrientation::wxHORIZONTAL);
		value.Add(Orientation::Vertical, value & wxOrientation::wxVERTICAL);

		return value;
	}

	FlagSet<wxAlignment> MapAlignment(FlagSet<Alignment> value) noexcept
	{
		if (*value == Alignment::Invalid)
		{
			return wxAlignment::wxALIGN_INVALID;
		}
		else if (*value == Alignment::None)
		{
			return wxAlignment::wxALIGN_NOT;
		}
		else
		{
			FlagSet<wxAlignment> wx;
			wx.Add(wxAlignment::wxALIGN_LEFT, value.Contains(Alignment::Left));
			wx.Add(wxAlignment::wxALIGN_RIGHT, value.Contains(Alignment::Right));
			wx.Add(wxAlignment::wxALIGN_TOP, value.Contains(Alignment::Top));
			wx.Add(wxAlignment::wxALIGN_BOTTOM, value.Contains(Alignment::Bottom));
			wx.Add(wxAlignment::wxALIGN_CENTER_VERTICAL, value.Contains(Alignment::CenterVertical));
			wx.Add(wxAlignment::wxALIGN_CENTER_HORIZONTAL, value.Contains(Alignment::CenterHorizontal));

			return wx;
		}
	}
	FlagSet<Alignment> MapAlignment(FlagSet<wxAlignment> wx) noexcept
	{
		if (*wx == wxAlignment::wxALIGN_INVALID)
		{
			return Alignment::Invalid;
		}
		else if (*wx == wxAlignment::wxALIGN_NOT)
		{
			return Alignment::None;
		}
		else
		{
			FlagSet<Alignment> value;
			value.Add(Alignment::Left, wx.Contains(wxAlignment::wxALIGN_LEFT));
			value.Add(Alignment::Right, wx.Contains(wxAlignment::wxALIGN_RIGHT));
			value.Add(Alignment::Top, wx.Contains(wxAlignment::wxALIGN_TOP));
			value.Add(Alignment::Bottom, wx.Contains(wxAlignment::wxALIGN_BOTTOM));
			value.Add(Alignment::CenterVertical, wx.Contains(wxAlignment::wxALIGN_CENTER_VERTICAL));
			value.Add(Alignment::CenterHorizontal, wx.Contains(wxAlignment::wxALIGN_CENTER_HORIZONTAL));

			return value;
		}
	}

	FlagSet<wxEllipsizeFlags> MapEllipsizeFlag(FlagSet<EllipsizeFlag> value) noexcept
	{
		FlagSet<wxEllipsizeFlags> wx;
		wx.Add(wxEllipsizeFlags::wxELLIPSIZE_FLAGS_EXPAND_TABS, value & EllipsizeFlag::ExpandTabs);
		wx.Add(wxEllipsizeFlags::wxELLIPSIZE_FLAGS_PROCESS_MNEMONICS, value & EllipsizeFlag::ProcessMnemonics);

		return wx;
	}
	FlagSet<EllipsizeFlag> MapEllipsizeFlag(FlagSet<wxEllipsizeFlags> wx) noexcept
	{
		FlagSet<EllipsizeFlag> value;
		value.Add(EllipsizeFlag::ExpandTabs, value & wxEllipsizeFlags::wxELLIPSIZE_FLAGS_EXPAND_TABS);
		value.Add(EllipsizeFlag::ProcessMnemonics, value & wxEllipsizeFlags::wxELLIPSIZE_FLAGS_PROCESS_MNEMONICS);

		return value;
	}
}
