#include "kxf-pch.h"
#include "NativeEncodingConverter.h"
#include <Windows.h>

namespace kxf
{
	// IEncodingConverter
	size_t NativeEncodingConverter::ToMultiByteBuffer(std::span<const wchar_t> source, std::span<std::byte> destination)
	{
		m_LastError = Win32Error::Success();

		if (m_CodePage >= 0)
		{
			int length = ::WideCharToMultiByte(static_cast<uint32_t>(m_CodePage),
											   m_Flags.ToInt(),
											   source.data(), source.size(),
											   reinterpret_cast<char*>(destination.data()), destination.size(),
											   nullptr,
											   nullptr);

			if (length == 0)
			{
				m_LastError = Win32Error::GetLastError();
			}
			return length;
		}

		m_LastError = ERROR_INVALID_PARAMETER;
		return 0;
	}
	size_t NativeEncodingConverter::ToWideCharBuffer(std::span<const std::byte> source, std::span<wchar_t> destination)
	{
		m_LastError = Win32Error::Success();

		if (m_CodePage >= 0)
		{
			int length = ::MultiByteToWideChar(static_cast<uint32_t>(m_CodePage),
											   m_Flags.ToInt(),
											   reinterpret_cast<const char*>(source.data()), source.size(),
											   destination.data(), destination.size());

			if (length == 0)
			{
				m_LastError = Win32Error::GetLastError();
			}
			return length;
		}

		m_LastError = ERROR_INVALID_PARAMETER;
		return 0;
	}

	String NativeEncodingConverter::GetEncodingName() const
	{
		CPINFOEXW codePageInfo = {};
		if (m_CodePage >= 0 && ::GetCPInfoExW(static_cast<uint32_t>(m_CodePage), 0, &codePageInfo))
		{
			return codePageInfo.CodePageName;
		}
		return {};
	}
}
