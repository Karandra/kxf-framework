#include "kxf-pch.h"
#include "CFunctionHook.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/Log/Categories.h"
#include <detours/detours.h>
#include <detours/detver.h>

namespace kxf::LogCategory
{
	KX_DefineLogModuleCategory(kxf, Detours);
}

namespace kxf
{
	void CFunctionHook::OnAttached(CFunctionHookTransaction& transaction)
	{
		m_Attached = true;
	}
	void CFunctionHook::OnDetached(CFunctionHookTransaction& transaction)
	{
		m_Attached = false;
		m_OriginalAddress = nullptr;
		m_TargetAddress = nullptr;
		m_HookAddress = nullptr;
	}

	CFunctionHook::~CFunctionHook()
	{
		if (IsAttached())
		{
			Log::WarningCategory(LogCategory::kxf_Detours, "Destroying CFunctionHook without detaching it first: Name=[{}], TargetAddress=[{}], HookAddress=[{}]", m_Name, m_TargetAddress, m_HookAddress);
		}
		if (IsCommitted())
		{
			Log::WarningCategory(LogCategory::kxf_Detours, "Destroying committed CFunctionHook without detaching it first: Name=[{}], TargetAddress=[{}], HookAddress=[{}]", m_Name, m_TargetAddress, m_HookAddress);
		}
	}
}

namespace kxf
{
	bool CFunctionHookTransaction::DoAttachHook(CFunctionHook& hook, void* targetPtr, void* hookPtr, String name)
	{
		if (m_State == State::Ready && targetPtr && hookPtr && targetPtr != hookPtr)
		{
			hook.m_Name = std::move(name);
			hook.m_OriginalAddress = targetPtr;
			hook.m_TargetAddress = targetPtr;
			hook.m_HookAddress = hookPtr;

			m_LastError = ::DetourAttach(&hook.m_TargetAddress, hookPtr);
			if (m_LastError.IsSuccess())
			{
				hook.OnAttached(*this);
				Log::TraceCategory(LogCategory::kxf_Detours, "DetourAttach: Name=[{}], TargetAddress=[{}], HookAddress=[{}]", hook.m_Name, hook.m_TargetAddress, hook.m_HookAddress);

				return true;
			}
			else
			{
				hook.m_OriginalAddress = nullptr;
				hook.m_TargetAddress = nullptr;
				hook.m_HookAddress = nullptr;

				Log::ErrorCategory(LogCategory::kxf_Detours, "DetourAttach: {}", m_LastError);
			}
		}
		return false;
	}
	bool CFunctionHookTransaction::DoDetachHook(CFunctionHook& hook)
	{
		if (m_State == State::Ready && hook.IsAttached())
		{
			void* originalAddress = hook.m_OriginalAddress;
			void* targetAddress = hook.m_TargetAddress;
			void* hookAddress = hook.m_HookAddress;

			m_LastError = ::DetourDetach(&hook.m_TargetAddress, hook.m_HookAddress);
			if (m_LastError.IsSuccess())
			{
				hook.OnDetached(*this);
				Log::TraceCategory(LogCategory::kxf_Detours, "DetourDetach: Name=[{}], TargetAddress=[{}], HookAddress=[{}]", hook.m_Name, targetAddress, hookAddress);

				return true;
			}
			else
			{
				Log::ErrorCategory(LogCategory::kxf_Detours, "DetourDetach: {}", m_LastError);
			}
		}
		return false;
	}

	bool CFunctionHookTransaction::BeginTransaction()
	{
		if (m_Thread)
		{
			m_LastError = ::DetourTransactionBegin();
			m_State = State::Wait;

			if (m_LastError.IsSuccess())
			{
				m_LastError = ::DetourUpdateThread(m_Thread.GetHandle());
				if (m_LastError.IsSuccess())
				{
					m_State = State::Ready;
					OnTransactionBegin();

					return true;
				}
				else
				{
					Log::ErrorCategory(LogCategory::kxf_Detours, "DetourUpdateThread: {}", m_LastError);
				}
			}
			else
			{
				Log::ErrorCategory(LogCategory::kxf_Detours, "DetourTransactionBegin: {}", m_LastError);
			}
		}
		else
		{
			m_LastError = ERROR_INVALID_PARAMETER;
			m_State = State::Unknown;
			Log::ErrorCategory(LogCategory::kxf_Detours, "Invalid thread");
		}
		return false;
	}
	void CFunctionHookTransaction::OnTransactionBegin()
	{
	}
	void CFunctionHookTransaction::OnTransactionEnd()
	{
	}

	CFunctionHookTransaction::CFunctionHookTransaction(CFunctionHookController& controller, RunningSystemThread thread)
		:m_Controller(&controller), m_Thread(std::move(thread))
	{
		BeginTransaction();
	}
	CFunctionHookTransaction::~CFunctionHookTransaction()
	{
		if (m_State == State::Ready)
		{
			Commit();
		}
		else if (m_State == State::Wait)
		{
			Abort();
		}
	}

	bool CFunctionHookTransaction::Commit()
	{
		if (m_State == State::Ready)
		{
			m_LastError = ::DetourTransactionCommit();
			m_State = State::Committed;
			OnTransactionEnd();

			if (m_LastError.IsFail())
			{
				Log::ErrorCategory(LogCategory::kxf_Detours, "DetourTransactionCommit: {}", m_LastError);
			}
			return m_LastError.IsSuccess();
		}
		return false;
	}
	bool CFunctionHookTransaction::Abort()
	{
		if (m_State == State::Wait || m_State == State::Ready)
		{
			m_LastError = ::DetourTransactionAbort();
			m_State = State::Aborted;
			OnTransactionEnd();

			if (m_LastError.IsFail())
			{
				Log::ErrorCategory(LogCategory::kxf_Detours, "DetourTransactionCommit: {}", m_LastError);
			}
			return m_LastError.IsSuccess();
		}
		return false;
	}
}

namespace kxf
{
	CFunctionHookController::CFunctionHookController()
	{
		m_LastError = Win32Error::Success();
		if (!::DetourRestoreAfterWith())
		{
			m_LastError = Win32Error::GetLastError();
			Log::WarningCategory(LogCategory::kxf_Detours, "DetourRestoreAfterWith: {}", m_LastError);
		}
	}
	CFunctionHookController::~CFunctionHookController()
	{
		// Nothing to uninitialize
	}

	CFunctionHookTransaction CFunctionHookController::NewTransaction(RunningSystemThread thread)
	{
		return CFunctionHookTransaction(*this, std::move(thread));
	}
}
