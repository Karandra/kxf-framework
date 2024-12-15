#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/Core/String.h"
#include "kxf/Core/DateTime.h"
#include "kxf/System/SystemProcess.h"
#include "kxf/System/SystemThread.h"
#include "kxf/Threading/ThreadLocalSlot.h"
#include "kxf/Utility/Common.h"
#include <source_location>
#include <atomic>
#include <vector>
#include <map>

namespace kxf
{
	class ScopedLoggerTLS;
	class ScopedLoggerUnknownTLS;
	class ScopedLoggerNewScope;
	class ScopedLoggerGlobalContext;
	class ScopedMessageLogger;
}

namespace kxf
{
	class IScopedLoggerTarget: public RTTI::Interface<IScopedLoggerTarget>
	{
		KxRTTI_DeclareIID(IScopedLoggerTarget, {0xddebd088, 0x6198, 0x46aa, {0x9f, 0xe9, 0xc2, 0xda, 0xc5, 0xb4, 0x60, 0x9a}});

		public:
			virtual ~IScopedLoggerTarget() = default;

		public:
			virtual void Write(LogLevel logLevel, StringView str) = 0;
			virtual void Flush() = 0;

			virtual String FormatRecord(const ScopedLoggerTLS& tls, LogLevel logLevel, DateTime timestamp, StringView message, StringView category = {}) const
			{
				return {};
			}
	};

	class IScopedLoggerContext: public RTTI::Interface<IScopedLoggerContext>
	{
		KxRTTI_DeclareIID(IScopedLoggerContext, {0x7dd1c550, 0xe0a1, 0x4f92, {0x8c, 0x3c, 0x3f, 0xd8, 0x46, 0x3c, 0x80, 0xd2 }});

		public:
			virtual ~IScopedLoggerContext() = default;

		public:
			virtual std::shared_ptr<IScopedLoggerTarget> CreateLogTarget(ScopedLoggerTLS& tls) = 0;
	};

}

namespace kxf
{
	class ScopedLoggerTLS
	{
		friend class ScopedLogger;
		friend class ScopedLoggerGlobalContext;

		protected:
			ScopedLoggerGlobalContext& m_GlobalContext;
			std::atomic<std::weak_ptr<IScopedLoggerContext>> m_UserContextRef;
			std::atomic<std::shared_ptr<IScopedLoggerTarget>> m_LogTarget;
			std::vector<ScopedLogger*> m_ScopeStack;
			SystemProcess m_Process;
			SystemThread m_Thread;
			DateTime m_TimeStamp;
			size_t m_ScopeLevel = 0;
			bool m_Terminated = false;

		protected:
			void Initialize();
			void Destroy();

			void LogOpenClose(bool open);
			String FormatRecord(LogLevel logLevel, DateTime timestamp, StringView str, StringView category) const;
			void InitializeUserData(std::shared_ptr<IScopedLoggerContext> userContext);

			void OnScopeEnter(ScopedLogger& scope)
			{
				m_ScopeLevel++;
				m_ScopeStack.emplace_back(&scope);
			}
			void OnScopeLeave(ScopedLogger& scope)
			{
				m_ScopeLevel--;
				m_ScopeStack.pop_back();
			}
			void OnTermination()
			{
				if (!m_Terminated)
				{
					m_Terminated = true;
				}
			}
			void UpdateUserContext(std::shared_ptr<IScopedLoggerContext> userContext);

		public:
			ScopedLoggerTLS(ScopedLoggerGlobalContext& globalContext)
				:m_GlobalContext(globalContext)
			{
			}

		public:
			ScopedLoggerGlobalContext& GetGlobalContext() const
			{
				return m_GlobalContext;
			}
			ScopedLogger* GetCurrentScope() const
			{
				if (!m_ScopeStack.empty())
				{
					return m_ScopeStack.back();
				}
				return nullptr;
			}

			SystemProcess GetProcess() const noexcept
			{
				return m_Process;
			}
			SystemThread GetThread() const noexcept
			{
				return m_Thread;
			}
			DateTime GetTimestamp() const noexcept
			{
				return m_TimeStamp;
			}
			size_t GetScopeLevel() const noexcept
			{
				return m_ScopeLevel;
			}
			bool IsTerminated() const noexcept
			{
				return m_Terminated;
			}
			bool IsUnknown() const noexcept
			{
				return m_Thread.IsNull();
			}

			void Flush();
			void Write(LogLevel logLevel, DateTime timestamp, StringView message, StringView category);
	};
}

namespace kxf
{
	class ScopedMessageLogger final
	{
		friend class ScopedLoggerTLS;
		friend class ScopedLoggerUnknownTLS;

		private:
			ScopedLogger* m_Scope = nullptr;
			String m_Message;
			String m_Category;
			String m_Separator;
			bool m_SeparatorAllowed = false;
			DateTime m_TimeStamp;
			LogLevel m_LogLevel = LogLevel::Unknown;

		private:
			void Init()
			{
				m_TimeStamp = DateTime::Now();
				m_Separator.assign(kxS(", "));
			}
			void Reset()
			{
				m_Message.clear();
				m_Category.clear();
				m_SeparatorAllowed = false;
				Init();
			}
			bool CanLog() const
			{
				return m_LogLevel != LogLevel::Disabled;
			}

			template<class T>
			void DoLog(const std::basic_string<T>& str)
			{
				DoLog(StringViewOf(str));
			}

			template<class T>
			void DoLog(std::basic_string_view<T> str)
			{
				if (!str.empty() && CanLog())
				{
					ProcessSeparator();
					if constexpr(std::is_same_v<T, wchar_t>)
					{
						m_Message += str;
					}
					else
					{
						m_Message += String::FromUTF8(str);
					}
				}
			}

			void DoLog(const String& str)
			{
				DoLog(str.view());
			}
			void DoLog(const char* str)
			{
				DoLog(StringViewOf(str));
			}
			void DoLog(const wchar_t* str)
			{
				DoLog(StringViewOf(str));
			}
			void ProcessSeparator()
			{
				if (m_SeparatorAllowed && !m_Separator.empty() && !m_Message.empty())
				{
					m_Message += m_Separator;
					m_SeparatorAllowed = false;
				}
			}

			template<class... Args>
			void DoLogArgs(String separator, Args&&... arg)
			{
				SetSeparator(std::move(separator));
				std::initializer_list<int> list{(*this << std::forward<Args>(arg), Sep(), 0) ...};
			}

		public:
			ScopedMessageLogger(LogLevel logLevel = LogLevel::Unknown, String category = {})
				:m_LogLevel(logLevel), m_Category(std::move(category))
			{
				Init();
			}
			ScopedMessageLogger(ScopedLogger& scope, LogLevel logLevel = LogLevel::Unknown, String category = {})
				:m_Scope(&scope), m_LogLevel(logLevel), m_Category(std::move(category))
			{
				Init();
			}
			ScopedMessageLogger(const ScopedMessageLogger&) = delete;
			ScopedMessageLogger(ScopedMessageLogger&&) = default;
			~ScopedMessageLogger()
			{
				Write();
			}

		public:
			ScopedLogger* GetScope() const noexcept
			{
				return m_Scope;
			}
			DateTime GetTimeStamp() const noexcept
			{
				return m_TimeStamp;
			}

			ScopedLogger* DetachScope()
			{
				auto ptr = m_Scope;
				m_Scope = nullptr;

				return ptr;
			}
			StringView ToString() const noexcept
			{
				return m_Message.view();
			}

			void Write();
			void Write(ScopedLoggerTLS& tls)
			{
				tls.Write(m_LogLevel, m_TimeStamp, m_Message.view(), m_Category.view());
			}

			void Finalize()
			{
				Write();
				Reset();
			}
			void Finalize(ScopedLoggerTLS& tls)
			{
				Write(tls);
				Reset();
			}

		public:
			template<class... Args>
			ScopedMessageLogger& Log(Args&&... arg)
			{
				DoLogArgs({}, std::forward<Args>(arg)...);
				return *this;
			}

			template<class... Args>
			ScopedMessageLogger& Print(Args&&... arg)
			{
				DoLogArgs(", ", std::forward<Args>(arg)...);
				return *this;
			}

			template<class... Args>
			ScopedMessageLogger& PrintSep(String separator, Args&&... arg)
			{
				DoLogArgs(separator, std::forward<Args>(arg)...);
				return *this;
			}

			template<class TFormat, class... Args>
			ScopedMessageLogger& Format(const TFormat& format, Args&&... arg)
			{
				ProcessSeparator();
				m_Message.Format(format, std::forward<Args>(arg)...);
				return *this;
			}

			ScopedMessageLogger& LineBreak()
			{
				DoLog("\n");
				return *this;
			}
			ScopedMessageLogger& Sep()
			{
				m_SeparatorAllowed = true;
				return *this;
			}
			ScopedMessageLogger& SetSeparator(String value)
			{
				m_Separator = std::move(value);
				return *this;
			}
			ScopedMessageLogger& SetCategory(String value)
			{
				m_Category = std::move(value);
				return *this;
			}

			ScopedMessageLogger& operator<<(const String& str)
			{
				DoLog(StringViewOf(str));
				return *this;
			}
			ScopedMessageLogger& operator<<(std::string_view str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(std::wstring_view str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(const std::string& str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(const std::wstring& str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(const char* str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(const wchar_t* str)
			{
				DoLog(str);
				return *this;
			}

			template<class T> requires(std::is_pointer_v<T>)
			ScopedMessageLogger& operator<<(T ptr)
			{
				DoLog(String().Format("0x{:0{}x}", reinterpret_cast<intptr_t>(ptr), sizeof(void*) * 2));
				return *this;
			}

			template<class T>
			requires(!std::is_pointer_v<T> && std::is_invocable_r_v<String, decltype(kxf::Format<const String&, const T&>), const String&, const T&>)
			ScopedMessageLogger& operator<<(const T& formattable)
			{
				DoLog(kxf::Format("{}", formattable));
				return *this;
			}

			template<class T>
			ScopedMessageLogger& operator<<(const std::optional<T>& opt)
			{
				if (opt)
				{
					*this << "optional(" << *opt << ')';
				}
				else
				{
					DoLog("optional(nullopt)");
				}
				return *this;
			}

		public:
			ScopedMessageLogger& operator=(const ScopedMessageLogger&) = delete;
			ScopedMessageLogger& operator=(ScopedMessageLogger&&) = default;
	};
}

namespace kxf
{
	class ScopedLogger
	{
		protected:
			ScopedLoggerTLS& m_ScopeTLS;
			String m_Function;
			String m_ReturnValue;
			//String m_FileName;
			//uint32_t m_Line = 0;
			//uint32_t m_Column = 0;
			bool m_IsSuccess = false;
			bool m_IsVoid = false;

		protected:
			void FromSourceLocation(const std::source_location& sourceLocation)
			{
				//m_Line = sourceLocation.line();
				//m_Column = sourceLocation.column();
				//m_FileName = sourceLocation.file_name();
				m_Function = sourceLocation.function_name();
			}
			bool CanEnterLeave() const noexcept
			{
				return !m_Function.IsEmpty();
			}

			void OnEnter(std::optional<StringView> serializedParameters = {});
			void OnLeave();

			ScopedMessageLogger FlowControl(String category = {})
			{
				return ScopedMessageLogger(*this, LogLevel::FlowControl, std::move(category));
			}

		protected:
			ScopedLogger(ScopedLoggerTLS& tls)
				:m_ScopeTLS(tls)
			{
			}

			template<class... Args>
			ScopedLogger(ScopedLoggerTLS& tls, const std::source_location& sourceLocation, Args&&... arg)
				:m_ScopeTLS(tls)
			{
				FromSourceLocation(sourceLocation);

				if (!Utility::IsEmptyParameterPack<Args...>())
				{
					ScopedMessageLogger logger;
					logger.PrintSep(", ", std::forward<Args>(arg)...);
					OnEnter(logger.ToString());
				}
				else
				{
					OnEnter();
				}
			}

			~ScopedLogger()
			{
				OnLeave();
			}

		public:
			ScopedLoggerTLS& GetTLS() const noexcept
			{
				return m_ScopeTLS;
			}
			size_t GetScopeLevel() const noexcept
			{
				return m_ScopeTLS.GetScopeLevel();
			}

			ScopedMessageLogger Log(String category = {})
			{
				return ScopedMessageLogger(*this, LogLevel::Unknown, std::move(category));
			}
			ScopedMessageLogger Trace(String category = {})
			{
				return ScopedMessageLogger(*this, LogLevel::Trace, std::move(category));
			}
			ScopedMessageLogger Debug(String category = {})
			{
				return ScopedMessageLogger(*this, LogLevel::Debug, std::move(category));
			}
			ScopedMessageLogger Info(String category = {})
			{
				return ScopedMessageLogger(*this, LogLevel::Information, std::move(category));
			}
			ScopedMessageLogger Warning(String category = {})
			{
				return ScopedMessageLogger(*this, LogLevel::Warning, std::move(category));
			}
			ScopedMessageLogger Error(String category = {})
			{
				return ScopedMessageLogger(*this, LogLevel::Error, std::move(category));
			}
			ScopedMessageLogger Critical(String category = {})
			{
				return ScopedMessageLogger(*this, LogLevel::Critical, std::move(category));
			}

			void SetSuccess(bool success = true) noexcept
			{
				m_IsSuccess = success;
			}
			void SetFail(bool fail = true) noexcept
			{
				m_IsSuccess = !fail;
			}

			template<class T>
			void LogReturn(const T& value, bool success = true)
			{
				m_IsSuccess = success;
				if (!m_IsVoid)
				{
					ScopedMessageLogger messageLogger(LogLevel::FlowControl);
					messageLogger << value;

					m_ReturnValue = messageLogger.ToString();
				}
			}

			template<class T>
			ScopedMessageLogger operator<<(T&& value)
			{
				ScopedMessageLogger messageLogger(*this, LogLevel::Unknown);
				messageLogger << std::forward<T>(value);

				return messageLogger;
			}
	};

	class ScopedLoggerNewScope final: public ScopedLogger
	{
		private:
			static ScopedLoggerTLS& GetTLS();

		public:
			ScopedLoggerNewScope(const std::source_location& sourceLocation = std::source_location::current())
				:ScopedLogger(GetTLS(), sourceLocation)
			{
			}

			template<class... Args>
			ScopedLoggerNewScope(const std::source_location& sourceLocation, Args&&... arg)
				:ScopedLogger(GetTLS(), sourceLocation, std::forward<Args>(arg)...)
			{
			}
	};

	class ScopedLoggerAutoScope final: public ScopedLogger
	{
		private:
			static ScopedLoggerTLS& GetActiveTLS();

		private:
			ScopedLogger* m_Scope = nullptr;

		public:
			ScopedLoggerAutoScope()
				:ScopedLogger(GetActiveTLS())
			{
				m_Scope = m_ScopeTLS.GetCurrentScope();
			}

		public:
			ScopedLogger& GetCurrentScope() const noexcept
			{
				return *m_Scope;
			}
	};
}

namespace kxf
{
	class ScopedLoggerUnknownScope final: public ScopedLogger
	{
		friend class ScopedLoggerUnknownTLS;

		private:
			void Initialize();
			void Destroy();

		private:
			ScopedLoggerUnknownScope(ScopedLoggerTLS& tls)
				:ScopedLogger(tls)
			{
			}
	};

	class ScopedLoggerUnknownTLS final: public ScopedLoggerTLS
	{
		private:
			ScopedLoggerUnknownScope m_Scope;

		private:
			void Initialize();
			void Destroy();

		public:
			ScopedLoggerUnknownTLS(ScopedLoggerGlobalContext& globalContext)
				:ScopedLoggerTLS(globalContext), m_Scope(*this)
			{
				Initialize();
			}
			~ScopedLoggerUnknownTLS()
			{
				Destroy();
			}
	};
}

namespace kxf
{
	String ToString(LogLevel value);

	class ScopedLoggerGlobalContext final
	{
		public:
			static ScopedLoggerGlobalContext& Initialize(std::shared_ptr<IScopedLoggerContext> userContext, LogLevel logLevel = LogLevel::Unknown);
			static ScopedLoggerGlobalContext& GetInstance();

		private:
			std::atomic<std::shared_ptr<IScopedLoggerContext>> m_UserContext;
			std::atomic<TimeZoneOffset> m_TimeOffset;
			std::atomic<LogLevel> m_LogLevel = LogLevel::Unknown;

			std::optional<ScopedLoggerUnknownTLS> m_UnknownContextTLS;
			FiberLocalSlot m_TLSIndex;
			bool m_UpdateUserContext = false;

		private:
			bool IsInitialized() const noexcept;
			void Initialize();
			void Destroy();

			void OnUserContextUpdated();

		private:
			ScopedLoggerGlobalContext(std::shared_ptr<IScopedLoggerContext> userContext, LogLevel logLevel)
				:m_UserContext(std::move(userContext)), m_LogLevel(logLevel)
			{
				Initialize();
			}
			ScopedLoggerGlobalContext(const ScopedLoggerGlobalContext&) = delete;
			~ScopedLoggerGlobalContext()
			{
				Destroy();
			}

		public:
			std::shared_ptr<IScopedLoggerContext> GetUserContext() const noexcept
			{
				return m_UserContext;
			}

			ScopedLoggerTLS* QueryThreadContext() noexcept;
			ScopedLoggerTLS& GetThreadContext();
			ScopedLoggerTLS& GetUnknownThreadContext() noexcept;

			LogLevel GetLogLevel() const noexcept
			{
				return m_LogLevel;
			}
			void SetLogLevel(LogLevel logLevel) noexcept
			{
				if (ToInt(logLevel) >= 0)
				{
					m_LogLevel = logLevel;
				}
			}
			bool CanLogLevel(LogLevel logLevel) const noexcept;

			TimeZoneOffset GetTimeOffset() const noexcept
			{
				return m_TimeOffset;
			}
			void SetTimeOffset(const TimeZoneOffset& timeOffset)
			{
				m_TimeOffset = timeOffset;
			}
	};
}

namespace kxf::Log
{
	template<class TFormat, class... Args>
	void LogAtLevel(LogLevel logLevel, const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope scope;
		ScopedMessageLogger(scope, logLevel).Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void LogCategoryAtLevel(String category, LogLevel logLevel, const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope scope;
		ScopedMessageLogger(scope, logLevel, std::move(category)).Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Critical(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Critical().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void CriticalCategory(String category, const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Critical(std::move(category)).Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Error(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Error().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void ErrorCategory(String category, const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Error(std::move(category)).Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Warning(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Warning().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void WarningCategory(String category, const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Warning(std::move(category)).Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Info(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Info().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void InfoCategory(String category, const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Info(std::move(category)).Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Debug(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Debug().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void DebugCategory(String category, const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Debug(std::move(category)).Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Trace(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Trace().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void TraceCategory(String category, const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAutoScope().Trace(std::move(category)).Format(format, std::forward<Args>(arg)...);
	}
}

#define KX_SCOPEDLOG							scopedLogger_
#define KX_SCOPEDLOG_AUTO						kxf::ScopedLoggerAutoScope	KX_SCOPEDLOG
#define KX_SCOPEDLOG_FUNC						kxf::ScopedLoggerNewScope	KX_SCOPEDLOG(std::source_location::current())
#define KX_SCOPEDLOG_ARGS(...)					kxf::ScopedLoggerNewScope	KX_SCOPEDLOG(std::source_location::current(), __VA_ARGS__)

#define KX_SCOPEDLOG_VALUE_AS(name, value)		.Format(#name "=[{}]", (value)).Sep()
#define KX_SCOPEDLOG_VALUE(value)				KX_SCOPEDLOG_VALUE_AS(value, value)
