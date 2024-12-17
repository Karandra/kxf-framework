#pragma once
#include "Common.h"
#include "ScopedLogger.h"
#include "kxf/IO/IStream.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Threading/ReadWriteLock.h"

namespace kxf::Private
{
	class ScopedLoggerFlushControl final
	{
		public:
			static constexpr size_t FlushNever = std::numeric_limits<size_t>::max();
			static constexpr size_t FlushDefault = 64;
			static constexpr size_t FlushAlways = 0;

		private:
			size_t m_WriteCount = 0;
			size_t m_FlushThreshold = FlushDefault;

		public:
			void OnWrite() noexcept
			{
				++m_WriteCount;
			}
			void OnFlush() noexcept
			{
				m_WriteCount = 0;
			}
			bool ShouldFlush(LogLevel logLevel) const noexcept;

			void SetFlushThreshold(size_t value) noexcept
			{
				m_FlushThreshold = value;
			}
	};
}

namespace kxf
{
	class ScopedLoggerEmptyTarget final: public IScopedLoggerTarget
	{
		public:
			ScopedLoggerEmptyTarget() noexcept = default;
			ScopedLoggerEmptyTarget(ScopedLoggerTLS& tls) noexcept
			{
			}

		public:
			// IScopedLoggerTarget
			void Write(LogLevel logLevel, StringView str) override
			{
			}
			void Flush() override
			{
			}
	};

	class ScopedLoggerConsoleTarget: public IScopedLoggerTarget
	{
		public:
			ScopedLoggerConsoleTarget() noexcept = default;
			ScopedLoggerConsoleTarget(ScopedLoggerTLS& tls) noexcept
			{
			}

		public:
			// IScopedLoggerTarget
			void Write(LogLevel logLevel, StringView str) override;
			void Flush() override;
	};

	class ScopedLoggerFileTarget: public IScopedLoggerTarget
	{
		private:
			std::shared_ptr<IOutputStream> m_Stream;
			Private::ScopedLoggerFlushControl m_FlushControl;

		public:
			ScopedLoggerFileTarget(ScopedLoggerTLS& tls, IFileSystem& fs, const FSPath& directory = {});

		public:
			// IScopedLoggerTarget
			void Write(LogLevel logLevel, StringView str) override;
			void Flush() override;

			// ScopedLoggerFileTarget
			void SetFlushThreshold(size_t value) noexcept
			{
				m_FlushControl.SetFlushThreshold(value);
			}
	};

	class ScopedLoggerSingleFileTarget: public IScopedLoggerTarget
	{
		private:
			ReadWriteLock m_Lock;
			std::shared_ptr<IOutputStream> m_Stream;
			Private::ScopedLoggerFlushControl m_FlushControl;

		public:
			ScopedLoggerSingleFileTarget(ScopedLoggerTLS& tls)
			{
			}
			ScopedLoggerSingleFileTarget(std::shared_ptr<IOutputStream> stream)
				:m_Stream(std::move(stream))
			{
			}

		public:
			// IScopedLoggerTarget
			void Write(LogLevel logLevel, StringView str) override;
			void Flush() override;

			// ScopedLoggerSingleFileTarget
			void SetFlushThreshold(size_t value) noexcept
			{
				m_FlushControl.SetFlushThreshold(value);
			}
	};

	class ScopedLoggerAggregateTarget: public IScopedLoggerTarget
	{
		private:
			template<class T, class TFunc>
			static void ForEach(T&& container, TFunc&& func)
			{
				for (auto it = container.rbegin(); it != container.rend(); ++it)
				{
					if (std::invoke(func, *it->get()) == CallbackCommand::Terminate)
					{
						break;
					}
				}
			}

		public:
			std::vector<std::shared_ptr<IScopedLoggerTarget>> m_LogTargets;

		public:
			ScopedLoggerAggregateTarget() noexcept = default;
			ScopedLoggerAggregateTarget(ScopedLoggerTLS& tls) noexcept
			{
			}

		public:
			// IScopedLoggerTarget
			void Write(LogLevel logLevel, StringView str) override
			{
				ForEach(m_LogTargets, [&](IScopedLoggerTarget& ref)
				{
					ref.Write(logLevel, str);
					return CallbackCommand::Continue;
				});
			}
			void Flush() override
			{
				ForEach(m_LogTargets, [&](IScopedLoggerTarget& ref)
				{
					ref.Flush();
					return CallbackCommand::Continue;
				});
			}

			String FormatRecord(const ScopedLoggerTLS& tls, LogLevel logLevel, DateTime timestamp, StringView message, StringView category) const override
			{
				String formatted;
				ForEach(m_LogTargets, [&](const IScopedLoggerTarget& ref)
				{
					formatted = ref.FormatRecord(tls, logLevel, timestamp, message, category);
					if (!formatted.IsEmpty())
					{
						return CallbackCommand::Terminate;
					}
					return CallbackCommand::Continue;
				});

				return formatted;
			}

			// AggregateScopedLoggerTarget
			void PushTarget(std::shared_ptr<IScopedLoggerTarget> ptr)
			{
				m_LogTargets.emplace_back(std::move(ptr));
			}
			std::shared_ptr<IScopedLoggerTarget> PopTarget()
			{
				if (!m_LogTargets.empty())
				{
					auto ptr = std::move(m_LogTargets.back());
					m_LogTargets.pop_back();

					return ptr;
				}
				return nullptr;
			}
	};
}
