#include "KxfPCH.h"
#include "CreateProcessExecutor.h"
#include "ProcessEvent.h"
#include "kxf/System/Private/Shell.h"
#include "kxf/System/Private/System.h"

namespace kxf::System
{
	bool ProcessPipe::Create() noexcept
	{
		Close();

		SECURITY_ATTRIBUTES security;
		security.nLength = sizeof(security);
		security.lpSecurityDescriptor = nullptr;
		security.bInheritHandle = TRUE;

		return ::CreatePipe(&m_PipeHandles[Direction::Read], &m_PipeHandles[Direction::Write], &security, 0);
	}
	void ProcessPipe::Close() noexcept
	{
		for (void*& handle: m_PipeHandles)
		{
			if (handle && handle != INVALID_HANDLE_VALUE)
			{
				::CloseHandle(handle);
				handle = nullptr;
			}
		}
	}
}

namespace kxf::System
{
	// CreateProcessExecutor
	void CreateProcessExecutor::PrepareEnvironmentBuffer(const ISystemProcess& info)
	{
		info.EnumEnvironemntVariables([&](const String& name, const String& value)
		{
			// name=value\0
			m_EnvironmentBuffer.AppendData(name.wc_str(), name.length() * sizeof(wchar_t));
			m_EnvironmentBuffer.AppendData(L"=", sizeof(wchar_t));
			m_EnvironmentBuffer.AppendData(value.wc_str(), value.length() * sizeof(wchar_t));
			m_EnvironmentBuffer.AppendData(L"\0", sizeof(wchar_t));

			return CallbackCommand::Continue;
		});
		if (m_EnvironmentBuffer.GetDataLen() != 0)
		{
			m_EnvironmentBuffer.AppendData(L"\0", sizeof(wchar_t));
		}
	}
	bool CreateProcessExecutor::PrepareRedirection()
	{
		if (m_PipeIn.Create() && m_PipeOut.Create() && m_PipeError.Create())
		{
			m_StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
			m_StartupInfo.hStdInput = m_PipeIn[ProcessPipe::Read];
			m_StartupInfo.hStdOutput = m_PipeOut[ProcessPipe::Write];
			m_StartupInfo.hStdError = m_PipeError[ProcessPipe::Write];

			// We must set the handles to those sides of std* pipes that we won't
			// in the child to be non-inheritable. We must do this before launching
			// the child process as otherwise these handles will be inherited by
			// the child which will never close them and so the pipe will not
			// return ERROR_BROKEN_PIPE if the parent or child exits unexpectedly
			// causing the remaining process to potentially become deadlocked in
			// ReadFile() or WriteFile().
			::SetHandleInformation(m_PipeIn[ProcessPipe::Write], HANDLE_FLAG_INHERIT, 0);
			::SetHandleInformation(m_PipeOut[ProcessPipe::Read], HANDLE_FLAG_INHERIT, 0);
			::SetHandleInformation(m_PipeError[ProcessPipe::Read], HANDLE_FLAG_INHERIT, 0);

			return true;
		}
		return false;
	}
	void CreateProcessExecutor::PrepareEndRedirection(wxStreamTempInputBuffer& outBuffer, wxStreamTempInputBuffer& errorBuffer)
	{
		if (m_IsCreated)
		{
			if (IsProcessAlive())
			{
				// We can now initialize the wxStreams
				m_InStream = std::make_unique<wxPipeOutputStream>(m_PipeIn.Detach(ProcessPipe::Write));
				m_OutStream = std::make_unique<wxPipeInputStream>(m_PipeOut.Detach(ProcessPipe::Read));
				m_ErrorStream = std::make_unique<wxPipeInputStream>(m_PipeError.Detach(ProcessPipe::Read));

				// The input buffer outBuffer is connected to stdout, this is why it is called outBuffer and not inBuffer
				outBuffer.Init(m_OutStream.get());
				errorBuffer.Init(m_ErrorStream.get());

				// Notify about streams creation
				OnStreamsInitialized();
			}
		}
		else
		{
			// Close the other handles too
			::CloseHandle(m_PipeIn.Detach(ProcessPipe::Write));
			::CloseHandle(m_PipeOut.Detach(ProcessPipe::Read));
			::CloseHandle(m_PipeError.Detach(ProcessPipe::Read));
		}
	}
	void CreateProcessExecutor::CompleteRedirection()
	{
		if (!m_RedirectionCompleted && m_Flags & CreateSystemProcessFlag::RedirectStdIO)
		{
			wxStreamTempInputBuffer outBuffer;
			wxStreamTempInputBuffer errorBuffer;
			PrepareEndRedirection(outBuffer, errorBuffer);

			m_RedirectionCompleted = true;
		}
	}

	wxThread::ExitCode CreateProcessExecutor::Entry()
	{
		if (m_IsCreated)
		{
			ResumeMainThread();

			WaitProcessInputIdle();
			WaitProcessTermination();
		}
		return nullptr;
	}
	void CreateProcessExecutor::ResumeMainThread()
	{
		if (!m_IsMainThreadResumed)
		{
			::ResumeThread(m_ProcessInfo.hThread);
		}
		m_IsMainThreadResumed = true;
	}
	bool CreateProcessExecutor::IsProcessAlive() const
	{
		return m_IsCreated && !m_IsTerminated;
	}

	bool CreateProcessExecutor::WaitProcessInputIdle()
	{
		if (IsProcessAlive() && m_Flags & CreateSystemProcessFlag::WaitInputIdle)
		{
			if (::WaitForInputIdle(m_ProcessInfo.hProcess, INFINITE) == WAIT_OBJECT_0)
			{
				SendProcessInputIdleEvent();
				return true;
			}
		}
		return false;
	}
	bool CreateProcessExecutor::WaitProcessTermination()
	{
		if (IsProcessAlive() && m_Flags & CreateSystemProcessFlag::WaitTermination)
		{
			if (::WaitForSingleObject(m_ProcessInfo.hProcess, INFINITE) == WAIT_OBJECT_0)
			{
				m_IsTerminated = true;
				CompleteRedirection();

				SendProcessTerminationEvent();
				return true;
			}
		}
		return false;
	}
	
	void CreateProcessExecutor::SendEvent(std::unique_ptr<IEvent> event, const EventID& eventID)
	{
		if (m_EvtHandler)
		{
			if (wxThread::IsMain())
			{
				m_EvtHandler.ProcessEvent(*event, eventID);
			}
			else
			{
				m_EvtHandler.QueueEvent(std::move(event), eventID);
			}
		}
	}
	void CreateProcessExecutor::SendProcessInputIdleEvent()
	{
		if (m_EvtHandler)
		{
			SendEvent(std::make_unique<ProcessEvent>(m_ProcessInfo.dwProcessId, 0), ProcessEvent::EvtInputIdle);
		}
	}
	void CreateProcessExecutor::SendProcessTerminationEvent()
	{
		if (m_EvtHandler)
		{
			SendEvent(std::make_unique<ProcessEvent>(m_ProcessInfo.dwProcessId, GetExitCode().value_or(STILL_ACTIVE)), ProcessEvent::EvtTermination);
		}
	}

	CreateProcessExecutor::CreateProcessExecutor(EvtHandlerDelegate evtHandler, FlagSet<CreateSystemProcessFlag> flags)
		:wxThread(wxTHREAD_JOINABLE), m_EvtHandler(std::move(evtHandler)), m_Flags(flags), m_EnvironmentBuffer(0), m_CommandLineBuffer(0)
	{
		m_StartupInfo.cb = sizeof(m_StartupInfo);
	}
	CreateProcessExecutor::~CreateProcessExecutor()
	{
		CompleteRedirection();
		if (m_ProcessInfo.hThread)
		{
			::CloseHandle(m_ProcessInfo.hThread);
		}
	}
	
	SystemThread CreateProcessExecutor::GetMainThread() const
	{
		if (IsProcessAlive())
		{
			return m_ProcessInfo.dwThreadId;
		}
		return {};
	}
	bool CreateProcessExecutor::CreateProcess(const ISystemProcess& info)
	{
		// Process creation flags
		uint32_t creationFlags = CREATE_SUSPENDED|CREATE_UNICODE_ENVIRONMENT|Private::MapSystemProcessPriority(info.GetPriority()).value_or(0);

		// Startup flags
		m_StartupInfo.dwFlags = STARTF_USESHOWWINDOW|STARTF_FORCEOFFFEEDBACK;
		m_StartupInfo.wShowWindow = Shell::Private::MapSHWindowCommand(info.GetShowWindowCommand()).value_or(SW_SHOWNORMAL);

		if (m_Flags & CreateSystemProcessFlag::RedirectStdIO)
		{
			creationFlags |= CREATE_NO_WINDOW;
			if (!PrepareRedirection())
			{
				return false;
			}
		}

		// Process environment
		PrepareEnvironmentBuffer(info);

		// Process executable path
		m_ExecutablePath = info.GetExecutablePath().GetFullPathTryNS(FSPathNamespace::Win32File);

		// Working directory
		m_WorkingDirectory = info.GetWorkingDirectory().GetFullPathTryNS(FSPathNamespace::Win32File);

		// Process command line
		String commandLine = info.GetExecutableParameters();
		if (commandLine.IsEmpty())
		{
			commandLine = Format("\"{}\"", m_ExecutablePath);
		}
		else
		{
			commandLine = Format("\"{}\" {}", m_ExecutablePath, commandLine);
		}
		m_CommandLineBuffer.AppendData(commandLine.wc_str(), commandLine.length() * sizeof(wchar_t) + sizeof(wchar_t));

		// Start the process
		// TODO: Support 'CreateSystemProcessFlag::ToggleElevation' option
		m_IsCreated = ::CreateProcessW(m_ExecutablePath.wc_str(),
									   static_cast<wchar_t*>(m_CommandLineBuffer.GetData()),
									   nullptr,
									   nullptr,
									   m_Flags & CreateSystemProcessFlag::RedirectStdIO,
									   creationFlags,
									   m_EnvironmentBuffer.IsEmpty() ? nullptr : m_EnvironmentBuffer.GetData(),
									   m_WorkingDirectory.IsEmpty() ? nullptr : m_WorkingDirectory.wc_str(),
									   &m_StartupInfo,
									   &m_ProcessInfo
		);
		if (m_IsCreated)
		{
			AttachHandle(m_ProcessInfo.hProcess);
			return true;
		}
		return false;
	}

	// ISystemProcess
	bool CreateProcessExecutor::Resume()
	{
		if (IsProcessAlive())
		{
			if (!m_IsMainThreadResumed)
			{
				ResumeMainThread();
				return true;
			}
			return RunningSystemProcess::Resume();
		}
		return false;
	}
}
