#pragma once
#include "Common.h"
#include "Any.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IAsyncTask;
}

namespace kxf
{
	class AsyncTaskInfo final
	{
		private:
			std::move_only_function<Any(std::shared_ptr<IAsyncTask>)> m_Task;

		public:
			AsyncTaskInfo() noexcept = default;
			AsyncTaskInfo(const AsyncTaskInfo&) = delete;
			AsyncTaskInfo(AsyncTaskInfo&&) = default;

			// Exact signature
			template<class TFunc>
			requires(std::is_constructible_v<decltype(m_Task), TFunc>)
			AsyncTaskInfo(TFunc&& func)
				:m_Task(std::move(func))
			{
			}

			// Parameterless void function, no result
			template<class TFunc>
			requires(std::is_invocable_v<TFunc> && std::is_same_v<void, std::invoke_result_t<TFunc>>)
			AsyncTaskInfo(TFunc&& func)
			{
				m_Task = [callable = std::move(func)](auto&&) mutable -> Any
				{
					std::invoke(callable);
					return {};
				};
			}

			// Parameterless non-void function with Any-constructible result
			template<class TFunc>
			requires(std::is_invocable_v<TFunc> && std::is_constructible_v<Any, std::invoke_result_t<TFunc>>)
			AsyncTaskInfo(TFunc&& func)
			{
				m_Task = [callable = std::move(func)](auto&&) mutable -> Any
				{
					return std::invoke(callable);
				};
			}

		public:
			bool IsNull() const noexcept
			{
				return !static_cast<bool>(m_Task);
			}

			Any Execute(std::shared_ptr<IAsyncTask> task)
			{
				return std::invoke(m_Task, std::move(task));
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			AsyncTaskInfo& operator=(const AsyncTaskInfo&) = delete;
			AsyncTaskInfo& operator=(AsyncTaskInfo&&) = default;
	};
};

namespace kxf
{
	class KXF_API IAsyncTaskExecutor: public RTTI::Interface<IAsyncTaskExecutor>
	{
		kxf_RTTI_DeclareIID(IAsyncTask, {0xef1f966d, 0xd3b9, 0x462c, {0xa2, 0x25, 0x6b, 0xf4, 0x1, 0x6e, 0x50, 0xf2}});

		public:
			virtual void Run() = 0;
			virtual void Terminate() = 0;
			virtual bool IsRunning() const = 0;

			virtual std::shared_ptr<IAsyncTask> QueueTask(AsyncTaskInfo task) = 0;
	};
}
