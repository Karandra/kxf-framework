#include "kxf-pch.h"
#include "NtStatus.h"
#include "Win32Error.h"
#include "HResult.h"
#include "Private/ErrorCode.h"
#include "Private/System.h"

namespace kxf
{
	// NtStatus
	NtStatus NtStatus::Success() noexcept
	{
		return STATUS_SUCCESS;
	}
	NtStatus NtStatus::Fail() noexcept
	{
		// There isn't any generic fail code but this should suffice
		return STATUS_UNSUCCESSFUL;
	}

	NtStatus NtStatus::GetLastError() noexcept
	{
		if (NativeAPI::NtDLL::RtlGetLastNtStatus)
		{
			return NativeAPI::NtDLL::RtlGetLastNtStatus();
		}
		return STATUS_PROCEDURE_NOT_FOUND;
	}
	void NtStatus::SetLastError(NtStatus error) noexcept
	{
		if (NativeAPI::NtDLL::RtlSetLastWin32ErrorAndNtStatusFromNtStatus)
		{
			return NativeAPI::NtDLL::RtlSetLastWin32ErrorAndNtStatusFromNtStatus(*error);
		}
	}

	NtStatus::NtStatus() noexcept
		:m_Value(STATUS_UNSUCCESSFUL)
	{
	}

	// IErrorCode
	bool NtStatus::IsSuccess() const noexcept
	{
		return System::Private::NT_SUCCESS(m_Value);
	}

	String NtStatus::ToString() const
	{
		// TODO: Add NTSTATUS error code to its constant name mapping
		return {};
	}
	String NtStatus::GetMessage(const Locale& locale) const
	{
		return System::Private::FormatMessage(::GetModuleHandleW(L"NtDLL.dll"), m_Value, FORMAT_MESSAGE_FROM_HMODULE, locale);
	}
	
	uint32_t NtStatus::GetFacility() const noexcept
	{
		return System::Private::NT_FACILITY(m_Value);
	}
	bool NtStatus::IsError() const noexcept
	{
		return System::Private::NT_ERROR(m_Value);
	}
	bool NtStatus::IsWarning() const noexcept
	{
		return System::Private::NT_WARNING(m_Value);
	}
	bool NtStatus::IsInformation() const noexcept
	{
		return System::Private::NT_INFORMATION(m_Value);
	}

	// NtStatus
	std::optional<Win32Error> NtStatus::ToWin32() const noexcept
	{
		return System::Private::Win32FromNtStatus(m_Value);
	}
	std::optional<HResult> NtStatus::ToHResult() const noexcept
	{
		if (auto win32Error = System::Private::Win32FromNtStatus(m_Value))
		{
			return HRESULT_FROM_WIN32(win32Error->GetValue());
		}
		return {};
	}
}
