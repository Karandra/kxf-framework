#pragma once
#include "Common.h"

namespace kxf
{
	enum class CallbackCommand: int32_t
	{
		None = -1,

		Continue,
		Terminate,
		Discard
	};
}

namespace kxf
{
	class KX_API CallbackResult final
	{
		private:
			std::array<size_t, 3> m_Counters;
			CallbackCommand m_LastCommand = CallbackCommand::None;

		public:
			CallbackResult() noexcept
			{
				m_Counters.fill(0);
			}

		public:
			void UpdateWith(CallbackCommand command)
			{
				if (auto index = ToInt(command); index >= 0 && static_cast<size_t>(index) < m_Counters.size())
				{
					m_Counters[static_cast<size_t>(index)]++;
					m_LastCommand = command;
				}
				else
				{
					m_LastCommand = CallbackCommand::Terminate;
				}
			}

			CallbackCommand GetLastCommand() const noexcept
			{
				return m_LastCommand;
			}
			size_t GetInvokeCount(CallbackCommand command) const noexcept
			{
				if (auto index = ToInt(command); index >= 0 && static_cast<size_t>(index) < m_Counters.size())
				{
					return m_Counters[static_cast<size_t>(index)];
				}
				return 0;
			}
			size_t GetTotalInvokeCount(CallbackCommand command) const noexcept
			{
				return std::accumulate(m_Counters.begin(), m_Counters.end(), static_cast<size_t>(0));
			}
	};
}

namespace kxf
{
	template<class Rx_, class... Args_>
	class BasicCallbackFunction
	{
		protected:
			std::move_only_function<Rx_(Args_...)> m_Callable;

		protected:
			template<class TFunc>
			requires(std::is_invocable_v<TFunc, Args_...>)
			void SetCallable(TFunc&& func)
			{
				using R = std::invoke_result_t<TFunc, Args_...>;

				if constexpr(std::is_same_v<R, Rx_>)
				{
					m_Callable = std::move(func);
				}
				else if constexpr(std::is_void_v<Rx_> && !std::is_void_v<R>)
				{
					m_Callable = [callable = std::move(func)](Args_&&... arg) mutable
					{
						static_cast<void>(std::invoke(callable, std::forward<Args_>(arg)...));
					};
				}
				else if constexpr(!std::is_void_v<Rx_> && std::is_void_v<R> && std::is_default_constructible_v<Rx_>)
				{
					m_Callable = [callable = std::move(func)](Args_&&... arg) mutable -> Rx_
					{
						std::invoke(callable, std::forward<Args_>(arg)...);
						return {};
					};
				}
				else
				{
					static_assert(false, "BasicCallbackFunction: invalid function signature");
				}
			}

		public:
			BasicCallbackFunction() noexcept = default;

			template<class TFunc>
			requires(std::is_invocable_v<TFunc, Args_...>)
			BasicCallbackFunction(TFunc&& func)
			{
				SetCallable(std::forward<TFunc>(func));
			}

			BasicCallbackFunction(BasicCallbackFunction&&) noexcept = default;
			BasicCallbackFunction(const BasicCallbackFunction&) = delete;

		public:
			bool IsNull() const noexcept
			{
				return !static_cast<bool>(m_Callable);
			}
			Rx_ Invoke(Args_... arg)
			{
				return std::invoke(m_Callable, std::forward<Args_>(arg)...);
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

			BasicCallbackFunction& operator=(BasicCallbackFunction&&) noexcept = default;
			BasicCallbackFunction& operator=(const BasicCallbackFunction&) = delete;
	};
};

namespace kxf
{
	template<class... Args_>
	class CallbackFunction final: public BasicCallbackFunction<CallbackCommand, Args_...>
	{
		private:
			CallbackResult m_Result;

		public:
			CallbackFunction() noexcept = default;

			template<class TFunc>
			requires(std::is_same_v<std::invoke_result_t<TFunc, Args_...>, CallbackCommand>)
			CallbackFunction(TFunc&& func)
			{
				this->m_Callable = std::move(func);
			}

			template<class TFunc>
			requires(std::is_same_v<std::invoke_result_t<TFunc, Args_...>, void>)
			CallbackFunction(TFunc&& func)
			{
				this->m_Callable = [callable = std::move(func)](Args_&&... arg) mutable
				{
					std::invoke(callable, std::forward<Args_>(arg)...);
					return CallbackCommand::Continue;
				};
			}

		public:
			void Reset() noexcept
			{
				m_Result = {};
			}
			bool ShouldTerminate() const noexcept
			{
				return m_Result.GetLastCommand() == CallbackCommand::Terminate;
			}

			CallbackFunction& Invoke(Args_... arg)
			{
				if (this->m_Callable)
				{
					CallbackCommand command = std::invoke(this->m_Callable, std::forward<Args_>(arg)...);
					m_Result.UpdateWith(command);
				}
				else
				{
					m_Result.UpdateWith(CallbackCommand::Discard);
				}
				return *this;
			}
			CallbackCommand GetLastCommand() const noexcept
			{
				return m_Result.GetLastCommand();
			}
			CallbackResult GetResult() const
			{
				return m_Result;
			}
	};
};
