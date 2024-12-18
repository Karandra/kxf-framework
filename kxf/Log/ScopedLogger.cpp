#include "kxf-pch.h"
#include "ScopedLogger.h"
#include "ScopedLoggerTarget.h"
#include "Private/WxOverride.h"
#include "kxf/System/DynamicLibrary.h"

namespace
{
	kxf::String FormatTimestamp(kxf::DateTime timestamp, const kxf::TimeZoneOffset& tzOffset)
	{
		return timestamp.Format("%Y-%m-%d/%H:%M:%S.%l", tzOffset);
	}
	std::string_view FormatLogLevel(kxf::LogLevel logLevel)
	{
		using namespace kxf;

		switch (logLevel)
		{
			case LogLevel::FlowControl:
			{
				return "FlowControl";
			}
			case LogLevel::Trace:
			{
				return "Trace";
			}
			case LogLevel::Debug:
			{
				return "Debug";
			}
			case LogLevel::Unknown:
			case LogLevel::Information:
			{
				return "Information";
			}
			case LogLevel::Warning:
			{
				return "Warning";
			}
			case LogLevel::Error:
			{
				return "Error";
			}
			case LogLevel::Critical:
			{
				return "Critical";
			}
		};
		return {};
	}
}

namespace kxf
{
	String ToString(LogLevel value)
	{
		return FormatLogLevel(value);
	}

	ScopedLoggerGlobalContext& ScopedLoggerGlobalContext::Initialize(std::shared_ptr<IScopedLoggerContext> userContext, LogLevel logLevel)
	{
		static ScopedLoggerGlobalContext globalContext(userContext, logLevel);
		if (globalContext.IsInitialized() && userContext)
		{
			std::shared_ptr<IScopedLoggerContext> expected;
			if (globalContext.m_UserContext.compare_exchange_strong(expected, std::move(userContext)))
			{
				LogLevel expectedLevel = LogLevel::Unknown;
				globalContext.m_LogLevel.compare_exchange_strong(expectedLevel, logLevel);

				globalContext.OnUserContextUpdated();
			}
		}

		return globalContext;
	}
	ScopedLoggerGlobalContext& ScopedLoggerGlobalContext::GetInstance()
	{
		return Initialize(nullptr);
	}

	bool ScopedLoggerGlobalContext::IsInitialized() const noexcept
	{
		return !m_TLSIndex.IsNull();
	}
	void ScopedLoggerGlobalContext::Initialize()
	{
		m_TLSIndex.Initialize([](void* ptr)
		{
			if (auto tls = Utility::AssumeUniquePtr(static_cast<ScopedLoggerTLS*>(ptr)))
			{
				tls->OnTermination();
				tls->Destroy();
			}
		});
		if (!m_TLSIndex)
		{
			throw std::runtime_error(__FUNCTION__ ": Failed to initialize thread local slot");
		}

		m_TimeOffset = TimeZone::Local;
		m_UnknownContextTLS.emplace(*this);

		Log::WxOverride::Install();
	}
	void ScopedLoggerGlobalContext::Destroy()
	{
		m_UnknownContextTLS.reset();
		m_TLSIndex.Uninitialize();
	}

	void ScopedLoggerGlobalContext::OnUserContextUpdated()
	{
		m_UpdateUserContext = true;
		m_UnknownContextTLS->UpdateUserContext(m_UserContext);
	}

	ScopedLoggerTLS* ScopedLoggerGlobalContext::QueryThreadContext() noexcept
	{
		if (void* ptr = m_TLSIndex.GetValue())
		{
			return static_cast<ScopedLoggerTLS*>(ptr);
		}
		return nullptr;
	}
	ScopedLoggerTLS& ScopedLoggerGlobalContext::GetThreadContext()
	{
		if (auto ptr = QueryThreadContext())
		{
			if (m_UpdateUserContext)
			{
				ptr->UpdateUserContext(m_UserContext);
			}
			return *ptr;
		}
		else
		{
			auto tls = std::make_unique<ScopedLoggerTLS>(*this);
			tls->Initialize();

			ptr = tls.release();
			m_TLSIndex.SetValue(ptr);
			return *ptr;
		}
	}
	ScopedLoggerTLS& ScopedLoggerGlobalContext::GetUnknownThreadContext() noexcept
	{
		return *m_UnknownContextTLS;
	}
	bool ScopedLoggerGlobalContext::CanLogLevel(LogLevel logLevel) const noexcept
	{
		if (logLevel == LogLevel::Disabled)
		{
			return false;
		}
		else if (logLevel == LogLevel::FlowControl)
		{
			return true;
		}
		else
		{
			if (logLevel == LogLevel::Unknown)
			{
				logLevel = LogLevel::Information;
			}
			return ToInt(logLevel) <= ToInt(m_LogLevel.load());
		}
	}
}

namespace kxf
{
	void ScopedLoggerTLS::Initialize()
	{
		m_Process = SystemProcess::GetCurrentProcess();
		m_Thread = SystemThread::GetCurrentThread();
		m_TimeStamp = DateTime::Now();
		InitializeUserData(m_GlobalContext.GetUserContext());

		LogOpenClose(true);
	}
	void ScopedLoggerTLS::Destroy()
	{
		LogOpenClose(false);

		if (auto logTarget = m_LogTarget.load())
		{
			logTarget->Flush();
			logTarget = nullptr;
		}
	}

	void ScopedLoggerTLS::LogOpenClose(bool open)
	{
		if (open)
		{
			ScopedMessageLogger message(LogLevel::FlowControl);
			message.SetSeparator('/');

			if (IsUnknown())
			{
				message << "Log opened for " << m_Process.GetExecutablePath().GetName();
				message.Sep();
			}
			else
			{
				message << "Log opened for thread " << m_Thread.GetID();
				message.Sep();

				if (auto description = m_Thread.GetDescription(); !description.IsEmpty())
				{
					message << description;
					message.Sep();
				}
			}
			message << DynamicLibrary::GetCurrentModule().GetFilePath().GetName();

			message.Write(*this);
		}
		else
		{
			ScopedMessageLogger message(LogLevel::FlowControl);
			message.SetSeparator('/');

			if (IsUnknown())
			{
				message << "Log closed for " << m_Process.GetExecutablePath().GetName();
				message.Sep();
			}
			else
			{
				message << "Log closed for thread " << m_Thread.GetID();
				message.Sep();
			}
			message << DynamicLibrary::GetCurrentModule().GetFilePath().GetName();

			message.Write(*this);
		}
	}
	String ScopedLoggerTLS::FormatRecord(LogLevel logLevel, DateTime timestamp, StringView message, StringView category) const
	{
		String buffer;
		buffer.reserve(255);

		// Log time
		buffer.Format("[{}]", FormatTimestamp(timestamp, m_GlobalContext.GetTimeOffset()));

		// Log location
		bool isUnknown = IsUnknown();
		buffer.Format("[PID:{:0>6}|{}:{:0>6}]",
					  m_Process.GetID(),
					  isUnknown ? "UNK" : "TID",
					  isUnknown ? SystemThread::GetCurrentThread().GetID() : m_Thread.GetID()
		);

		// Log message level
		buffer.Format("[{:<11}]", FormatLogLevel(logLevel));

		// Add indents
		buffer.Append(' ', m_ScopeLevel * 4);

		// Add category if present
		if (!category.empty())
		{
			buffer.Format(" <{}>", category);
		}

		// Log the actual message string
		buffer += ' ';
		buffer += message;

		return buffer;
	}
	void ScopedLoggerTLS::InitializeUserData(std::shared_ptr<IScopedLoggerContext> userContext)
	{
		if (userContext)
		{
			m_UserContextRef = userContext;
			m_LogTarget = userContext->CreateLogTarget(*this);
		}
		else
		{
			m_UserContextRef.store({});
			m_LogTarget = nullptr;
		}
	}
	void ScopedLoggerTLS::UpdateUserContext(std::shared_ptr<IScopedLoggerContext> userContext)
	{
		auto weak = m_UserContextRef.load();
		if (weak.expired() || weak.lock() != userContext)
		{
			InitializeUserData(std::move(userContext));
		}
	}

	void ScopedLoggerTLS::Flush()
	{
		if (auto logTarget = m_LogTarget.load())
		{
			logTarget->Flush();
		}
	}
	void ScopedLoggerTLS::Write(LogLevel logLevel, DateTime timestamp, StringView message, StringView category)
	{
		if (!message.empty() && m_GlobalContext.CanLogLevel(logLevel))
		{
			if (auto logTarget = m_LogTarget.load())
			{
				String formatted = logTarget->FormatRecord(*this, logLevel, timestamp, message, category);
				if (formatted.IsEmpty())
				{
					formatted = FormatRecord(logLevel, timestamp, message, category);
				}

				logTarget->Write(logLevel, formatted.view());
			}
		}
	}
}

namespace kxf
{
	void ScopedLogger::OnEnter(std::optional<StringView> serializedParameters)
	{
		if (CanEnterLeave())
		{
			if (auto logger = FlowControl(); true)
			{
				logger.Log("Enter: ", m_Function);
				if (serializedParameters.has_value())
				{
					logger.Format(" /. ({})", *serializedParameters);
				}
			}
			m_ScopeTLS.OnScopeEnter(*this);
		}
	}
	void ScopedLogger::OnLeave()
	{
		if (CanEnterLeave())
		{
			m_ScopeTLS.OnScopeLeave(*this);

			auto funcName = m_Function.view();
			if (auto pos = funcName.find('('); pos != funcName.npos)
			{
				funcName.remove_suffix(funcName.length() - pos);
			}

			if (auto logger = FlowControl(); true)
			{
				logger.Log("Leave: ");
				if (!m_IsSuccess)
				{
					logger << "<FAILED> ";
				}
				logger << funcName;
				if (!m_IsVoid && !m_ReturnValue.empty())
				{
					logger << " -> " << m_ReturnValue;
				}
			}
		}
	}
}

namespace kxf
{
	ScopedLoggerTLS& ScopedLoggerNewScope::GetTLS()
	{
		return ScopedLoggerGlobalContext::GetInstance().GetThreadContext();
	}

	ScopedLoggerTLS& ScopedLoggerAutoScope::GetActiveTLS()
	{
		auto& global = ScopedLoggerGlobalContext::GetInstance();
		if (auto tls = global.QueryThreadContext())
		{
			return *tls;
		}
		else
		{
			return global.GetUnknownThreadContext();
		}
	}

	void ScopedLoggerUnknownScope::Initialize()
	{
		m_Function = "<unknown context>()";
		m_IsVoid = true;
		m_IsSuccess = true;
	}
	void ScopedLoggerUnknownScope::Destroy()
	{
		m_Function.clear();
	}

	void ScopedLoggerUnknownTLS::Initialize()
	{
		m_Process = SystemProcess::GetCurrentProcess();
		m_Thread = {};
		m_TimeStamp = DateTime::Now();
		InitializeUserData(m_GlobalContext.GetUserContext());
		m_Scope.Initialize();

		LogOpenClose(true);
	}
	void ScopedLoggerUnknownTLS::Destroy()
	{
		m_Scope.Destroy();
		ScopedLoggerTLS::Destroy();
	}
}

namespace kxf
{
	void ScopedMessageLogger::Write()
	{
		if (m_Scope)
		{
			Write(m_Scope->GetTLS());
		}
	}
}
