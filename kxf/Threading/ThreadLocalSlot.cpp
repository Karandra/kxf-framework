#include "kxf-pch.h"
#include "ThreadLocalSlot.h"

#include <Windows.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::Threading
{
	uint32_t TLSTraits::GetInvalidIndex() noexcept
	{
		return TLS_OUT_OF_INDEXES;
	}
	uint32_t TLSTraits::Initialize() noexcept
	{
		return ::TlsAlloc();
	}
	bool TLSTraits::Uninitialize(uint32_t index) noexcept
	{
		return ::TlsFree(index);
	}
	void* TLSTraits::GetValue(uint32_t index) noexcept
	{
		return ::TlsGetValue(index);
	}
	bool TLSTraits::SetValue(uint32_t index, void* ptr) noexcept
	{
		return ::TlsSetValue(index, ptr);
	}
}

namespace kxf::Threading
{
	uint32_t FLSTraits::GetInvalidIndex() noexcept
	{
		return FLS_OUT_OF_INDEXES;
	}
	uint32_t FLSTraits::Initialize() noexcept
	{
		return ::FlsAlloc(nullptr);
	}
	uint32_t FLSTraits::Initialize(CallbackType* callback) noexcept
	{
		return ::FlsAlloc(callback);
	}
	bool FLSTraits::Uninitialize(uint32_t index) noexcept
	{
		return ::FlsFree(index);
	}
	void* FLSTraits::GetValue(uint32_t index) noexcept
	{
		return ::FlsGetValue(index);
	}
	bool FLSTraits::SetValue(uint32_t index, void* ptr) noexcept
	{
		return ::FlsSetValue(index, ptr);
	}
}
