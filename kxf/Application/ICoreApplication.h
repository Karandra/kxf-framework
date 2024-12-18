#pragma once
#include "Common.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/FileSystem/FSActionEvent.h"
#include "kxf/Localization/ILocalizationPackage.h"
class wxCmdLineParser;

namespace kxf
{
	class IEvent;
	class IEventLoop;
	class IEventFilter;
	class IEventExecutor;

	class IAsyncTaskExecutor;
	class CommandLineParser;
}

namespace kxf::Application
{
	class KXF_API IBasicInfo: public RTTI::Interface<IBasicInfo>
	{
		kxf_RTTI_DeclareIID(IBasicInfo, {0x2604f989, 0xb6d1, 0x4eb7, {0x8b, 0x2a, 0xd2, 0x70, 0x6, 0x96, 0x4c, 0x21}});

		public:
			virtual ~IBasicInfo() = default;

		public:
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
	};

	class KXF_API IMainEventLoop: public RTTI::Interface<IMainEventLoop>
	{
		kxf_RTTI_DeclareIID(IMainEventLoop, {0x537d4221, 0xb942, 0x4536, {0x91, 0xf0, 0xb, 0xd9, 0x56, 0x38, 0x59, 0x19}});

		public:
			virtual ~IMainEventLoop() = default;

		public:
			virtual std::shared_ptr<IEventLoop> CreateMainLoop() = 0;
			virtual IEventLoop* GetMainLoop() = 0;
			virtual void ExitMainLoop(int exitCode = 0) = 0;

			virtual void OnEventLoopEnter(IEventLoop& loop) = 0;
			virtual void OnEventLoopExit(IEventLoop& loop) = 0;
	};

	class KXF_API IActiveEventLoop: public RTTI::Interface<IActiveEventLoop>
	{
		kxf_RTTI_DeclareIID(IActiveEventLoop, {0xd27bff03, 0x58d1, 0x471b, {0x83, 0x67, 0x3c, 0xde, 0xfd, 0x4c, 0x77, 0xb0}});

		protected:
			static void CallOnEnterEventLoop(IEventLoop& eventLoop);
			static void CallOnExitEventLoop(IEventLoop& eventLoop);

		public:
			virtual ~IActiveEventLoop() = default;

		public:
			virtual IEventLoop* GetActiveEventLoop() = 0;
			virtual void SetActiveEventLoop(IEventLoop* eventLoop) = 0;

			virtual void WakeUp() = 0;
			virtual bool Pending() = 0;
			virtual bool Dispatch() = 0;
			virtual bool DispatchIdle() = 0;
			virtual bool Yield(FlagSet<EventYieldFlag> flags) = 0;
	};

	class KXF_API IPendingEvents: public RTTI::Interface<IPendingEvents>
	{
		kxf_RTTI_DeclareIID(IPendingEvents, {0xf3c249fd, 0xf2ca, 0x41af, {0x85, 0x31, 0x5a, 0x74, 0x3d, 0x78, 0x34, 0xb7}});

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
			virtual ~IPendingEvents() = default;

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
	};

	class KXF_API IExceptionHandler: public RTTI::Interface<IExceptionHandler>
	{
		kxf_RTTI_DeclareIID(IExceptionHandler, {0x924de29c, 0xfa90, 0x4ebe, {0xba, 0xbc, 0x33, 0x0, 0x10, 0x5, 0x9d, 0xa5}});

		public:
			virtual ~IExceptionHandler() = default;

		public:
			virtual bool OnMainLoopException() = 0;
			virtual void OnUnhandledException() = 0;
			virtual void OnFatalException() = 0;

			virtual bool StoreCurrentException() = 0;
			virtual void RethrowStoredException() = 0;
	};

	class KXF_API IDebugHandler: public RTTI::Interface<IDebugHandler>
	{
		kxf_RTTI_DeclareIID(IDebugHandler, {0x2165d85c, 0xe7ae, 0x4cc6, {0x88, 0x6f, 0x3f, 0x93, 0x8d, 0x9a, 0x1d, 0x7}});

		public:
			virtual ~IDebugHandler() = default;

		public:
			virtual void OnAssertFailure(const String& file, int line, const String& function, const String& condition, const String& message) = 0;
	};

	class KXF_API ICommandLine: public RTTI::Interface<ICommandLine>
	{
		kxf_RTTI_DeclareIID(ICommandLine, {0xbf92c139, 0xff8f, 0x4840, {0xa8, 0x9e, 0x9f, 0xd, 0x97, 0x3d, 0x8b, 0xf}});

		public:
			virtual ~ICommandLine() = default;

		public:
			virtual void InitializeCommandLine(char** argv, size_t argc) = 0;
			virtual void InitializeCommandLine(wchar_t** argv, size_t argc) = 0;

			virtual Enumerator<String> EnumCommandLineArgs() const = 0;
			virtual void OnCommandLineInit(CommandLineParser& parser) = 0;
			virtual bool OnCommandLineParsed(CommandLineParser& parser) = 0;
			virtual bool OnCommandLineError(CommandLineParser& parser) = 0;
			virtual bool OnCommandLineHelp(CommandLineParser& parser) = 0;
	};
}

namespace kxf
{
	class KXF_API ICoreApplication: public RTTI::ExtendInterface
		<
			ICoreApplication,
			IEvtHandler,
			Application::IBasicInfo,
			Application::IMainEventLoop,
			Application::IActiveEventLoop,
			Application::IPendingEvents,
			Application::IExceptionHandler,
			Application::IDebugHandler,
			Application::ICommandLine
		>
	{
		kxf_RTTI_DeclareIID(ICoreApplication, {0x2db9e5b5, 0x29cb, 0x4e8a, {0xb4, 0x59, 0x16, 0xee, 0xb, 0xad, 0x92, 0xdf}});

		public:
			kxf_EVENT_MEMBER(ActivateEvent, Activated);
			kxf_EVENT_MEMBER(FSActionEvent, ExecutingModuleWorkingDirectoryChanged);

		public:
			static ICoreApplication* GetInstance() noexcept;
			static void SetInstance(ICoreApplication* instance) noexcept;

		private:
			using IEvtHandler::ProcessPendingEvents;
			using IEvtHandler::DiscardPendingEvents;

		public:
			virtual ~ICoreApplication() = default;

		public:
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

			virtual IEvtHandler& GetEvtHandler() = 0;
			virtual IAsyncTaskExecutor& GetTaskExecutor() = 0;
			virtual const ILocalizationPackage& GetLocalizationPackage() const = 0;
	};
}
