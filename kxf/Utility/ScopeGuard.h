#pragma once
#include "Common.h"

namespace kxf::Utility
{
	template<class TCallable_>
	requires(std::is_invocable_r_v<void, TCallable_>)
	class ScopeGuard final
	{
		public:
			using TCallable = TCallable_;

		private:
			std::optional<TCallable> m_Callable;

		public:
			ScopeGuard(TCallable&& callable) noexcept
				:m_Callable(std::forward<TCallable>(callable))
			{
			}
			ScopeGuard(ScopeGuard&&) noexcept = default;
			ScopeGuard(const ScopeGuard&) = delete;
			~ScopeGuard() noexcept
			{
				Invoke();
			}

		public:
			bool IsActive() const noexcept
			{
				return m_Callable.has_value();
			}
			void Dismiss() noexcept
			{
				m_Callable.reset();
			}
			void Invoke() noexcept(std::is_nothrow_invocable_r_v<void, TCallable>)
			{
				if (m_Callable)
				{
					std::invoke(*m_Callable);
					m_Callable.reset();
				}
			}

		public:
			ScopeGuard& operator=(ScopeGuard&&) noexcept = default;
			ScopeGuard& operator=(const ScopeGuard&) = delete;
	};
}

namespace kxf::Utility
{
	template<class TCallable_>
	class ExceptionScopeGuard final
	{
		static_assert(std::is_nothrow_invocable_r_v<void, TCallable_>, "no-throw invokable callable required");

		public:
			using TCallable = TCallable_;

		private:
			ScopeGuard<TCallable> m_Guard;
			int m_UncaughtExceptions = std::uncaught_exceptions();

		public:
			ExceptionScopeGuard(TCallable&& callable) noexcept
				:m_Guard(std::forward<TCallable>(callable))
			{
			}
			ExceptionScopeGuard(ExceptionScopeGuard&& other) noexcept
			{
				*this = std::move(other);
			}
			ExceptionScopeGuard(const ExceptionScopeGuard&) = delete;
			~ExceptionScopeGuard() noexcept
			{
				if (m_UncaughtExceptions != std::uncaught_exceptions() && m_UncaughtExceptions != std::numeric_limits<int>::max())
				{
					m_Guard.Invoke();
				}
				else
				{
					m_Guard.Dismiss();
				}
			}

		public:
			bool IsActive() const noexcept
			{
				return m_Guard.IsActive();
			}
			void Dismiss() noexcept
			{
				m_Guard.Dismiss();
			}
			void Invoke() noexcept
			{
				m_Guard.Invoke();
			}

		public:
			ExceptionScopeGuard& operator=(ExceptionScopeGuard&& other) noexcept
			{
				m_Guard = std::move(other.m_Guard);
				m_UncaughtExceptions = std::exchange(other.m_UncaughtExceptions, std::numeric_limits<int>::max());

				return *this;
			}
			ExceptionScopeGuard& operator=(const ExceptionScopeGuard&) = delete;
	};
}
