#include "kxf-pch.h"
#include "HResult.h"
#include "Win32Error.h"
#include "NtStatus.h"
#include "COM.h"
#include "Private/System.h"
#include "Private/ErrorCode.h"

namespace kxf
{
	// HResult
	HResult HResult::Success() noexcept
	{
		return S_OK;
	}
	HResult HResult::False() noexcept
	{
		return S_FALSE;
	}
	HResult HResult::Fail() noexcept
	{
		return E_FAIL;
	}
	HResult HResult::Abort() noexcept
	{
		return E_ABORT;
	}
	HResult HResult::Pending() noexcept
	{
		return E_PENDING;
	}
	HResult HResult::Unexpected() noexcept
	{
		return E_UNEXPECTED;
	}
	HResult HResult::OutOfMemory() noexcept
	{
		return E_OUTOFMEMORY;
	}
	HResult HResult::OutOfRange() noexcept
	{
		return E_BOUNDS;
	}
	HResult HResult::AccessDenied() noexcept
	{
		return E_ACCESSDENIED;
	}
	HResult HResult::NoInterface() noexcept
	{
		return E_NOINTERFACE;
	}
	HResult HResult::NotImplemented() noexcept
	{
		return E_NOTIMPL;
	}
	HResult HResult::StateChnaged() noexcept
	{
		return E_CHANGED_STATE;
	}
	HResult HResult::InvalidArgument() noexcept
	{
		return E_INVALIDARG;
	}
	HResult HResult::InvalidPointer() noexcept
	{
		return E_POINTER;
	}
	HResult HResult::InvalidHandle() noexcept
	{
		return E_HANDLE;
	}
	HResult HResult::InvalidStateChnage() noexcept
	{
		return E_ILLEGAL_STATE_CHANGE;
	}
	HResult HResult::InvalidMethodCall() noexcept
	{
		return E_ILLEGAL_METHOD_CALL;
	}

	HResult::HResult() noexcept
		:m_Value(E_FAIL)
	{
	}
	HResult::HResult(int32_t value, IErrorInfo* errorInfo) noexcept
		:m_Value(value), m_ErrorInfo(errorInfo)
	{
		if (m_ErrorInfo)
		{
			m_ErrorInfo->AddRef();
		}
	}
	HResult::~HResult()
	{
		if (m_ErrorInfo)
		{
			m_ErrorInfo->Release();
		}
	}

	// IErrorCode
	bool HResult::IsSuccess() const noexcept
	{
		return SUCCEEDED(GetValue());
	}

	String HResult::ToString() const
	{
		// TODO: Add HRESULT error code to its constant name mapping
		return {};
	}
	String HResult::GetMessage(const Locale& locale) const
	{
		return System::Private::FormatMessage(nullptr, m_Value, FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_MAX_WIDTH_MASK, locale);
	}

	// HResult
	String HResult::GetSource() const
	{
		_bstr_t result = _com_error(m_Value, m_ErrorInfo).Source();
		return static_cast<const XChar*>(result);
	}
	String HResult::GetHelpFile() const
	{
		_bstr_t result = _com_error(m_Value, m_ErrorInfo).HelpFile();
		return static_cast<const XChar*>(result);
	}
	uint32_t HResult::GetHelpContext() const noexcept
	{
		return _com_error(m_Value, m_ErrorInfo).HelpContext();
	}
	String HResult::GetDescription() const
	{
		_bstr_t result = _com_error(m_Value, m_ErrorInfo).Description();
		return static_cast<const XChar*>(result);
	}
	uint32_t HResult::GetFacility() const noexcept
	{
		return HRESULT_FACILITY(static_cast<HRESULT>(m_Value));
	}
	UniversallyUniqueID HResult::GetUniqueID() const noexcept
	{
		GUID result = _com_error(m_Value, m_ErrorInfo).GUID();
		return COM::FromGUID(result);
	}

	std::optional<Win32Error> HResult::ToWin32() const noexcept
	{
		return System::Private::Win32FromHRESULT(m_Value);
	}
	std::optional<NtStatus> HResult::ToNtStatus() const noexcept
	{
		if (auto win32Error = System::Private::Win32FromHRESULT(m_Value))
		{
			return System::Private::NtStatusFromWin32(win32Error->GetValue());
		}
		return {};
	}
}
