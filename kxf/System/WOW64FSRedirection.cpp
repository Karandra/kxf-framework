#include "kxf-pch.h"
#include "WOW64FSRedirection.h"
#include "NativeAPI.h"

namespace kxf
{
	DisableWOW64FSRedirection::DisableWOW64FSRedirection() noexcept
	{
		if (NativeAPI::Kernel32::Wow64DisableWow64FsRedirection)
		{
			if (NativeAPI::Kernel32::Wow64DisableWow64FsRedirection(&m_Value))
			{
				m_ErrorCode = Win32Error::Success();
			}
			else
			{
				m_ErrorCode = Win32Error::GetLastError();
			}
		}
		else
		{
			m_ErrorCode = ERROR_INVALID_FUNCTION;
		}
	}
	DisableWOW64FSRedirection::~DisableWOW64FSRedirection() noexcept
	{
		if (NativeAPI::Kernel32::Wow64RevertWow64FsRedirection && m_ErrorCode.IsSuccess())
		{
			NativeAPI::Kernel32::Wow64RevertWow64FsRedirection(&m_Value);
		}
	}
}
