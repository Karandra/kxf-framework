#pragma once
#include "Common.h"
#include "ScopedLogger.h"
#include "kxf/IO/IStream.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/FileSystem/IFileSystem.h"

namespace kxf
{
	class ScopedLoggerFileTarget;
	class ScopedLoggerFileContext: public IScopedLoggerContext
	{
		private:
			std::shared_ptr<IFileSystem> m_FileSystem;
			FSPath m_LogDirectory;

		public:
			ScopedLoggerFileContext(std::shared_ptr<IFileSystem> fileSystem, FSPath logDirectory)
				:m_FileSystem(std::move(fileSystem)), m_LogDirectory(std::move(logDirectory))
			{
			}

		public:
			// IScopedLoggerContext
			std::shared_ptr<IScopedLoggerTarget> CreateLogTarget(ScopedLoggerTLS& tls) override;
	};
}

namespace kxf
{
	class ScopedLoggerSingleFileTarget;
	class ScopedLoggerSingleFileContext: public IScopedLoggerContext
	{
		protected:
			std::shared_ptr<IScopedLoggerTarget> m_Target;

		public:
			ScopedLoggerSingleFileContext() = default;
			ScopedLoggerSingleFileContext(std::shared_ptr<IOutputStream> stream);
			ScopedLoggerSingleFileContext(IFileSystem& fs, const FSPath& filePath);

		public:
			// IScopedLoggerContext
			std::shared_ptr<IScopedLoggerTarget> CreateLogTarget(ScopedLoggerTLS& tls) override
			{
				return m_Target;
			}
	};
}

namespace kxf
{
	class ScopedLoggerAggregateTarget;
	class ScopedLoggerAggregateContext: public IScopedLoggerContext
	{
		private:
			std::shared_ptr<ScopedLoggerAggregateTarget> m_AggregateTarget;

		public:
			ScopedLoggerAggregateContext(std::shared_ptr<ScopedLoggerAggregateTarget> aggregateTarget)
				:m_AggregateTarget(std::move(aggregateTarget))
			{
			}

		public:
			// IScopedLoggerContext
			std::shared_ptr<IScopedLoggerTarget> CreateLogTarget(ScopedLoggerTLS& tls) override;
	};
}
