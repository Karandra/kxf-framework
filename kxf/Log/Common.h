#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/String.h"

namespace kxf
{
	enum class LogLevel
	{
		FlowControl = -1,
		Disabled = 0,

		Critical,
		Error,
		Warning,
		Information,
		Debug,
		Trace,

		Unknown
	};
}

namespace kxf::Log
{
	KXF_API bool IsEnabled() noexcept;
	KXF_API bool IsLevelEnabled(LogLevel level) noexcept;
	KXF_API void Enable(LogLevel level) noexcept;
}
