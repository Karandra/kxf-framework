#include "kxf-pch.h"
#include "Win32Error.h"
#include "NtStatus.h"
#include "HResult.h"
#include "Private/ErrorCode.h"
#include "Private/System.h"

namespace kxf
{
	// Win32Error
	Win32Error Win32Error::Success() noexcept
	{
		return ERROR_SUCCESS;
	}
	Win32Error Win32Error::Fail() noexcept
	{
		return ERROR_UNHANDLED_ERROR;
	}

	Win32Error Win32Error::GetLastError() noexcept
	{
		return ::GetLastError();
	}
	void Win32Error::SetLastError(Win32Error error) noexcept
	{
		::SetLastError(*error);
	}

	Win32Error::Win32Error() noexcept
		:m_Value(ERROR_UNHANDLED_ERROR)
	{
	}

	// IErrorCode
	String Win32Error::ToString() const
	{
		// TODO: Add Win32 error code to its constant name mapping
		return {};
	}
	String Win32Error::GetMessage(const Locale& locale) const
	{
		return System::Private::FormatMessage(nullptr, m_Value, FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_MAX_WIDTH_MASK, locale);
	}

	// Win32Error
	std::optional<HResult> Win32Error::ToHResult() const noexcept
	{
		return HRESULT_FROM_WIN32(GetValue());
	}
	std::optional<NtStatus> Win32Error::ToNtStatus() const noexcept
	{
		return System::Private::NtStatusFromWin32(m_Value);
	}
}
