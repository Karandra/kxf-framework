#pragma once
#include "Common.h"
#include "ICoreApplication.h"
#include "CommandLineParser.h"
#include "kxf/Core/Async/DefaultAsyncTaskExecutor.h"
#include "kxf/Threading/LockGuard.h"
#include "kxf/Threading/RecursiveRWLock.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/EvtHandlerAccessor.h"

namespace kxf
{
	class CoreApplication;
}
namespace kxf::Private
{
	class CoreApplicationEvtHandler final: public EvtHandler
	{
		private:
			CoreApplication& m_App;

		protected:
			// IEvtHandler
			bool OnDynamicBind(EventItem& eventItem) override;
			bool OnDynamicUnbind(EventItem& eventItem) override;

		public:
			CoreApplicationEvtHandler(CoreApplication& app) noexcept
				:m_App(app)
			{
			}

		public:
			auto Access() noexcept
			{
				return EventSystem::EvtHandlerAccessor(*this);
			}
	};
}

namespace kxf
{
	class KXF_API CoreApplication: public RTTI::Implementation<CoreApplication, ICoreApplication>
	{
		friend class Private::CoreApplicationEvtHandler;

		public:
			static CoreApplication* GetInstance() noexcept
			{
				return static_cast<CoreApplication*>(ICoreApplication::GetInstance());
			}
			static void SetInstance(CoreApplication* instance) noexcept
			{
				ICoreApplication::SetInstance(instance);
			}

		protected:
			// IEvtHandler
			Private::CoreApplicationEvtHandler m_EvtHandler;

			// ICoreApplication -> Basic Info
			String m_Name;
			String m_DisplayName;
			String m_VendorName;
			String m_VendorDisplayName;
			String m_ClassName;
			Version m_Version;

			// ICoreApplication -> Main Event Loop
			std::shared_ptr<IEventLoop> m_MainLoop;

			// ICoreApplication -> Active Event Loop
			IEventLoop* m_ActiveEventLoop = nullptr;

			// ICoreApplication -> Pending Events
			std::atomic<bool> m_PendingEventsProcessingEnabled = true;

			mutable RecursiveRWLock m_ScheduledForDestructionLock;
			std::vector<std::shared_ptr<IObject>> m_ScheduledForDestruction;

			mutable RecursiveRWLock m_PendingEvtHandlersLock;
			std::list<IEvtHandler*> m_PendingEvtHandlers;
			std::list<IEvtHandler*> m_DelayedPendingEvtHandlers;

			// ICoreApplication -> Exception Handler
			std::exception_ptr m_StoredException;

			// ICoreApplication -> Command Line
			CommandLineParser m_CommandLineParser;
			size_t m_ArgC = 0;
			char** m_ArgVA = nullptr;
			wchar_t** m_ArgVW = nullptr;

			// ICoreApplication -> Application
			mutable RecursiveRWLock m_EventFiltersLock;
			std::list<std::shared_ptr<IEventFilter>> m_EventFilters;

			DefaultAsyncTaskExecutor m_TaskExecutor;
			std::optional<int> m_ExitCode;

			bool m_NativeAppInitialized = false;
			bool m_NativeAppCleanedUp = false;

			// CoreApplication
			void* m_DLLNotificationsCookie = nullptr;

		protected:
			// IObject
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override;

			// IEvtHandler
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<BindEventFlag> flags = {}) override
			{
				return m_EvtHandler.Access().DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return m_EvtHandler.Access().DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return m_EvtHandler.Access().DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventItem& eventItem) override;
			bool OnDynamicUnbind(EventItem& eventItem) override;

			std::unique_ptr<IEvent> DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}) override
			{
				return m_EvtHandler.Access().DoQueueEvent(std::move(event), eventID, uuid, flags);
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) override
			{
				return m_EvtHandler.Access().DoProcessEvent(event, eventID, uuid, flags, onlyIn);
			}

			bool TryBefore(IEvent& event) override
			{
				return m_EvtHandler.Access().TryBefore(event);
			}
			bool TryAfter(IEvent& event) override
			{
				return m_EvtHandler.Access().TryAfter(event);
			}

		private:
			// CoreApplication
			bool InitDLLNotifications();
			void UninitDLLNotifications();

		public:
			CoreApplication()
				:m_EvtHandler(*this)
			{
			}

		public:
			// IEvtHandler
			bool ProcessPendingEvents() override
			{
				return m_EvtHandler.ProcessPendingEvents();
			}
			size_t DiscardPendingEvents() override
			{
				return m_EvtHandler.DiscardPendingEvents();
			}

			IEvtHandler* GetPrevHandler() const override
			{
				return m_EvtHandler.GetPrevHandler();
			}
			IEvtHandler* GetNextHandler() const override
			{
				return m_EvtHandler.GetNextHandler();
			}
			void SetPrevHandler(IEvtHandler* evtHandler) override
			{
				// Can't chain widgets
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				// Can't chain widgets
			}

			void Unlink() override
			{
				m_EvtHandler.Unlink();
			}
			bool IsUnlinked() const override
			{
				return m_EvtHandler.IsUnlinked();
			}

			bool IsEventProcessingEnabled() const override
			{
				return m_EvtHandler.IsEventProcessingEnabled();
			}
			void EnableEventProcessing(bool enable = true) override
			{
				m_EvtHandler.EnableEventProcessing(enable);
			}

		public:
			// ICoreApplication -> Basic Info
			String GetName() const override;
			void SetName(const String& name) override;

			String GetDisplayName() const override;
			void SetDisplayName(const String& name) override;

			String GetVendorName() const override;
			void SetVendorName(const String& name) override;

			String GetVendorDisplayName() const override;
			void SetVendorDisplayName(const String& name) override;

			Version GetVersion() const override;
			void SetVersion(const Version& version) override;

			String GetClassName() const override;
			void SetClassName(const String& name) override;

			// ICoreApplication -> Main Event Loop
			std::shared_ptr<IEventLoop> CreateMainLoop() override;
			IEventLoop* GetMainLoop() override
			{
				return m_MainLoop.get();
			}
			void ExitMainLoop(int exitCode = 0) override;

			void OnEventLoopEnter(IEventLoop& loop) override
			{
				// Nothing to do
			}
			void OnEventLoopExit(IEventLoop& loop) override
			{
				// Nothing to do
			}

			// ICoreApplication -> Active Event Loop
			IEventLoop* GetActiveEventLoop() override;
			void SetActiveEventLoop(IEventLoop* eventLoop) override;

			void WakeUp() override;
			bool Pending() override;
			bool Dispatch() override;
			bool DispatchIdle() override;
			bool Yield(FlagSet<EventYieldFlag> flags) override;

			// ICoreApplication -> Pending Events
			bool IsPendingEventHandlerProcessingEnabled() const override;
			void EnablePendingEventHandlerProcessing(bool enable = true) override;

			void AddPendingEventHandler(IEvtHandler& evtHandler) override;
			bool RemovePendingEventHandler(IEvtHandler& evtHandler) override;
			void DelayPendingEventHandler(IEvtHandler& evtHandler) override;

			bool ProcessPendingEventHandlers() override;
			size_t DiscardPendingEventHandlers() override;

			bool IsScheduledForDestruction(const IObject& object) const override;
			void ScheduleForDestruction(std::shared_ptr<IObject> object) override;
			void FinalizeScheduledForDestruction() override;

			// ICoreApplication -> Exception Handler
			bool OnMainLoopException() override;
			void OnFatalException() override;
			void OnUnhandledException() override;

			bool StoreCurrentException() override;
			void RethrowStoredException() override;

			// ICoreApplication -> Debug Handler
			void OnAssertFailure(const String& file, int line, const String& function, const String& condition, const String& message) override;

			// ICoreApplication -> Command Line
			void InitializeCommandLine(char** argv, size_t argc) override;
			void InitializeCommandLine(wchar_t** argv, size_t argc) override;

			Enumerator<String> EnumCommandLineArgs() const override;
			void OnCommandLineInit(CommandLineParser& parser) override;
			bool OnCommandLineParsed(CommandLineParser& parser) override;
			bool OnCommandLineError(CommandLineParser& parser) override;
			bool OnCommandLineHelp(CommandLineParser& parser) override;

			// ICoreApplication -> Application
			bool OnCreate() override;
			void OnDestroy() override;

			bool OnInit() override = 0;
			void OnExit() override;
			int OnRun() override;

			void Exit(int exitCode) override;
			std::optional<int> GetExitCode() const override
			{
				return m_ExitCode;
			}

			void AddEventFilter(std::shared_ptr<IEventFilter> eventFilter) override;
			void RemoveEventFilter(IEventFilter& eventFilter) override;
			IEventFilter::Result FilterEvent(IEvent& event) override;

			IAsyncTaskExecutor& GetTaskExecutor() override
			{
				return m_TaskExecutor;
			}
			std::shared_ptr<wxWidgets::Application> CreateWXApp() override;
	};
}
