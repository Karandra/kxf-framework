#pragma once
#include "Common.h"
#include "Win32Error.h"
#include "SystemThread.h"

namespace kxf
{
	class CFunctionHookController;
	class CFunctionHookTransaction;
}

namespace kxf
{
	class KXF_API CFunctionHook
	{
		friend class CFunctionHookTransaction;

		protected:
			String m_Name;
			void* m_OriginalAddress = nullptr;
			void* m_TargetAddress = nullptr;
			void* m_HookAddress = nullptr;
			bool m_Attached = false;

		private:
			void OnAttached(CFunctionHookTransaction& transaction);
			void OnDetached(CFunctionHookTransaction& transaction);

		public:
			CFunctionHook() noexcept = default;
			CFunctionHook(const CFunctionHook&) = delete;
			CFunctionHook(CFunctionHook&& other) noexcept
			{
				*this = std::move(other);
			}
			~CFunctionHook();

		public:
			bool IsNull() const noexcept
			{
				return m_TargetAddress == nullptr || m_HookAddress == nullptr;
			}
			bool IsAttached() const noexcept
			{
				return m_Attached;
			}
			bool IsCommitted() const noexcept
			{
				return m_Attached && m_OriginalAddress != m_TargetAddress;
			}

			String GetName() const
			{
				return m_Name;
			}

			template<class TFunc = void>
			requires(std::is_function_v<TFunc> || std::is_void_v<TFunc>)
			TFunc* GetHookAddress() const noexcept
			{
				return static_cast<TFunc*>(m_HookAddress);
			}

			template<class TFunc = void>
			requires(std::is_function_v<TFunc> || std::is_void_v<TFunc>)
			TFunc* GetTargetAddress() const noexcept
			{
				return static_cast<TFunc*>(m_TargetAddress);
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

			CFunctionHook& operator=(const CFunctionHook&) = delete;
			CFunctionHook& operator=(CFunctionHook&& other) noexcept
			{
				m_Name = std::move(other.m_Name);
				m_TargetAddress = std::exchange(other.m_TargetAddress, nullptr);
				m_HookAddress = std::exchange(other.m_HookAddress, nullptr);
				m_Attached = std::exchange(other.m_Attached, false);

				return *this;
			}
	};

	template<class TFunc>
	requires(std::is_function_v<TFunc>)
	class CFunctionTypedHook: public CFunctionHook
	{
		public:
			CFunctionTypedHook() noexcept = default;
			CFunctionTypedHook(const CFunctionTypedHook&) = delete;
			CFunctionTypedHook(CFunctionTypedHook&&) noexcept = default;

		public:
			template<class... Args>
			requires(std::is_invocable_v<TFunc, Args...>)
			auto InvokeHook(Args&&... arg)
			{
				return std::invoke(GetHookAddress<TFunc>(), std::forward<Args>(arg)...);
			}

			template<class... Args>
			requires(std::is_invocable_v<TFunc, Args...>)
			auto InvokeTarget(Args&&... arg)
			{
				return std::invoke(GetTargetAddress<TFunc>(), std::forward<Args>(arg)...);
			}

		public:
			CFunctionTypedHook& operator=(const CFunctionTypedHook&) = delete;
			CFunctionTypedHook& operator=(CFunctionTypedHook&&) noexcept = default;
	};
}

namespace kxf
{
	class KXF_API CFunctionHookTransaction final
	{
		friend class CFunctionHookController;
		enum class State
		{
			Unknown = -1,

			Wait,
			Ready,
			Aborted,
			Committed
		};

		private:
			CFunctionHookController* m_Controller = nullptr;
			RunningSystemThread m_Thread;
			State m_State = State::Unknown;
			Win32Error m_LastError = Win32Error::Fail();

		private:
			bool DoAttachHook(CFunctionHook& hook, void* targetPtr, void* hookPtr, String name);
			bool DoDetachHook(CFunctionHook& hook);

			bool BeginTransaction();
			void OnTransactionBegin();
			void OnTransactionEnd();

		private:
			CFunctionHookTransaction(CFunctionHookController& controller, RunningSystemThread thread);

		public:
			CFunctionHookTransaction(const CFunctionHookTransaction&) = delete;
			CFunctionHookTransaction(CFunctionHookTransaction&&) noexcept = default;
			~CFunctionHookTransaction() noexcept;

		public:
			Win32Error GetLastError() const noexcept
			{
				return m_LastError;
			}
			bool IsReady() const noexcept
			{
				return m_LastError.IsSuccess() && m_State == State::Ready;
			}
			bool Commit();
			bool Abort();

			bool AttachFunctionPtr(CFunctionHook& result, void* target, void* hook, String name = {})
			{
				return DoAttachHook(result, target, hook, std::move(name));
			}

			template<class TFunc>
			requires(std::is_function_v<TFunc>)
			bool AttachFunction(CFunctionHook& result, TFunc* target, TFunc* hook, String name = {})
			{
				return DoAttachHook(result, target, hook, std::move(name));
			}

			bool DetachHook(CFunctionHook& hook)
			{
				return DoDetachHook(hook);
			}

		public:
			explicit operator bool() const noexcept
			{
				return IsReady();
			}
			bool operator!() const noexcept
			{
				return !IsReady();
			}

			CFunctionHookTransaction& operator=(const CFunctionHookTransaction&) = delete;
			CFunctionHookTransaction& operator=(CFunctionHookTransaction&&) noexcept = default;
	};
}

namespace kxf
{
	class KXF_API CFunctionHookController final
	{
		public:
			static CFunctionHookController& GetInstance()
			{
				static CFunctionHookController instance;
				return instance;
			}

		private:
			Win32Error m_LastError = Win32Error::Success();

		private:
			CFunctionHookController();
			~CFunctionHookController();

		public:
			Win32Error GetLastError() const noexcept
			{
				return m_LastError;
			}

			CFunctionHookTransaction NewTransaction(RunningSystemThread thread = RunningSystemThread::GetCurrentThread());
	};
}
