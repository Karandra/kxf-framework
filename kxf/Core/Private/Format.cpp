#include "KxfPCH.h"
#include "Format.h"
#include "kxf/Log/ScopedLogger.h"

namespace kxf::Private
{
	void LogFormatterException(const std::format_error& e)
	{
		Log::Error("std::format_error: {}", e.what());
	}
}
