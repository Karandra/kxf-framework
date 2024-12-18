#pragma once
#include "Common.h"
#include "IEventLoop.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/FileSystem/FSActionEvent.h"
#include "kxf/Localization/ILocalizationPackage.h"

namespace kxf
{
	class IEvent;
	class IEventLoop;
	class IEventFilter;
	class IEventExecutor;
	class IAsyncTaskExecutor;
	class CommandLineParser;
}

namespace kxf
{
	class KXF_API ICoreApplication: public RTTI::ExtendInterface<ICoreApplication, IEvtHandler>
	{
		kxf_RTTI_DeclareIID(ICoreApplication, {0x2db9e5b5, 0x29cb, 0x4e8a, {0xb4, 0x59, 0x16, 0xee, 0xb, 0xad, 0x92, 0xdf}});

		public:
			kxf_EVENT_MEMBER(ActivateEvent, Activated);
			kxf_EVENT_MEMBER(FSActionEvent, ExecutingModuleWorkingDirectoryChanged);

		public:
			static ICoreApplication* GetInstance() noexcept;
			static void SetInstance(ICoreApplication* instance) noexcept;

		private:
			// IEvtHandler
			using IEvtHandler::ProcessPendingEvents;
			using IEvtHandler::DiscardPendingEvents;

		public:
			virtual ~ICoreApplication() = default;

		public:
			// ICoreApplication -> Basic Info
			virtual String GetName() const = 0;
			virtual void SetName(const String& name) = 0;

			virtual String GetDisplayName() const = 0;
			virtual void SetDisplayName(const String& name) = 0;

			virtual String GetVendorName() const = 0;
			virtual void SetVendorName(const String& name) = 0;

			virtual String GetVendorDisplayName() const = 0;
			virtual void SetVendorDisplayName(const String& name) = 0;

			virtual Version GetVersion() const = 0;
			virtual void SetVersion(const Version& version) = 0;

			virtual String GetClassName() const = 0;
			virtual void SetClassName(const String& name) = 0;

			// ICoreApplication -> Main Event Loop
			virtual std::shared_ptr<IEventLoop> CreateMainLoop() = 0;
			virtual IEventLoop* GetMainLoop() = 0;
			virtual void ExitMainLoop(int exitCode = 0) = 0;

			virtual void OnEventLoopEnter(IEventLoop& loop) = 0;
			virtual void OnEventLoopExit(IEventLoop& loop) = 0;

			// ICoreApplication -> Active Event Loop
		protected:
			static void CallOnEnterEventLoop(IEventLoop& eventLoop);
			static void CallOnExitEventLoop(IEventLoop& eventLoop);

		public:
			virtual IEventLoop* GetActiveEventLoop() = 0;
			virtual void SetActiveEventLoop(IEventLoop* eventLoop) = 0;

			virtual void WakeUp() = 0;
			virtual bool Pending() = 0;
			virtual bool Dispatch() = 0;
			virtual bool DispatchIdle() = 0;
			virtual bool Yield(FlagSet<EventYieldFlag> flags) = 0;

			// ICoreApplication -> Pending Events
		protected:
			virtual bool OnPendingEventHandlerProcess(IEvtHandler& evtHandler)
			{
				return true;
			}
			virtual bool OnPendingEventHandlerDiscard(IEvtHandler& evtHandler)
			{
				return true;
			}

		public:
			virtual bool IsPendingEventHandlerProcessingEnabled() const = 0;
			virtual void EnablePendingEventHandlerProcessing(bool enable = true) = 0;

			virtual void AddPendingEventHandler(IEvtHandler& evtHandler) = 0;
			virtual bool RemovePendingEventHandler(IEvtHandler& evtHandler) = 0;
			virtual void DelayPendingEventHandler(IEvtHandler& evtHandler) = 0;

			virtual bool ProcessPendingEventHandlers() = 0;
			virtual size_t DiscardPendingEventHandlers() = 0;

			virtual bool IsScheduledForDestruction(const IObject& object) const = 0;
			virtual void ScheduleForDestruction(std::shared_ptr<IObject> object) = 0;
			virtual void FinalizeScheduledForDestruction() = 0;

			// ICoreApplication -> Exception Handler
			virtual bool OnMainLoopException() = 0;
			virtual void OnUnhandledException() = 0;
			virtual void OnFatalException() = 0;

			virtual bool StoreCurrentException() = 0;
			virtual void RethrowStoredException() = 0;

			// ICoreApplication -> Debug Handler
			virtual void OnAssertFailure(const String& file, int line, const String& function, const String& condition, const String& message) = 0;

			// ICoreApplication -> Command Line
			virtual void InitializeCommandLine(char** argv, size_t argc) = 0;
			virtual void InitializeCommandLine(wchar_t** argv, size_t argc) = 0;

			virtual Enumerator<String> EnumCommandLineArgs() const = 0;
			virtual void OnCommandLineInit(CommandLineParser& parser) = 0;
			virtual bool OnCommandLineParsed(CommandLineParser& parser) = 0;
			virtual bool OnCommandLineError(CommandLineParser& parser) = 0;
			virtual bool OnCommandLineHelp(CommandLineParser& parser) = 0;

			// ICoreApplication -> Application
			virtual bool OnCreate() = 0;
			virtual void OnDestroy() = 0;

			virtual bool OnInit() = 0;
			virtual void OnExit() = 0;
			virtual int OnRun() = 0;

			virtual void Exit(int exitCode) = 0;
			virtual std::optional<int> GetExitCode() const = 0;

			virtual void AddEventFilter(std::shared_ptr<IEventFilter> eventFilter) = 0;
			virtual void RemoveEventFilter(IEventFilter& eventFilter) = 0;
			virtual IEventFilter::Result FilterEvent(IEvent& event) = 0;

			virtual IAsyncTaskExecutor& GetTaskExecutor() = 0;
			virtual const ILocalizationPackage& GetLocalizationPackage() const = 0;
	};
}
