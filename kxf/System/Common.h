#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/String.h"

namespace kxf
{
	enum class UserNameFormat
	{
		None = -1,

		Display,
		UniqueID,
		DownLevel,
		Canonical,
		CanonicalEx,
		UserPrincipal,
		ServicePrincipal,
		FullyQualifiedDN
	};

	enum class SHWindowCommand: uint32_t
	{
		None = 0,

		// Command
		Show = 1 << 0,
		Hide = 1 << 1,
		Minimize = 1 << 2,
		Maximize = 1 << 3,
		Restore = 1 << 4,

		// Modifiers
		Force = 1 << 16,
		Default = 1 << 17,
		Inactive = 1 << 18,
	};
	kxf_FlagSet_Declare(SHWindowCommand);

	enum class SystemStandardAccess: uint32_t
	{
		None = 0,

		Delete = 1 << 0,
		Synchronize = 1 << 1,
		ReadControl = 1 << 2,
		WriteOwner = 1 << 3,
		WriteDAC = 1 << 4,

		Everything = std::numeric_limits<uint32_t>::max()
	};
	kxf_FlagSet_Declare(SystemStandardAccess);

	enum class SystemThreadAccess: uint32_t
	{
		None = 0,

		QueryInformation = 1 << 0,
		QueryLimitedInformation = 1 << 1,
		SetInformation = 1 << 2,
		SetLimitedInformation = 1 << 3,
		SuspendResume = 1 << 4,
		Synchronize = 1 << 5,
		Terminate = 1 << 6,
		GetContext = 1 << 7,
		SetContext = 1 << 8,
		SetToken = 1 << 9,
		Impersonate = 1 << 10,
		DirectImpersonation = 1 << 11,

		Everything = std::numeric_limits<uint32_t>::max()
	};
	kxf_FlagSet_Declare(SystemThreadAccess);

	enum class SystemProcessAccess: uint32_t
	{
		None = 0,

		CreateProcess = 1 << 0,
		CreateThread = 1 << 1,
		QueryInformation = 1 << 2,
		QueryLimitedInformation = 1 << 3,
		SetInformation = 1 << 4,
		SuspendResume = 1 << 5,
		Synchronize = 1 << 6,
		Terminate = 1 << 7,
		VMOperation = 1 << 8,
		VMRead = 1 << 9,
		VMWrite = 1 << 10,
		SetQuota = 1 << 11,
		DuplicateHandle = 1 << 12,

		Everything = std::numeric_limits<uint32_t>::max()
	};
	kxf_FlagSet_Declare(SystemProcessAccess);

	enum class CreateSystemProcessFlag: uint32_t
	{
		None = 0,

		WaitInputIdle = 1 << 0,
		WaitTermination = 1 << 1,
		CreateSuspended = 1 << 2,
		ToggleElevation = 1 << 3,
		RedirectStdIO = 1 << 4,

		Async = 1 << 16
	};
	kxf_FlagSet_Declare(CreateSystemProcessFlag);

	enum class SystemProcessPriority
	{
		None = -1,

		Idle,
		BelowNormal,
		Normal,
		AboveNormal,
		High,
		Realtime
	};

	enum class KernelObjectNamespace
	{
		None = -1,

		Local,
		Global
	};
}
