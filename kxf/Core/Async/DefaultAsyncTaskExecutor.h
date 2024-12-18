#pragma once
#include "Common.h"
#include "kxf/Core/IAsyncTask.h"
#include "kxf/Core/IAsyncTaskExecutor.h"
#include "kxf/Threading/IThreadPool.h"
#include <thread>
#include <mutex>
#include <queue>

namespace kxf
{
	class DefaultAsyncTask;
}

namespace kxf
{
	class KXF_API DefaultAsyncTaskExecutor final: public RTTI::DynamicImplementation<DefaultAsyncTaskExecutor, IAsyncTaskExecutor, IThreadPool>
	{
		private:
			std::vector<std::thread> m_ThreadPool;
			mutable std::mutex m_ThreadPoolLock;

			std::vector<std::shared_ptr<DefaultAsyncTask>> m_TaskQueue;
			std::condition_variable m_TaskCondition;
			mutable std::mutex m_TaskQueueLock;

			size_t m_Concurrency = 0;
			std::atomic<bool> m_ShouldTerminate = false;

		private:
			void OnQueue(DefaultAsyncTask& task);
			void OnStartup(DefaultAsyncTask& task);
			void OnCompleted(DefaultAsyncTask& task, bool terminated);

			void OnThread();

		public:
			DefaultAsyncTaskExecutor() noexcept
				:m_Concurrency(std::thread::hardware_concurrency())
			{
			}
			DefaultAsyncTaskExecutor(size_t concurrency) noexcept
				:m_Concurrency(concurrency)
			{
				if (concurrency == 0)
				{
					m_Concurrency = std::thread::hardware_concurrency();
				}
			}
			DefaultAsyncTaskExecutor(const DefaultAsyncTaskExecutor&) = delete;
			~DefaultAsyncTaskExecutor()
			{
				Terminate();
			}

		public:
			// IAsyncTaskExecutor
			void Run() override;
			void Terminate() override;
			bool IsRunning() const override;

			std::shared_ptr<IAsyncTask> QueueTask(AsyncTaskInfo task) override;

			// IThreadPool
			void* GetHandle() const override
			{
				return nullptr;
			}
			size_t GetConcurrency() const override;
			bool SetConcurrency(size_t concurrency) override;

			std::shared_ptr<IAsyncTask> AddTask(std::move_only_function<void()> task) override;

		public:
			DefaultAsyncTaskExecutor& operator=(const DefaultAsyncTaskExecutor&) = delete;
	};
}
