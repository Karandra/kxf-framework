#include "kxf-pch.h"
#include "WxOverride.h"
#include "../Categories.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/wxWidgets/StringFormatters.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/Utility/Common.h"

namespace
{
	kxf::LogLevel MapLogLevel(wxLogLevel level)
	{
		using namespace kxf;

		switch (level)
		{
			case wxLOG_Trace:
			{
				return LogLevel::Trace;
			}
			case wxLOG_Debug:
			{
				return LogLevel::Debug;
			}
			case wxLOG_Info:
			case wxLOG_Status:
			case wxLOG_Message:
			case wxLOG_Progress:
			{
				return LogLevel::Information;
			}
			case wxLOG_Warning:
			{
				return LogLevel::Warning;
			}
			case wxLOG_Error:
			{
				return LogLevel::Error;
			}
			case wxLOG_FatalError:
			{
				return LogLevel::Critical;
			}
		};
		return LogLevel::Unknown;
	}
}

namespace kxf::Log
{
	void WxOverride::Install()
	{
		auto oldTarget = Utility::AssumeUniquePtr(wxLog::SetActiveTarget(new WxOverride()));

		wxSetAssertHandler([](const wxString& file, int line, const wxString& func, const wxString& condition, const wxString& message)
		{
			if (ScopedLoggerAutoScope logger; true)
			{
				 logger.Debug().Format("[wxWidgets] Assestion failed [File={}:{}], [Function={}], [Condition={}], [Message={}]", file, line, func, condition, message);
			}
			if (auto app = ICoreApplication::GetInstance())
			{
				app->OnAssertFailure(file, line, func, condition, message);
			}
		});
	}
	void WxOverride::DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info)
	{
		ScopedMessageLogger logger;
		logger.SetSeparator(", ");

		logger.Format("[wxWidgets] [Thread: {}]", info.threadId).Sep();
		if (info.filename)
		{
			logger.Format("[File: {}:{}]", info.filename, info.line).Sep();
		}
		if (info.component)
		{
			logger.Format("[Component: {}]", info.component).Sep();
		}
		if (!msg.IsEmpty())
		{
			logger.Format("[Message: {}]", msg);
		}
		ScopedLoggerAutoScope().GetTLS().Write(MapLogLevel(level), DateTime().SetValue(info.timestampMS), logger.ToString(), LogCategory::wxWidgets);
	}

	void WxOverride::Flush()
	{
		ScopedLoggerAutoScope().GetTLS().Flush();
	}
}
