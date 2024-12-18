#include "kxf-pch.h"
#include "System.h"
#include "kxf/Utility/ScopeGuard.h"

#include <Windows.h>
#include <TlHelp32.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::System::Private
{
	String FormatMessage(const void* source, uint32_t messageID, FlagSet<uint32_t> flags, const Locale& locale) noexcept
	{
		auto lcid = (locale && !locale.IsInvariant() ? locale : locale.GetUserDefault()).GetLCID();

		wchar_t* formattedMessage = nullptr;
		uint32_t length = ::FormatMessageW(*flags.Add(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS),
										   source,
										   messageID,
										   lcid.value_or(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT)),
										   reinterpret_cast<wchar_t*>(&formattedMessage),
										   0,
										   nullptr
		);
		if (length != 0 && formattedMessage)
		{
			Utility::ScopeGuard atExit([&]()
			{
				::LocalFree(formattedMessage);
			});
			return String(formattedMessage, length).TrimBoth();
		}
		return {};
	}
	size_t EnumWindows(std::function<CallbackCommand(void*, uint32_t, uint32_t)> func, std::optional<uint32_t> pid, std::optional<uint32_t> tid)
	{
		struct CallContext final
		{
			decltype(func)& Callback;
			size_t Count = 0;
			std::optional<uint32_t> PID;
			std::optional<uint32_t> TID;
		};

		CallContext context =
		{
			.Callback = func,
			.Count = 0,
			.PID = pid,
			.TID = tid,
		};
		::EnumWindows([](HWND windowHandle, LPARAM lParam) -> BOOL
		{
			auto& context = *reinterpret_cast<CallContext*>(lParam);

			DWORD pid = 0;
			DWORD tid = ::GetWindowThreadProcessId(windowHandle, &pid);

			if (pid != 0 && tid != 0)
			{
				if ((!context.PID || context.PID == pid) && (!context.TID || context.TID == tid))
				{
					context.Count++;
					return std::invoke(context.Callback, windowHandle, pid, tid) == CallbackCommand::Continue;
				}
			}
			return TRUE;
		}, reinterpret_cast<LPARAM>(&context));

		return context.Count;
	}
	size_t EnumThreads(std::function<CallbackCommand(uint32_t, uint32_t)> func, std::optional<uint32_t> pid, std::optional<uint32_t> tid)
	{
		HANDLE snapshotHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (snapshotHandle && snapshotHandle != INVALID_HANDLE_VALUE)
		{
			Utility::ScopeGuard atExit = [&]()
			{
				::CloseHandle(snapshotHandle);
			};

			THREADENTRY32 entry = {};
			entry.dwSize = sizeof(entry);
			if (::Thread32First(snapshotHandle, &entry))
			{
				size_t count = 0;
				bool invokeCallback = true;

				do
				{
					if (entry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(entry.th32OwnerProcessID))
					{
						if (invokeCallback && (!pid || pid == entry.th32OwnerProcessID) && (!tid || tid == entry.th32ThreadID))
						{
							count++;
							if (std::invoke(func, entry.th32OwnerProcessID, entry.th32ThreadID) == CallbackCommand::Terminate)
							{
								invokeCallback = false;
							}
						}
					}
					entry.dwSize = sizeof(entry);
				}
				while (::Thread32Next(snapshotHandle, &entry));
				return count;
			}
		}
		return 0;
	}

	String ResourceTypeToName(size_t id)
	{
		return ToString(id);
	}
	String ResourceTypeToName(const wchar_t* id)
	{
		return ResourceTypeToName(reinterpret_cast<size_t>(id));
	}
	const wchar_t* MakeIntResource(int resID)
	{
		return MAKEINTRESOURCEW(resID);
	}

	FlagSet<uint32_t> MapSystemStandardAccess(FlagSet<SystemStandardAccess> access) noexcept
	{
		if (access == SystemStandardAccess::Everything)
		{
			return STANDARD_RIGHTS_ALL;
		}
		else
		{
			FlagSet<uint32_t> nativeAccess;
			nativeAccess.Add(DELETE, access & SystemStandardAccess::Delete);
			nativeAccess.Add(SYNCHRONIZE, access & SystemStandardAccess::Synchronize);
			nativeAccess.Add(READ_CONTROL, access & SystemStandardAccess::ReadControl);
			nativeAccess.Add(WRITE_OWNER, access & SystemStandardAccess::WriteOwner);
			nativeAccess.Add(WRITE_DAC, access & SystemStandardAccess::WriteDAC);

			return nativeAccess;
		}
	}
	FlagSet<uint32_t> MapSystemThreadAccess(FlagSet<SystemThreadAccess> access) noexcept
	{
		if (access == SystemThreadAccess::Everything)
		{
			return THREAD_ALL_ACCESS;
		}
		else
		{
			FlagSet<uint32_t> nativeAccess;
			nativeAccess.Add(THREAD_QUERY_INFORMATION, access & SystemThreadAccess::QueryInformation);
			nativeAccess.Add(THREAD_QUERY_LIMITED_INFORMATION, access & SystemThreadAccess::QueryLimitedInformation);
			nativeAccess.Add(THREAD_SET_INFORMATION, access & SystemThreadAccess::SetInformation);
			nativeAccess.Add(THREAD_SET_LIMITED_INFORMATION, access & SystemThreadAccess::SetLimitedInformation);
			nativeAccess.Add(THREAD_SUSPEND_RESUME, access & SystemThreadAccess::SuspendResume);
			nativeAccess.Add(THREAD_TERMINATE, access & SystemThreadAccess::Terminate);
			nativeAccess.Add(THREAD_GET_CONTEXT, access & SystemThreadAccess::GetContext);
			nativeAccess.Add(THREAD_SET_CONTEXT, access & SystemThreadAccess::SetContext);
			nativeAccess.Add(THREAD_SET_THREAD_TOKEN, access & SystemThreadAccess::SetToken);
			nativeAccess.Add(THREAD_IMPERSONATE, access & SystemThreadAccess::Impersonate);
			nativeAccess.Add(THREAD_DIRECT_IMPERSONATION, access & SystemThreadAccess::DirectImpersonation);
			nativeAccess.Add(SYNCHRONIZE, access & SystemThreadAccess::Synchronize);

			return nativeAccess;
		}
	}
	FlagSet<uint32_t> MapSystemProcessAccess(FlagSet<SystemProcessAccess> access) noexcept
	{
		if (access == SystemProcessAccess::Everything)
		{
			return PROCESS_ALL_ACCESS;
		}
		else
		{
			FlagSet<uint32_t> nativeAccess;
			nativeAccess.Add(PROCESS_CREATE_PROCESS, access & SystemProcessAccess::CreateProcess);
			nativeAccess.Add(PROCESS_CREATE_THREAD, access & SystemProcessAccess::CreateThread);
			nativeAccess.Add(PROCESS_QUERY_INFORMATION, access & SystemProcessAccess::QueryInformation);
			nativeAccess.Add(PROCESS_QUERY_LIMITED_INFORMATION, access & SystemProcessAccess::QueryLimitedInformation);
			nativeAccess.Add(PROCESS_SET_INFORMATION, access & SystemProcessAccess::SetInformation);
			nativeAccess.Add(PROCESS_SUSPEND_RESUME, access & SystemProcessAccess::SuspendResume);
			nativeAccess.Add(PROCESS_TERMINATE, access & SystemProcessAccess::Terminate);
			nativeAccess.Add(PROCESS_VM_OPERATION, access & SystemProcessAccess::VMOperation);
			nativeAccess.Add(PROCESS_VM_READ, access & SystemProcessAccess::VMRead);
			nativeAccess.Add(PROCESS_VM_WRITE, access & SystemProcessAccess::VMWrite);
			nativeAccess.Add(PROCESS_SET_QUOTA, access & SystemProcessAccess::SetQuota);
			nativeAccess.Add(PROCESS_DUP_HANDLE, access & SystemProcessAccess::DuplicateHandle);
			nativeAccess.Add(SYNCHRONIZE, access & SystemProcessAccess::Synchronize);

			return nativeAccess;
		}
	}

	std::optional<uint32_t> MapSystemProcessPriority(SystemProcessPriority priority) noexcept
	{
		switch (priority)
		{
			case SystemProcessPriority::AboveNormal:
			{
				return ABOVE_NORMAL_PRIORITY_CLASS;
			}
			case SystemProcessPriority::BelowNormal:
			{
				return BELOW_NORMAL_PRIORITY_CLASS;
			}
			case SystemProcessPriority::High:
			{
				return HIGH_PRIORITY_CLASS;
			}
			case SystemProcessPriority::Idle:
			{
				return IDLE_PRIORITY_CLASS;
			}
			case SystemProcessPriority::Normal:
			{
				return NORMAL_PRIORITY_CLASS;
			}
			case SystemProcessPriority::Realtime:
			{
				return REALTIME_PRIORITY_CLASS;
			}
		};
		return {};
	}
	SystemProcessPriority MapSystemProcessPriority(uint32_t priority) noexcept
	{
		switch (priority)
		{
			case ABOVE_NORMAL_PRIORITY_CLASS:
			{
				return SystemProcessPriority::AboveNormal;
			}
			case BELOW_NORMAL_PRIORITY_CLASS:
			{
				return SystemProcessPriority::BelowNormal;
			}
			case HIGH_PRIORITY_CLASS:
			{
				return SystemProcessPriority::High;
			}
			case IDLE_PRIORITY_CLASS:
			{
				return SystemProcessPriority::Idle;
			}
			case NORMAL_PRIORITY_CLASS:
			{
				return SystemProcessPriority::Normal;
			}
			case REALTIME_PRIORITY_CLASS:
			{
				return SystemProcessPriority::Realtime;
			}
		};
		return SystemProcessPriority::None;
	}

	String GetKernelObjectName(const String& name, KernelObjectNamespace ns)
	{
		if (!name.IsEmpty())
		{
			switch (ns)
			{
				case KernelObjectNamespace::Local:
				{
					return Format("Local\\{}", name);
				}
				case KernelObjectNamespace::Global:
				{
					return Format("Global\\{}", name);
				}
			};
		}
		return {};
	}
}
