#pragma once
#include "Common.h"
#include "kxf/Core/IAsyncTask.h"
#include "kxf/Core/IAsyncTaskExecutor.h"
#include "kxf/Core/Any.h"
#include "kxf/Core/DateTime.h"
#include "kxf/System/SystemThread.h"

namespace kxf
{
	class KXF_API DefaultAsyncTask final: public RTTI::Implementation<DefaultAsyncTask, IAsyncTask>
	{
		friend class DefaultAsyncTaskExecutor;

		private:
			std::shared_ptr<IAsyncTaskExecutor> m_TaskExecutor;
			AsyncTaskInfo m_TaskInfo;
			Any m_TaskResult;
			std::atomic<bool> m_IsCompleted = false;
			std::atomic<bool> m_IsTerminated = false;
			std::atomic<bool> m_ShouldTerminate = false;

			TimeSpan m_QueueTime;
			TimeSpan m_StartupTime;
			TimeSpan m_CompletionTime;
			SystemThread m_ExecutingThread;

			std::atomic<bool> m_ShouldWait = false;
			std::condition_variable m_WaitCondition;
			std::mutex m_WaitLock;

		public:
			DefaultAsyncTask(AsyncTaskInfo taskInfo) noexcept
				:m_TaskInfo(std::move(taskInfo))
			{
			}
			DefaultAsyncTask(const DefaultAsyncTask&) = delete;

		public:
			// IAsyncTask
			std::shared_ptr<IAsyncTaskExecutor> GetTaskExecutor() const override
			{
				return m_TaskExecutor;
			}

			void Terminate()
			{
				m_ShouldTerminate = true;
			}
			bool IsTerminated() const override
			{
				return m_IsTerminated;
			}
			bool ShouldTerminate() const override
			{
				return m_ShouldTerminate;
			}

			void WaitCompletion() override
			{
				if (m_IsCompleted || m_IsTerminated || m_ExecutingThread.IsCurrent())
				{
					return;
				}

				if (std::unique_lock lock(m_WaitLock); true)
				{
					m_ShouldWait = true;
					m_WaitCondition.wait(lock, [&]()
					{
						return !m_ShouldWait;
					});
				}
			}
			bool IsCompleted() const override
			{
				return m_IsCompleted;
			}
			Any TakeResult()
			{
				return std::move(m_TaskResult);
			}

			TimeSpan GetQueueTime() const
			{
				return m_QueueTime;
			}
			TimeSpan GetStartupTime() const
			{
				return m_StartupTime;
			}
			TimeSpan GetCompletionTime() const
			{
				return m_CompletionTime;
			}
			SystemThread GetExecutingThread() const
			{
				return m_ExecutingThread;
			}

		public:
			DefaultAsyncTask& operator=(const DefaultAsyncTask&) = delete;
	};
}
