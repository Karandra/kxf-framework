#include "kxf-pch.h"
#include "ErrorCode.h"
#include "../HResult.h"
#include "../NtStatus.h"
#include "../Win32Error.h"
#include "kxf/Utility/ScopeGuard.h"

namespace kxf::System::Private
{
	std::optional<Win32Error> Win32FromNtStatus(NTSTATUS ntStatus) noexcept
	{
		// https://stackoverflow.com/questions/25566234/how-to-convert-specific-ntstatus-value-to-the-hresult
		// https://stuff.mit.edu/afs/sipb/project/wine/src/wine-0.9.37/dlls/ntdll/error.c

		if (ntStatus == STATUS_SUCCESS)
		{
			return Win32Error(ERROR_SUCCESS);
		}
		else if (NativeAPI::NtDLL::RtlNtStatusToDosError)
		{
			const ULONG win32Code = NativeAPI::NtDLL::RtlNtStatusToDosError(ntStatus);
			if (win32Code != ERROR_MR_MID_NOT_FOUND)
			{
				return Win32Error(win32Code);
			}
		}

		// Using 'GetOverlappedResult' hack
		OVERLAPPED overlapped = {};
		overlapped.Internal = ntStatus;
		overlapped.InternalHigh = 0;
		overlapped.Offset = 0;
		overlapped.OffsetHigh = 0;
		overlapped.hEvent = nullptr;

		// Remember current error and reset it
		Utility::ScopeGuard atExit = ([previousError = ::GetLastError()]()
		{
			::SetLastError(previousError);
		});
		::SetLastError(ERROR_SUCCESS);

		DWORD bytes = 0;
		::GetOverlappedResult(nullptr, &overlapped, &bytes, FALSE);

		const DWORD result = ::GetLastError();
		if (result != ERROR_SUCCESS)
		{
			return Win32Error(result);
		}
		return {};
	}
	std::optional<Win32Error> Win32FromHRESULT(HRESULT hresult) noexcept
	{
		if (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0) == static_cast<HRESULT>(hresult & 0xFFFF0000))
		{
			// Could have come from many values, but we choose this one
			return Win32Error(HRESULT_CODE(hresult));
		}
		else if (hresult == S_OK)
		{
			return Win32Error(HRESULT_CODE(hresult));
		}
		else
		{
			// Otherwise, we got an impossible value
			return {};
		}
	}
	std::optional<NtStatus> NtStatusFromWin32(DWORD win32Code) noexcept
	{
		auto ntStatus = [win32Code]() -> std::optional<NTSTATUS>
		{
			switch (win32Code)
			{
				#include "ErrorCodeNtStatus.i"
			};
			return {};
		}();

		if (ntStatus)
		{
			return NtStatus(*ntStatus);
		}
		return {};
	}
}
