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
	class KX_API CallbackFunctionState final
	{
		template<class... Args_>
		friend class CallbackFunction;

		private:
			std::array<size_t, 3> m_Counters;
			CallbackCommand m_LastCommand = CallbackCommand::None;

		private:
			size_t AsIndex(CallbackCommand command) const noexcept;
			void UpdateWith(CallbackCommand command) noexcept;

		public:
			CallbackFunctionState() noexcept
			{
				m_Counters.fill(0);
			}
			CallbackFunctionState(const CallbackFunctionState&) noexcept = default;
			CallbackFunctionState(CallbackFunctionState&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			size_t GetCount() const noexcept;
			size_t GetCount(CallbackCommand command) const noexcept;
			size_t GetTotalCount() const noexcept;
			CallbackCommand GetLastCommand() const noexcept
			{
				return m_LastCommand;
			}

		public:
			CallbackFunctionState& operator=(const CallbackFunctionState&) noexcept = default;
			CallbackFunctionState& operator=(CallbackFunctionState&& other) noexcept
			{
				if (this != &other)
				{
					m_Counters = other.m_Counters;
					other.m_Counters.fill(0);

					m_LastCommand = std::exchange(other.m_LastCommand, CallbackCommand::None);
				}
				return *this;
			}
	};

	template<class T>
	class CallbackResult final
	{
		public:
			using TResult = typename T;

		private:
			CallbackFunctionState m_State;
			std::optional<T> m_Result;

		public:
			CallbackResult() noexcept = default;
			CallbackResult(CallbackFunctionState state, T result)
				:m_State(std::move(state)), m_Result(std::move(result))
			{
			}

		public:
			bool IsNull() const noexcept
			{
				return m_State.GetLastCommand() != CallbackCommand::None && m_Result.has_value();
			}
			T Take() noexcept(std::is_nothrow_move_constructible_v<T>)
			{
				return std::exchange(m_Result, std::nullopt).value();
			}

			CallbackFunctionState GetState() const noexcept
			{
				return m_State;
			}
			size_t GetCount() const noexcept
			{
				return m_State.GetCount();
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

			T& operator*() noexcept
			{
				return *m_Result;
			}
			const T& operator*() const noexcept
			{
				return *m_Result;
			}
	};

	template<>
	class CallbackResult<void> final
	{
		private:
			CallbackFunctionState m_State;

		public:
			CallbackResult() noexcept = default;
			CallbackResult(CallbackFunctionState state) noexcept
				:m_State(std::move(state))
			{
			}

		public:
			bool IsNull() const noexcept
			{
				return m_State.GetLastCommand() == CallbackCommand::None;
			}

			CallbackFunctionState GetState() const noexcept
			{
				return m_State;
			}
			size_t GetCount() const noexcept
			{
				return m_State.GetCount();
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
			CallbackFunctionState m_State;

		private:
			void OnFinalize() noexcept
			{
				// It the callback wasn't called even once (because there were 0 elements
				// to iterate over, for example), set the last command to CallbackCommand::Discard
				// to indicate that the operation was successfully completed.
				if (m_State.m_LastCommand == CallbackCommand::None)
				{
					m_State.m_LastCommand = CallbackCommand::Discard;
				}
			}

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
			CallbackFunction& Invoke(Args_... arg)
			{
				if (this->m_Callable)
				{
					CallbackCommand command = std::invoke(this->m_Callable, std::forward<Args_>(arg)...);
					m_State.UpdateWith(command);
				}
				else
				{
					m_State.UpdateWith(CallbackCommand::Discard);
				}
				return *this;
			}
			bool ShouldTerminate() const noexcept
			{
				return m_State.GetLastCommand() == CallbackCommand::Terminate;
			}
			void ResetState() noexcept
			{
				m_State = {};
			}

			CallbackFunctionState GetState() const noexcept
			{
				return m_State;
			}
			CallbackCommand GetLastCommand() const noexcept
			{
				return m_State.GetLastCommand();
			}

			CallbackResult<void> Finalize() noexcept
			{
				OnFinalize();
				return std::move(m_State);
			}

			template<class T>
			CallbackResult<T> Finalize(T result)
			{
				OnFinalize();
				return {std::move(m_State), std::move(result)};
			}
	};
};
