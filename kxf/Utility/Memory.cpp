#include "kxf-pch.h"
#include "Memory.h"
#include <Windows.h>

namespace kxf::Utility
{
	void SecureZeroMemory(void* ptr, size_t size) noexcept
	{
		::RtlSecureZeroMemory(ptr, size);
	}
}
