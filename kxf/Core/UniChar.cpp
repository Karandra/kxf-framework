#pragma once
#include "kxf-pch.h"
#include "UniChar.h"

#include <Windows.h>
#include "kxf/System/UndefWindows.h"

namespace kxf
{
	UniChar UniChar::ToLowerCase() const noexcept
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<wchar_t>(::CharLowerW(reinterpret_cast<LPWSTR>(m_Value)));
	}
	UniChar UniChar::ToUpperCase() const noexcept
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<wchar_t>(::CharUpperW(reinterpret_cast<LPWSTR>(m_Value)));
	}

	std::strong_ordering UniChar::CompareNoCase(const UniChar& other) const noexcept
	{
		if (m_Value == other.m_Value)
		{
			return std::strong_ordering::equal;
		}
		else
		{
			return ToLowerCase().m_Value <=> other.ToLowerCase().m_Value;
		}
	}
}
