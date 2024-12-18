#pragma once
#include "Common.h"

namespace kxf
{
	enum class StdID: intptr_t
	{
		// wxWidgets IDs
		WX_AUTO_LOWEST = -32000,
		WX_AUTO_HIGHEST = -2000,

		// All predefined ids are between wxID_LOWEST and wxID_HIGHEST
		WX_LOWEST = 4999,

		// Highest wxWidgets generic ID
		WX_HIGHEST = 5999,

		// No ID matches this one when compared to it
		None = -3,

		// ID for a separator line in the menu (invalid for normal item)
		Separator = -2,

		// Any ID means that we don't care about the ID, whether when installing an event handler or when creating a new window.
		Any = -1,

		// Regular IDs
		Open = WX_LOWEST + 1,
		Close,
		New,
		Save,
		SaveAs,
		Revert,
		Exit,
		Undo,
		Redo,
		Help,
		Print,
		PrintSetup,
		PageSetup,
		Preview,
		About,
		HelpContents,
		HelpIndex,
		HelpSearch,
		HelpCommands,
		HelpProcedures,
		HelpContext,
		CloseAll,
		Preferences,

		Edit = 5030,
		Cut,
		Copy,
		Paste,
		Clear,
		Find,
		Duplicate,
		SelectAll,
		Delete,
		Replace,
		ReplaceAll,
		Properties,

		ViewDetails,
		ViewLargeIcons,
		ViewSmallIcons,
		ViewList,
		ViewSortDate,
		ViewSortName,
		ViewSortSize,
		ViewSortType,

		File,
		File1,
		File2,
		File3,
		File4,
		File5,
		File6,
		File7,
		File8,
		File9,
		File10,

		// Standard button and menu IDs
		OK = 5100,
		Cancel,
		Apply,
		Yes,
		No,
		Static,
		Forward,
		Backward,
		Default,
		More,
		Setup,
		Reset,
		ContextHelp,
		YesToAll,
		NoToAll,
		Abort,
		Retry,
		Ignore,
		Add,
		Remove,

		Up,
		Down,
		Home,
		Refresh,
		Stop,
		Index,

		Bold,
		Italic,
		JustifyCenter,
		JustifyFill,
		JustifyRight,
		JustifyLeft,
		Underline,
		Indent,
		Unindent,
		Zoom100,
		ZoomFit,
		ZoomIn,
		ZoomOut,
		Undelete,
		RevertToSaved,
		CDROM,
		Convert,
		Execute,
		Floppy,
		HardDisk,
		Bottom,
		First,
		Last,
		Top,
		Info,
		JumpTo,
		Network,
		SelectColor,
		SelectFont,
		SortAscending,
		SortDescending,
		SpellCheck,
		Strikethrough,

		SystemMenu = 5200,
		CloseFrame,
		MoveFrame,
		ResizeFrame,
		MaximizeFrame,
		MinimizeFrame,
		RestoreFrame,

		MDIWindowFirst = 5230,
		MDIWindowCascade = MDIWindowFirst,
		MDIWindowTileHorizontal,
		MDIWindowTileVertical,
		MDIWindowArrangeIcons,
		MDIWindowPrevious,
		MDIWindowNext,
		MDIWindowLast = 5235,

		// IDs used by generic file dialog (13 consecutive starting from this value)
		FileDialog = 5900,

		// IDs used by generic file control (4 consecutive starting from this value)
		FileControl = 5950,

		// KxFramework IDs
		KX_LOWEST = WX_HIGHEST + 1,

		Move,
		Error,
		Rename,
		Directory,
		SelectFile,
		SelectDirectory,
		Search,
		CopyLink,
		CopyAddress,

		KX_HIGHEST
	};
}
