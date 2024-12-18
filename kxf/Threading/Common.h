#pragma once
#include "kxf/Common.hpp"

namespace kxf::Threading
{
	KXF_API bool IsMainThread() noexcept;
	KXF_API uint32_t GetCurrentThreadID() noexcept;
	KXF_API uint32_t GetHardwareConcurrency() noexcept;
}
