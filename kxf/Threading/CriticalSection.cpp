#include "kxf-pch.h"
#include "CriticalSection.h"
#include <Windows.h>

namespace kxf
{
	CriticalSection::CriticalSection() noexcept
	{
		m_CritSec.Construct();
		::InitializeCriticalSection(&m_CritSec);
	}
	CriticalSection::CriticalSection(uint32_t spinCount) noexcept
	{
		m_CritSec.Construct();
		::InitializeCriticalSectionAndSpinCount(&m_CritSec, spinCount);
	}
	CriticalSection::~CriticalSection() noexcept
	{
		::DeleteCriticalSection(&m_CritSec);
		m_CritSec.Destroy();
	}

	void CriticalSection::Enter() noexcept
	{
		::EnterCriticalSection(&m_CritSec);
	}
	bool CriticalSection::TryEnter() noexcept
	{
		return ::TryEnterCriticalSection(&m_CritSec);
	}
	void CriticalSection::Leave() noexcept
	{
		::LeaveCriticalSection(&m_CritSec);
	}

	uint32_t CriticalSection::SetSpinCount(uint32_t spinCount) noexcept
	{
		return ::SetCriticalSectionSpinCount(&m_CritSec, spinCount);
	}
}
