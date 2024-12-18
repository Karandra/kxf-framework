#include "kxf-pch.h"
#include "RunningSystemProcess.h"
#include "kxf/System/NativeAPI.h"
#include "kxf/System/SystemProcess.h"
#include "kxf/System/SystemThread.h"
#include "kxf/System/SystemWindow.h"
#include "kxf/System/Private/System.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/String.h"
#include "kxf/Utility/ScopeGuard.h"

#include <Windows.h>
#include <PsAPI.h>
#include <WInternl.h>
#include "kxf/Win32/UndefMacros.h"

namespace
{
	bool SafeTerminateProcess(HANDLE processHandle, uint32_t exitCode) noexcept
	{
		using namespace kxf;

		bool isSuccess = false;
		DWORD errorCode = 0;

		HANDLE processHandleDuplicate = nullptr;
		HANDLE remoteThreadHandle = nullptr;
		Utility::ScopeGuard atExit = [&]()
		{
			if (processHandleDuplicate)
			{
				::CloseHandle(processHandleDuplicate);
			}
			if (remoteThreadHandle)
			{
				::CloseHandle(remoteThreadHandle);
			}

			if (!isSuccess)
			{
				::SetLastError(errorCode);
			}
		};

		::DuplicateHandle(::GetCurrentProcess(),
						  processHandle,
						  ::GetCurrentProcess(),
						  &processHandleDuplicate,
						  PROCESS_ALL_ACCESS,
						  FALSE,
						  0
		);


		DWORD processExitCode = 0;
		if (::GetExitCodeProcess(processHandleDuplicate ? processHandleDuplicate : processHandle, &processExitCode) && (processExitCode == STILL_ACTIVE))
		{
			DWORD threadID = 0;
			remoteThreadHandle = ::CreateRemoteThread(processHandleDuplicate ? processHandleDuplicate : processHandle,
													  nullptr,
													  0,
													  reinterpret_cast<LPTHREAD_START_ROUTINE>(::GetProcAddress(::GetModuleHandleW(L"kernel32.dll"), "ExitProcess")),
													  reinterpret_cast<void*>(static_cast<size_t>(exitCode)),
													  0,
													  &threadID);

			if (!remoteThreadHandle)
			{
				errorCode = ::GetLastError();
			}
		}
		else
		{
			errorCode = ERROR_PROCESS_ABORTED;
		}
		
		if (remoteThreadHandle)
		{
			// Must wait process to terminate to guarantee that it has exited
			::WaitForSingleObject(processHandleDuplicate ? processHandleDuplicate : processHandle, INFINITE);
			isSuccess = true;
		}
		return isSuccess;
	}
}

namespace kxf
{
	RunningSystemProcess RunningSystemProcess::GetCurrentProcess()
	{
		RunningSystemProcess process;
		process.AttachHandle(::GetCurrentProcess());

		return process;
	}
	RunningSystemProcess RunningSystemProcess::OpenCurrentProcess(FlagSet<SystemProcessAccess> access, bool inheritHandle)
	{
		return RunningSystemProcess(::GetCurrentProcessId(), access, inheritHandle);
	}

	// ISystemProcess
	bool RunningSystemProcess::IsCurrent() const
	{
		return GetID() == ::GetCurrentProcessId();
	}
	bool RunningSystemProcess::Is64Bit() const
	{
		if (m_Handle)
		{
			if (NativeAPI::Kernel32::IsWow64Process2)
			{
				// Set initial value to something we aren't looking for after the successful function call
				uint16_t processMachine = IMAGE_FILE_MACHINE_TARGET_HOST;
				uint16_t nativeMachine = IMAGE_FILE_MACHINE_TARGET_HOST;
				if (NativeAPI::Kernel32::IsWow64Process2(m_Handle, &processMachine, &nativeMachine))
				{
					return processMachine == IMAGE_FILE_MACHINE_UNKNOWN;
				}
			}
			if (NativeAPI::Kernel32::IsWow64Process)
			{
				BOOL value = FALSE;
				if (NativeAPI::Kernel32::IsWow64Process(m_Handle, &value))
				{
					return !value;
				}
			}
		}
		return false;
	}
	uint32_t RunningSystemProcess::GetID() const
	{
		return ::GetProcessId(m_Handle);
	}
	
	SystemProcessPriority RunningSystemProcess::GetPriority() const
	{
		return System::Private::MapSystemProcessPriority(::GetPriorityClass(m_Handle));
	}
	bool RunningSystemProcess::SetPriority(SystemProcessPriority priority)
	{
		if (auto value = System::Private::MapSystemProcessPriority(priority))
		{
			return ::SetPriorityClass(m_Handle, *value);
		}
		return false;
	}

	bool RunningSystemProcess::IsRunning() const
	{
		DWORD exitCode = 0;
		return ::GetExitCodeProcess(m_Handle, &exitCode) && exitCode == STILL_ACTIVE;
	}
	std::optional<uint32_t> RunningSystemProcess::GetExitCode() const
	{
		DWORD exitCode = STILL_ACTIVE;
		if (::GetExitCodeProcess(m_Handle, &exitCode) && exitCode != STILL_ACTIVE)
		{
			return exitCode;
		}
		return {};
	}
	bool RunningSystemProcess::Terminate(uint32_t exitCode)
	{
		return ::TerminateProcess(m_Handle, exitCode);
	}

	bool RunningSystemProcess::Suspend()
	{
		if (NativeAPI::NtDLL::NtSuspendProcess)
		{
			return NativeAPI::NtDLL::NtSuspendProcess(m_Handle) != 0;
		}
		return false;
	}
	bool RunningSystemProcess::Resume()
	{
		if (NativeAPI::NtDLL::NtResumeProcess)
		{
			return NativeAPI::NtDLL::NtResumeProcess(m_Handle) != 0;
		}
		return false;
	}

	String RunningSystemProcess::GetCommandLine() const
	{
		// http://stackoverflow.com/questions/18358150/ntqueryinformationprocess-keep-to-fail

		// PROCESS_QUERY_INFORMATION|PROCESS_VM_READ
		if (NativeAPI::NtDLL::NtQueryInformationProcess)
		{
			PROCESS_BASIC_INFORMATION processInformation = {};
			uint32_t processInformationSize = 0;
			if (NativeAPI::NtDLL::NtQueryInformationProcess(m_Handle, PROCESSINFOCLASS::ProcessBasicInformation, &processInformation, sizeof(processInformation), &processInformationSize) >= 0)
			{
				if (processInformation.PebBaseAddress == nullptr)
				{
					return {};
				}

				// Read PEB memory block
				SIZE_T read = 0;
				PEB peb = {};
				if (::ReadProcessMemory(m_Handle, processInformation.PebBaseAddress, &peb, sizeof(peb), &read))
				{
					// Get size of command line string
					RTL_USER_PROCESS_PARAMETERS processParameters = {};
					if (::ReadProcessMemory(m_Handle, peb.ProcessParameters, &processParameters, sizeof(processParameters), &read))
					{
						if (processParameters.CommandLine.Length != 0)
						{
							String result;
							const size_t resultLength = (processParameters.CommandLine.Length + 1) / sizeof(wchar_t);
							if (::ReadProcessMemory(m_Handle, processParameters.CommandLine.Buffer, Utility::StringBuffer(result, resultLength).wc_str(), processParameters.CommandLine.Length, &read))
							{
								return result;
							}
						}
					}
				}
			}
		}
		return {};
	}
	FSPath RunningSystemProcess::GetExecutablePath() const
	{
		DWORD length = INT16_MAX;
		String result;
		if (::QueryFullProcessImageNameW(m_Handle, 0, Utility::StringBuffer(result, length, true), &length))
		{
			return result;
		}
		return {};
	}
	FSPath RunningSystemProcess::GetWorkingDirectory() const
	{
		// TODO: https://stackoverflow.com/questions/14018280/how-to-get-a-process-working-dir-on-windows
		if (IsCurrent())
		{
			return NativeFileSystem().GetExecutingModuleWorkingDirectory();
		}
		return {};
	}
	String RunningSystemProcess::GetExecutableParameters() const
	{
		String commandLine = GetCommandLine();
		if (!commandLine.IsEmpty())
		{
			String parameters;
			if (commandLine.StartsWith(GetExecutablePath().GetFullPath(), &parameters, StringActionFlag::IgnoreCase))
			{
				return parameters;
			}
			return commandLine;
		}
		return {};
	}
	SHWindowCommand RunningSystemProcess::GetShowWindowCommand() const
	{
		return SHWindowCommand::None;
	}

	size_t RunningSystemProcess::EnumEnvironemntVariables(std::function<CallbackCommand(const String&, const String&)> func) const
	{
		// TODO:
		// http://stackoverflow.com/questions/38297878/get-startupinfo-for-given-process
		// https://msdn.microsoft.com/en-us/library/bb432286(v=vs.85).aspx
		return 0;
	}
	size_t RunningSystemProcess::EnumThreads(std::function<CallbackCommand(SystemThread)> func) const
	{
		if (!IsNull())
		{
			return System::Private::EnumThreads([&](uint32_t pid, uint32_t tid)
			{
				return std::invoke(func, tid);
			}, GetID());
		}
		return 0;
	}
	size_t RunningSystemProcess::EnumWindows(std::function<CallbackCommand(SystemWindow)> func) const
	{
		if (!IsNull())
		{
			return System::Private::EnumWindows([&](void* hwnd, uint32_t pid, uint32_t tid)
			{
				return std::invoke(func, hwnd);
			}, GetID());
		}
		return 0;
	}

	// RunningSystemProcess
	bool RunningSystemProcess::Open(uint32_t pid, FlagSet<SystemProcessAccess> access, bool inheritHandle)
	{
		if (!m_Handle)
		{
			m_Handle = ::OpenProcess(*System::Private::MapSystemProcessAccess(access), inheritHandle, pid);
			return m_Handle != nullptr;
		}
		return false;
	}
	void RunningSystemProcess::Close()
	{
		if (m_Handle)
		{
			::CloseHandle(m_Handle);
			m_Handle = nullptr;
		}
	}

	SystemThread RunningSystemProcess::GetMainThread() const
	{
		SystemThread result;
		std::optional<FILETIME> earliestTime;

		EnumThreads([&](SystemThread threadInfo)
		{
			if (auto thread = threadInfo.Open(SystemThreadAccess::QueryLimitedInformation))
			{
				FILETIME creation = {};
				FILETIME exit = {};
				FILETIME kernel = {};
				FILETIME user = {};
				if (::GetThreadTimes(thread.GetHandle(), &creation, &exit, &kernel, &user))
				{
					if (!earliestTime)
					{
						earliestTime = creation;
						result = threadInfo;
					}
					else if (::CompareFileTime(&creation, &*earliestTime) == -1)
					{
						earliestTime = creation;
						result = threadInfo;
					}
				}
			}
			return CallbackCommand::Continue;
		});
		return result;
	}
	bool RunningSystemProcess::SafeTerminate(uint32_t exitCode)
	{
		return SafeTerminateProcess(m_Handle, exitCode);
	}
}
