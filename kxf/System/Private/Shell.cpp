#include "kxf-pch.h"
#include "Shell.h"

#include <Windows.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::Shell::Private
{
	std::optional<uint32_t> MapSHWindowCommand(FlagSet<SHWindowCommand> option) noexcept
	{
		// Show
		if (option & SHWindowCommand::Show)
		{
			if (option & SHWindowCommand::Default)
			{
				return SW_SHOWDEFAULT;
			}
			else if (option & SHWindowCommand::Minimize)
			{
				if (option & SHWindowCommand::Inactive)
				{
					return SW_SHOWMINNOACTIVE;
				}
				return SW_SHOWMINIMIZED;
			}
			else if (option & SHWindowCommand::Maximize)
			{
				return SW_SHOWMAXIMIZED;
			}
			else if (option & SHWindowCommand::Inactive)
			{
				// Or should I use 'SW_SHOWNA'? I don't really see the difference.
				return SW_SHOWNOACTIVATE;
			}
			return SW_SHOWNORMAL;
		}

		// Hide
		if (option & SHWindowCommand::Hide)
		{
			return SW_HIDE;
		}

		// Maximize
		if (option & SHWindowCommand::Minimize)
		{
			if (option & SHWindowCommand::Force)
			{
				return SW_FORCEMINIMIZE;
			}
			return SW_MINIMIZE;
		}

		// Maximize
		if (option & SHWindowCommand::Maximize)
		{
			return SW_MAXIMIZE;
		}

		// Restore
		if (option & SHWindowCommand::Restore)
		{
			return SW_RESTORE;
		}
		return {};
	}
	FlagSet<SHWindowCommand> MapSHWindowCommand(uint32_t option) noexcept
	{
		switch (option)
		{
			case SW_SHOWNORMAL:
			{
				return SHWindowCommand::Show;
			}
			case SW_SHOWDEFAULT:
			{
				return SHWindowCommand::Show|SHWindowCommand::Default;
			}
			case SW_SHOWMINIMIZED:
			{
				return SHWindowCommand::Show|SHWindowCommand::Minimize;
			}
			case SW_SHOWMINNOACTIVE:
			{
				return SHWindowCommand::Show|SHWindowCommand::Minimize|SHWindowCommand::Inactive;
			}
			case SW_SHOWMAXIMIZED:
			{
				return SHWindowCommand::Show|SHWindowCommand::Maximize;
			}
			case SW_SHOWNA:
			case SW_SHOWNOACTIVATE:
			{
				return SHWindowCommand::Show|SHWindowCommand::Inactive;
			}
			case SW_HIDE:
			{
				return SHWindowCommand::Hide;
			}
			case SW_MINIMIZE:
			{
				return SHWindowCommand::Minimize;
			}
			case SW_FORCEMINIMIZE:
			{
				return SHWindowCommand::Minimize|SHWindowCommand::Force;
			}
			case SW_RESTORE:
			{
				return SHWindowCommand::Restore;
			}
		}
		return SHWindowCommand::None;
	}
}
