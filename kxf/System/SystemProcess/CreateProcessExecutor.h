#pragma once
#include "RunningSystemProcess.h"
#include "RunningSystemProcessStdIO.h"
#include "kxf/System/SystemThread.h"
#include "kxf/EventSystem/EvtHandlerDelegate.h"

#include <wx/thread.h>
#include <wx/stream.h>
#include <Windows.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::System
{
	class KXF_API ProcessPipe final
	{
		public:
			enum Direction
			{
				Read,
				Write
			};

		private:
			void* m_PipeHandles[2] = {nullptr, nullptr};

		public:
			ProcessPipe() noexcept = default;
			~ProcessPipe() noexcept
			{
				Close();
			}

		public:
			bool IsNull() const noexcept
			{
				return m_PipeHandles[Read] == nullptr;
			}

			bool Create() noexcept;
			void Close() noexcept;

			void* Detach(Direction direction) noexcept
			{
				void* pipeHandle = m_PipeHandles[direction];
				m_PipeHandles[direction] = nullptr;
				return pipeHandle;
			}
			void* operator[](Direction direction) const noexcept
			{
				return m_PipeHandles[direction];
			}
	};
}

namespace kxf::System
{
	class KXF_API CreateProcessExecutor: public RTTI::Implementation<CreateProcessExecutor, RunningSystemProcess, RunningSystemProcessStdIO>, public wxThread
	{
		private:
			EvtHandlerDelegate m_EvtHandler;
			FlagSet<CreateSystemProcessFlag> m_Flags;

			STARTUPINFOW m_StartupInfo = {};
			PROCESS_INFORMATION m_ProcessInfo = {};
			wxMemoryBuffer m_EnvironmentBuffer;
			wxMemoryBuffer m_CommandLineBuffer;
			String m_ExecutablePath;
			String m_WorkingDirectory;

			std::atomic<bool> m_IsCreated = false;
			std::atomic<bool> m_IsTerminated = false;
			std::atomic<bool> m_IsMainThreadResumed = false;

			// IO redirection
			ProcessPipe m_PipeIn;
			ProcessPipe m_PipeOut;
			ProcessPipe m_PipeError;
			bool m_RedirectionCompleted = false;

		private:
			void PrepareEnvironmentBuffer(const ISystemProcess& info);
			bool PrepareRedirection();
			void PrepareEndRedirection(wxStreamTempInputBuffer& outBuffer, wxStreamTempInputBuffer& errorBuffer);
			void CompleteRedirection();

		protected:
			// IObject
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				// Don't return 'ISystemProcessStdIO' interface if we don't have standard IO redirected
				if (!m_Flags.Contains(CreateSystemProcessFlag::RedirectStdIO) && iid.IsOfType<ISystemProcessStdIO>())
				{
					return nullptr;
				}
				return Implementation::QueryInterface(iid);
			}

			// CreateProcessExecutor
			ExitCode Entry() override;
			void ResumeMainThread();
			bool IsProcessAlive() const;

			bool WaitProcessInputIdle();
			bool WaitProcessTermination();

			void SendEvent(std::unique_ptr<IEvent> event, const EventID& eventID);
			void SendProcessInputIdleEvent();
			void SendProcessTerminationEvent();
			
		public:
			CreateProcessExecutor(EvtHandlerDelegate evtHandler, FlagSet<CreateSystemProcessFlag> flags);
			~CreateProcessExecutor();

		public:
			// CreateProcessExecutor
			SystemThread GetMainThread() const;

			bool CreateProcess(const ISystemProcess& info);
			wxThreadError Run()
			{
				return wxThread::Run();
			}
			void RunHere()
			{
				Entry();
			}

			// ISystemProcess
			bool Resume() override;
	};
}
