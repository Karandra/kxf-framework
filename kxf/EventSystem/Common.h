#pragma once
#include "kxf/Common.hpp"
#include <wx/event.h>
#include <wx/windowid.h>

namespace kxf
{
	enum class EventCategory: uint32_t
	{
		None = 0,

		UI = wxEVT_CATEGORY_UI,
		Timer = wxEVT_CATEGORY_TIMER,
		Thread = wxEVT_CATEGORY_THREAD,
		Socket = wxEVT_CATEGORY_SOCKET,
		Unknown = wxEVT_CATEGORY_UNKNOWN,
		Clipboard = wxEVT_CATEGORY_CLIPBOARD,
		UserInput = wxEVT_CATEGORY_USER_INPUT,

		Everything = UI|Timer|Thread|Socket|Unknown|Clipboard|UserInput
	};
	kxf_FlagSet_Declare(EventCategory);

	enum class BindEventFlag: uint32_t
	{
		None = 0,

		Direct = 1,
		Queued = 6,
		Auto = 12,

		Unique = 1 << 16,
		Blocking = 1 << 17,
		AlwaysSkip = 1 << 18,
		OneShot = 1 << 19
	};
	kxf_FlagSet_Declare(BindEventFlag);

	enum class BindSignalFlag: uint32_t
	{
		None = 0,

		EventContext = 1 << 0
	};
	kxf_FlagSet_Declare(BindSignalFlag);

	enum class ProcessEventFlag: uint32_t
	{
		None = 0,

		// Try to process the event in this handler and all those chained to it
		Locally = 1 << 0,

		// Processes an event and handles any exceptions that occur in the process
		HandleExceptions = 1 << 1
	};
	kxf_FlagSet_Declare(ProcessEventFlag);

	enum class SignalParametersSemantics
	{
		Copy,
		Move
	};
}
