#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IAsyncTask;
}

namespace kxf
{
	class KX_API IThreadPool: public RTTI::Interface<IThreadPool>
	{
		KxRTTI_DeclareIID(IThreadPool, {0x40905495, 0x70f3, 0x42f3, {0xb5, 0xf6, 0x64, 0x57, 0xab, 0x8f, 0x1a, 0xa9}});

		public:
			virtual void Run() = 0;
			virtual void Terminate() = 0;
			virtual bool IsRunning() const = 0;
			virtual void* GetHandle() const = 0;

			virtual size_t GetConcurrency() const = 0;
			virtual bool SetConcurrency(size_t value) = 0;

			virtual std::shared_ptr<IAsyncTask> AddTask(std::move_only_function<void()> task) = 0;
	};
}
