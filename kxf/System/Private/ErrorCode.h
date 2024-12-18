#pragma once
#include "../Common.h"
#include "../NativeAPI.h"

#include <Windows.h>
#include "IncludeNtStatus.h"

#include "BeginIncludeCOM.h"
namespace
{
	kxf_MakeWinUnicodeCallWrapper(FormatMessage);
}

#include "EndIncludeCOM.h"

namespace kxf
{
	class HResult;
	class NtStatus;
	class Win32Error;
}

namespace kxf::System::Private
{
	// Inclusion of 'NtDef.h' causes too many compilation errors
	constexpr bool NT_SUCCESS(NTSTATUS status) noexcept
	{
		return status >= 0;
	}
	constexpr bool NT_INFORMATION(NTSTATUS status) noexcept
	{
		return static_cast<ULONG>(status >> 30) == STATUS_SEVERITY_INFORMATIONAL;
	}
	constexpr bool NT_WARNING(NTSTATUS status) noexcept
	{
		return static_cast<ULONG>(status >> 30) == STATUS_SEVERITY_WARNING;
	}
	constexpr bool NT_ERROR(NTSTATUS status) noexcept
	{
		return static_cast<ULONG>(status >> 30) == STATUS_SEVERITY_ERROR;
	}
	constexpr uint32_t NT_FACILITY(NTSTATUS status) noexcept
	{
		return status & 0xFFF0000u;
	}

	std::optional<Win32Error> Win32FromNtStatus(NTSTATUS ntStatus) noexcept;
	std::optional<Win32Error> Win32FromHRESULT(HRESULT hresult) noexcept;
	std::optional<NtStatus> NtStatusFromWin32(DWORD win32Code) noexcept;
}
