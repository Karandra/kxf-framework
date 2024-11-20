#include "KxfPCH.h"
#include "DefaultAsyncTaskExecutor.h"
#include "DefaultAsyncTask.h"

namespace kxf
{
	void DefaultAsyncTaskExecutor::OnQueue(DefaultAsyncTask& task)
	{
		task.m_QueueTime = TimeSpan::Now();
		task.m_TaskExecutor = QueryInterface<IAsyncTaskExecutor>();
	}
	void DefaultAsyncTaskExecutor::OnStartup(DefaultAsyncTask& task)
	{
		task.m_StartupTime = TimeSpan::Now();
		task.m_ExecutingThread = SystemThread::GetCurrentThread();
	}
	void DefaultAsyncTaskExecutor::OnCompleted(DefaultAsyncTask& task, bool terminated)
	{
		task.m_CompletionTime = TimeSpan::Now();
		task.m_IsCompleted = !terminated;
		task.m_IsTerminated = terminated;

		if (task.m_ShouldWait)
		{
			task.m_WaitCondition.notify_all();
			task.m_ShouldWait = false;
		}
	}

	void DefaultAsyncTaskExecutor::OnThread()
	{
		bool shouldTerminate = false;
		while (!shouldTerminate)
		{
			std::shared_ptr<DefaultAsyncTask> task;
			if (std::unique_lock lock(m_TaskQueueLock); true)
			{
				m_TaskCondition.wait(lock, [&]()
				{
					shouldTerminate = m_ShouldTerminate;
					return !m_TaskQueue.empty() || shouldTerminate;
				});

				if (!shouldTerminate)
				{
					task = std::move(m_TaskQueue.front());
					m_TaskQueue.pop_back();
				}
			}

			if (task)
			{
				OnStartup(*task);
				task->m_TaskResult = task->m_TaskInfo.Execute(task);
				OnCompleted(*task, task->m_ShouldTerminate);
			}
		}
	}

	// IAsyncTaskExecutor
	void DefaultAsyncTaskExecutor::Run()
	{
		if (std::unique_lock lock(m_ThreadPoolLock); m_ThreadPool.empty())
		{
			m_ThreadPool.reserve(m_Concurrency);
			for (size_t i = 0; i < m_Concurrency; i++)
			{
				m_ThreadPool.emplace_back([this]()
				{
					OnThread();
				});
			}
		}
	}
	void DefaultAsyncTaskExecutor::Terminate()
	{
		if (std::unique_lock lock(m_ThreadPoolLock); !m_ThreadPool.empty())
		{
			m_ShouldTerminate = true;

			// Wake up all threads
			m_TaskCondition.notify_all();

			// Join all threads
			for (auto& thread: m_ThreadPool)
			{
				thread.join();
			}
			m_ThreadPool.clear();
			m_ShouldTerminate = false;
		}
	}
	bool DefaultAsyncTaskExecutor::IsRunning() const
	{
		if (std::unique_lock lock(m_ThreadPoolLock); m_ThreadPool.empty())
		{
			return true;
		}
		return false;
	}

	std::shared_ptr<IAsyncTask> DefaultAsyncTaskExecutor::QueueTask(AsyncTaskInfo task)
	{
		if (task)
		{
			std::shared_ptr<DefaultAsyncTask> ptr;
			if (std::unique_lock lock(m_TaskQueueLock); true)
			{
				ptr = m_TaskQueue.emplace_back(std::make_shared<DefaultAsyncTask>(std::move(task)));
				OnQueue(*ptr);
			}
			m_TaskCondition.notify_one();

			return ptr;
		}
		return nullptr;
	}

	// IThreadPool
	size_t DefaultAsyncTaskExecutor::GetConcurrency() const
	{
		return m_Concurrency;
	}
	bool DefaultAsyncTaskExecutor::SetConcurrency(size_t concurrency)
	{
		if (concurrency == 0)
		{
			return false;
		}

		if (std::unique_lock lock(m_ThreadPoolLock); m_ThreadPool.empty())
		{
			m_Concurrency = concurrency;
			return true;
		}
		return false;
	}

	std::shared_ptr<IAsyncTask> DefaultAsyncTaskExecutor::AddTask(std::move_only_function<void()> task)
	{
		if (task)
		{
			return QueueTask(AsyncTaskInfo(std::move(task)));
		}
		return nullptr;
	}
}
