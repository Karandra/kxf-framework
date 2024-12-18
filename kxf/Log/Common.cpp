#include "kxf-pch.h"
#include "Common.h"
#include "ScopedLogger.h"

namespace kxf::Log
{
	bool IsEnabled() noexcept
	{
		return ScopedLoggerGlobalContext::GetInstance().GetLogLevel() != LogLevel::Disabled;
	}
	bool IsLevelEnabled(LogLevel level) noexcept
	{
		return ScopedLoggerGlobalContext::GetInstance().CanLogLevel(level);
	}
	void Enable(LogLevel level) noexcept
	{
		ScopedLoggerGlobalContext::GetInstance().SetLogLevel(level);
	}
}
