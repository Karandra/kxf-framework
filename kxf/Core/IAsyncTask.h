#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class Any;
	class TimeSpan;
	class SystemThread;
	class IAsyncTaskExecutor;
}

namespace kxf
{
	class KXF_API IAsyncTask: public RTTI::Interface<IAsyncTask>
	{
		kxf_RTTI_DeclareIID(IAsyncTask, {0x87684dfe, 0x2e65, 0x4739, {0xb0, 0x4b, 0x63, 0xa5, 0x9e, 0x5d, 0x71, 0x4b}});

		public:
			virtual std::shared_ptr<IAsyncTaskExecutor> GetTaskExecutor() const = 0;

			virtual void Terminate() = 0;
			virtual bool IsTerminated() const = 0;
			virtual bool ShouldTerminate() const = 0;

			virtual void WaitCompletion() = 0;
			virtual bool IsCompleted() const = 0;
			virtual Any TakeResult() = 0;

			virtual TimeSpan GetQueueTime() const = 0;
			virtual TimeSpan GetStartupTime() const = 0;
			virtual TimeSpan GetCompletionTime() const = 0;
			virtual SystemThread GetExecutingThread() const = 0;
	};
}
