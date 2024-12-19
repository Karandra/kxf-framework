#include "kxf-pch.h"
#include "CoreApplication.h"
#include "Private/Utility.h"
#include "Private/Win32ConsoleEventLoop.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/EventSystem/IEventExecutor.h"
#include "kxf/EventSystem/IdleEvent.h"
#include "kxf/System/NativeAPI.h"
#include "kxf/System/NtStatus.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/System/DynamicLibraryEvent.h"
#include "kxf/Utility/Container.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/wxWidgets/ApplicationWrapper.h"
#include "kxf/wxWidgets/ApplicationWrapperConsole.h"
#include "kxf/wxWidgets/EvtHandlerWrapper.h"
#include "kxf/wxWidgets/Setup.h"

namespace
{
	enum
	{
		LDR_DLL_NOTIFICATION_REASON_LOADED = 1,
		LDR_DLL_NOTIFICATION_REASON_UNLOADED = 2
	};

	struct UNICODE_STRING final
	{
		USHORT Length = 0;
		USHORT MaximumLength = 0;
		PWCH Buffer = nullptr;
	};
	struct LDR_DLL_LOADED_NOTIFICATION_DATA final
	{
		ULONG Flags = 0;
		const UNICODE_STRING* FullDllName = nullptr;
		const UNICODE_STRING* BaseDllName = nullptr;
		void* DllBase = nullptr;
		ULONG SizeOfImage = 0;
	};
	struct LDR_DLL_UNLOADED_NOTIFICATION_DATA final
	{
		ULONG Flags = 0;
		const UNICODE_STRING* FullDllName = nullptr;
		const UNICODE_STRING* BaseDllName = nullptr;
		void* DllBase = nullptr;
		ULONG SizeOfImage = 0;
	};
	union LDR_DLL_NOTIFICATION_DATA final
	{
		LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
		LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
	};

	kxf::String FromUNICODE_STRING(const UNICODE_STRING& uniString)
	{
		return kxf::String(uniString.Buffer, uniString.Length / sizeof(*UNICODE_STRING::Buffer));
	}
}

namespace
{
	template<class TFunc>
	bool TryNativeAppValue(kxf::String& name, TFunc&& func)
	{
		if (auto app = wxAppConsole::GetInstance())
		{
			name = std::invoke(func, app);
			return !name.IsEmpty();
		}
		return false;
	}
}

namespace kxf::Private
{
	bool CoreApplicationEvtHandler::OnDynamicBind(EventItem& eventItem)
	{
		return m_App.CoreApplication::OnDynamicBind(eventItem);
	}
	bool CoreApplicationEvtHandler::OnDynamicUnbind(EventItem& eventItem)
	{
		return m_App.CoreApplication::OnDynamicUnbind(eventItem);
	}
}

namespace kxf
{
	// CoreApplication
	bool CoreApplication::InitDLLNotifications()
	{
		if (!NativeAPI::NtDLL::LdrRegisterDllNotification)
		{
			return false;
		}
		if (m_DLLNotificationsCookie)
		{
			return true;
		}
		
		NtStatus status = NativeAPI::NtDLL::LdrRegisterDllNotification(0, [](uint32_t reason, const void* data, void* context)
		{
			if (auto app = static_cast<CoreApplication*>(context))
			{
				EventID eventID;
				DynamicLibraryEvent event;
				auto SetParameters = [&](const auto& parameters)
				{
					event.SetLibrary(parameters.DllBase);
					if (parameters.BaseDllName)
					{
						event.SetBaseName(FromUNICODE_STRING(*parameters.BaseDllName));
					}
					if (parameters.FullDllName)
					{
						event.SetFullPath(FromUNICODE_STRING(*parameters.FullDllName));
					}
				};

				switch (reason)
				{
					case LDR_DLL_NOTIFICATION_REASON_LOADED:
					{
						eventID = DynamicLibraryEvent::EvtLoaded;
						SetParameters(reinterpret_cast<const LDR_DLL_NOTIFICATION_DATA*>(data)->Loaded);
						break;
					}
					case LDR_DLL_NOTIFICATION_REASON_UNLOADED:
					{
						eventID = DynamicLibraryEvent::EvtUnloaded;
						SetParameters(reinterpret_cast<const LDR_DLL_NOTIFICATION_DATA*>(data)->Unloaded);
						break;
					}
				};

				if (eventID)
				{
					app->ProcessEvent(event, std::move(eventID), ProcessEventFlag::HandleExceptions);
				}
			}
		}, this, &m_DLLNotificationsCookie);
		return status.IsSuccess() && m_DLLNotificationsCookie;
	}
	void CoreApplication::UninitDLLNotifications()
	{
		if (NativeAPI::NtDLL::LdrUnregisterDllNotification && m_DLLNotificationsCookie)
		{
			NativeAPI::NtDLL::LdrUnregisterDllNotification(m_DLLNotificationsCookie);
		}
		m_DLLNotificationsCookie = nullptr;
	}

	// IEvtHandler
	bool CoreApplication::OnDynamicBind(EventItem& eventItem)
	{
		if (!m_DLLNotificationsCookie)
		{
			if (eventItem.IsSameEventID(DynamicLibraryEvent::EvtLoaded) || eventItem.IsSameEventID(DynamicLibraryEvent::EvtUnloaded))
			{
				return InitDLLNotifications();
			}
		}
		if (auto app = wxAppConsole::GetInstance())
		{
			wxWidgets::ForwardBind(*this, *app, eventItem);
		}
		return true;
	}
	bool CoreApplication::OnDynamicUnbind(EventItem& eventItem)
	{
		return false;
	}

	// IObject
	RTTI::QueryInfo CoreApplication::DoQueryInterface(const IID& iid) noexcept
	{
		if (auto app = wxAppConsole::GetInstance())
		{
			if (iid.IsOfType<wxWidgets::ApplicationConsole>())
			{
				if (auto appConsole = dynamic_cast<wxWidgets::ApplicationConsole*>(app))
				{
					return appConsole->QueryInterface(iid);
				}
				return nullptr;
			}
			else if (iid.IsOfType<wxWidgets::ApplicationGUI>())
			{
				if (auto appGUI = dynamic_cast<wxWidgets::ApplicationGUI*>(app))
				{
					return appGUI->QueryInterface(iid);
				}
				return nullptr;
			}
		}
		return TBaseClass::DoQueryInterface(iid);
	}

	// ICoreApplication -> Basic Info
	String CoreApplication::GetName() const
	{
		if (!m_Name.IsEmpty())
		{
			return m_Name;
		}
		else if (String name; TryNativeAppValue(name, &wxAppConsole::GetAppName))
		{
			return name;
		}
		return DynamicLibrary::GetCurrentModule().GetFilePath().GetName();
	}
	void CoreApplication::SetName(const String& name)
	{
		m_Name = name;
		if (auto app = wxAppConsole::GetInstance())
		{
			app->SetAppName(name);
		}
	}

	String CoreApplication::GetDisplayName() const
	{
		if (!m_DisplayName.IsEmpty())
		{
			return m_DisplayName;
		}
		else if (!m_Name.IsEmpty())
		{
			return m_Name;
		}
		return GetName().MakeCapitalized();
	}
	void CoreApplication::SetDisplayName(const String& name)
	{
		m_DisplayName = name;
		if (auto app = wxAppConsole::GetInstance())
		{
			app->SetAppDisplayName(name);
		}
	}

	String CoreApplication::GetVendorName() const
	{
		if (String name; TryNativeAppValue(name, &wxAppConsole::GetVendorName))
		{
			return name;
		}
		return m_VendorName;
	}
	void CoreApplication::SetVendorName(const String& name)
	{
		m_VendorName = name;
		if (auto app = wxAppConsole::GetInstance())
		{
			app->SetVendorName(name);
		}
	}

	String CoreApplication::GetVendorDisplayName() const
	{
		if (String name; TryNativeAppValue(name, &wxAppConsole::GetVendorDisplayName))
		{
			return name;
		}
		return !m_VendorDisplayName.IsEmpty() ? m_VendorDisplayName : m_VendorName;
	}
	void CoreApplication::SetVendorDisplayName(const String& name)
	{
		m_VendorDisplayName = name;
		if (auto app = wxAppConsole::GetInstance())
		{
			app->SetVendorDisplayName(name);
		}
	}

	Version CoreApplication::GetVersion() const
	{
		return m_Version;
	}
	void CoreApplication::SetVersion(const Version& version)
	{
		m_Version = version;
	}

	String CoreApplication::GetClassName() const
	{
		if (!m_ClassName.IsEmpty())
		{
			return m_ClassName;
		}
		if (String name; TryNativeAppValue(name, &wxAppConsole::GetClassName))
		{
			return name;
		}
		return {};
	}
	void CoreApplication::SetClassName(const String& name)
	{
		m_ClassName = name;
		if (auto app = wxAppConsole::GetInstance())
		{
			app->SetClassName(name);
		}
	}

	// ICoreApplication -> Main Event Loop
	std::shared_ptr<IEventLoop> CoreApplication::CreateMainLoop()
	{
		return std::make_shared<kxf::EventSystem::Private::Win32ConsoleEventLoop>();
	}
	void CoreApplication::ExitMainLoop(int exitCode)
	{
		KXF_SCOPEDLOG_ARGS(exitCode);

		// We should exit from the main event loop, not just any currently active (e.g. modal dialog) event loop
		m_ExitCode = exitCode;
		if (m_MainLoop && m_MainLoop->IsRunning())
		{
			m_MainLoop->Exit(exitCode);
		}
	}

	// ICoreApplication -> Active Event Loop
	IEventLoop* CoreApplication::GetActiveEventLoop()
	{
		return m_ActiveEventLoop;
	}
	void CoreApplication::SetActiveEventLoop(IEventLoop* eventLoop)
	{
		m_ActiveEventLoop = eventLoop;
		if (eventLoop)
		{
			wxEventLoopBase::SetActive(static_cast<wxEventLoopBase*>(eventLoop->GetHandle()));
			ICoreApplication::CallOnEnterEventLoop(*eventLoop);
		}
	}

	void CoreApplication::WakeUp()
	{
		if (IEventLoop* eventLoop = GetActiveEventLoop())
		{
			eventLoop->WakeUp();
		}
	}
	bool CoreApplication::Pending()
	{
		if (IEventLoop* eventLoop = GetActiveEventLoop())
		{
			return eventLoop->Pending();
		}
		return false;
	}
	bool CoreApplication::Dispatch()
	{
		if (IEventLoop* eventLoop = GetActiveEventLoop())
		{
			return eventLoop->Dispatch();
		}
		return false;
	}
	bool CoreApplication::DispatchIdle()
	{
		bool needMore = false;
		if (auto app = wxAppConsole::GetInstance())
		{
			needMore = app->wxAppConsole::ProcessIdle();
		}

		// Synthesize an idle event and check if more of them are needed
		IdleEvent& event = BuildProcessEvent(IdleEvent::EvtIdle)
			.SetSourceToSelf()
			.Process()
			.GetEvent();

		// Flush the logged messages if any (do this after processing the events which could have logged new messages)
		wxLog::FlushActive();

		// Garbage collect all objects previously scheduled for destruction
		FinalizeScheduledForDestruction();

		return needMore || event.IsMoreRequested();
	}
	bool CoreApplication::Yield(FlagSet<EventYieldFlag> flags)
	{
		if (IEventLoop* activeEventLoop = GetActiveEventLoop())
		{
			return activeEventLoop->Yield(flags);
		}
		else if (auto tempEventLoop = CreateMainLoop())
		{
			return tempEventLoop->Yield(flags);
		}
		return false;
	}

	// ICoreApplication -> Pending Events
	bool CoreApplication::IsPendingEventHandlerProcessingEnabled() const
	{
		return m_PendingEventsProcessingEnabled;
	}
	void CoreApplication::EnablePendingEventHandlerProcessing(bool enable)
	{
		m_PendingEventsProcessingEnabled = enable;

		if (auto app = wxAppConsole::GetInstance())
		{
			if (enable)
			{
				app->ResumeProcessingOfPendingEvents();
			}
			else
			{
				app->SuspendProcessingOfPendingEvents();
			}
		}
	}

	void CoreApplication::ScheduleForDestruction(std::shared_ptr<IObject> object)
	{
		// We need an active event loop to schedule deletion. If no active loop present the object is deleted immediately.
		if (object)
		{
			WriteLockGuard lock(m_ScheduledForDestructionLock);

			if (!Utility::Container::Contains(m_ScheduledForDestruction, [&](const auto& item)
			{
				return item.get() == object.get();
			}))
			{
				m_ScheduledForDestruction.emplace_back(std::move(object));
			}
		}
	}
	bool CoreApplication::IsScheduledForDestruction(const IObject& object) const
	{
		ReadLockGuard lock(m_ScheduledForDestructionLock);

		return Utility::Container::Contains(m_ScheduledForDestruction, [&](const auto& item)
		{
			return item.get() == &object;
		});
	}
	void CoreApplication::FinalizeScheduledForDestruction()
	{
		if (auto app = wxWidgets::ApplicationWrapperConsole::GetInstance())
		{
			app->DeletePendingObjects();
		}

		if (WriteLockGuard lock(m_ScheduledForDestructionLock); !m_ScheduledForDestruction.empty())
		{
			m_ScheduledForDestruction.clear();
		}
	}

	void CoreApplication::AddPendingEventHandler(IEvtHandler& evtHandler)
	{
		WriteLockGuard lock(m_PendingEvtHandlersLock);

		if (!Utility::Container::Contains(m_PendingEvtHandlers, [&](const IEvtHandler* item)
		{
			return item == &evtHandler;
		}))
		{
			m_PendingEvtHandlers.emplace_back(&evtHandler);
		}
	}
	bool CoreApplication::RemovePendingEventHandler(IEvtHandler& evtHandler)
	{
		size_t count = 0;
		auto Find = [&](const IEvtHandler* item)
		{
			if (item == &evtHandler)
			{
				count++;
				return true;
			}
			return false;
		};

		// Try to remove the handler from both lists 
		if (WriteLockGuard lock(m_PendingEvtHandlersLock); true)
		{
			Utility::Container::RemoveSingleIf(m_PendingEvtHandlers, Find);
			Utility::Container::RemoveSingleIf(m_DelayedPendingEvtHandlers, Find);
		}
		return count != 0;
	}
	void CoreApplication::DelayPendingEventHandler(IEvtHandler& evtHandler)
	{
		// Move the handler from the list of handlers with processable pending events
		// to the list of handlers with pending events which needs to be processed later.
		auto Find = [&](const IEvtHandler* item)
		{
			return item == &evtHandler;
		};

		if (WriteLockGuard lock(m_PendingEvtHandlersLock); true)
		{
			Utility::Container::RemoveSingleIf(m_PendingEvtHandlers, Find);
			if (!Utility::Container::Contains(m_DelayedPendingEvtHandlers, Find))
			{
				m_DelayedPendingEvtHandlers.emplace_back(&evtHandler);
			}
		}
	}

	bool CoreApplication::ProcessPendingEventHandlers()
	{
		if (m_PendingEventsProcessingEnabled)
		{
			size_t count = 0;
			if (auto app = wxAppConsole::GetInstance())
			{
				// We're calling the base class version because the version from 'NativeApp' calls us in the overridden version.
				if (app->wxAppConsole::HasPendingEvents())
				{
					count++;
				}
				app->wxAppConsole::ProcessPendingEvents();
			}

			WriteLockGuard lock(m_PendingEvtHandlersLock);

			// This helper list should be empty
			if (m_DelayedPendingEvtHandlers.empty())
			{
				// Iterate until the list becomes empty: the handlers remove themselves from it when they don't have any more pending events
				while (!m_PendingEvtHandlers.empty())
				{
					// In 'IEvtHandler::ProcessPendingEvents', new handlers might be added and we are required to unlock the lock here.
					IEvtHandler* evtHandler = m_PendingEvtHandlers.front();

					// This inner unlock-relock must be consistent with the outer guard.
					m_PendingEvtHandlersLock.UnlockWrite();
					Utility::ScopeGuard relock = [&]()
					{
						m_PendingEvtHandlersLock.LockWrite();
					};

					if (OnPendingEventHandlerProcess(*evtHandler))
					{
						// We always call 'IEvtHandler::ProcessPendingEvents' on the first event handler with pending events because handlers
						// auto-remove themselves from this list (see 'RemovePendingEventHandler') if they have no more pending events.
						if (evtHandler->ProcessPendingEvents())
						{
							count++;
						}
					}
					else
					{
						// Remove the handler from pending events, but don't process any of its events
						if (RemovePendingEventHandler(*evtHandler))
						{
							count++;
						}
					}
				}

				// Now the 'm_PendingEvtHandlers' is surely empty, however some event handlers may have moved themselves into
				// 'm_DelayedPendingEvtHandlers' because of a selective 'Yield' call in progress. Now we need to move them back
				// to 'm_PendingEvtHandlersLock' so the next call to this function has the chance of processing them.
				if (!m_DelayedPendingEvtHandlers.empty())
				{
					m_PendingEvtHandlers.insert(m_PendingEvtHandlers.end(), m_DelayedPendingEvtHandlers.begin(), m_DelayedPendingEvtHandlers.end());
					m_DelayedPendingEvtHandlers.clear();
				}
			}
			return count != 0;
		}
		return false;
	}
	size_t CoreApplication::DiscardPendingEventHandlers()
	{
		if (auto app = wxAppConsole::GetInstance())
		{
			app->DeletePendingEvents();
		}

		WriteLockGuard lock(m_PendingEvtHandlersLock);

		size_t count = 0;
		for (IEvtHandler* evtHandler: m_PendingEvtHandlers)
		{
			if (OnPendingEventHandlerDiscard(*evtHandler) && evtHandler->DiscardPendingEvents() != 0)
			{
				count++;
			}
		}
		m_PendingEvtHandlers.clear();
		return count;
	}

	// ICoreApplication -> Exception Handler
	bool CoreApplication::OnMainLoopException()
	{
		return Application::Private::OnMainLoopException();
	}
	void CoreApplication::OnFatalException()
	{
		Application::Private::OnFatalException();
	}
	void CoreApplication::OnUnhandledException()
	{
		Application::Private::OnUnhandledException();
	}

	bool CoreApplication::StoreCurrentException()
	{
		if (!m_StoredException)
		{
			m_StoredException = std::current_exception();
			return true;
		}
		else
		{
			// We can't store more than one exception currently: while we could support this by just
			// using a 'std::vector<std::exception_ptr>', it shouldn't be actually necessary because
			// we should never have more than one active exception anyhow.
			return false;
		}
	}
	void CoreApplication::RethrowStoredException()
	{
		if (std::exception_ptr storedException = std::move(m_StoredException))
		{
			std::rethrow_exception(storedException);
		}
	}

	// ICoreApplication -> Debug Handler
	void CoreApplication::OnAssertFailure(const String& file, int line, const String& function, const String& condition, const String& message)
	{
		// No need to do anything with it here, it's going to be logged by our log system
	}

	// ICoreApplication -> Command Line
	void CoreApplication::InitializeCommandLine(char** argv, size_t argc)
	{
		KXF_SCOPEDLOG_ARGS(argv, argc);

		m_ArgC = argc;
		m_ArgVA = argv;
		m_ArgVW = nullptr;

		m_CommandLineParser.SetCommandLine(argc, argv);
		OnCommandLineInit(m_CommandLineParser);

		KXF_SCOPEDLOG.SetSuccess();
	}
	void CoreApplication::InitializeCommandLine(wchar_t** argv, size_t argc)
	{
		KXF_SCOPEDLOG_ARGS(argv, argc);

		m_ArgC = argc;
		m_ArgVA = nullptr;
		m_ArgVW = argv;
		
		m_CommandLineParser.SetCommandLine(argc, argv);
		OnCommandLineInit(m_CommandLineParser);

		KXF_SCOPEDLOG.SetSuccess();
	}

	Enumerator<String> CoreApplication::EnumCommandLineArgs() const
	{
		if (m_CommandLineParser)
		{
			return m_CommandLineParser.EnumParameters();
		}
		return {};
	}
	void CoreApplication::OnCommandLineInit(CommandLineParser& parser)
	{
		if (!m_CommandLineParser)
		{
			return;
		}

		switch (m_CommandLineParser.Parse())
		{
			case CommandLineParserResult::Success:
			{
				OnCommandLineParsed(m_CommandLineParser);
				break;
			}
			case CommandLineParserResult::HelpRequested:
			{
				OnCommandLineHelp(m_CommandLineParser);
				break;
			}
			default:
			{
				OnCommandLineError(m_CommandLineParser);
				break;
			}
		};
	}
	bool CoreApplication::OnCommandLineParsed(CommandLineParser& parser)
	{
		return true;
	}
	bool CoreApplication::OnCommandLineError(CommandLineParser& parser)
	{
		if (parser)
		{
			parser.ShowUsage();
		}
		return false;
	}
	bool CoreApplication::OnCommandLineHelp(CommandLineParser& parser)
	{
		if (parser)
		{
			parser.ShowUsage();
		}
		return false;
	}

	// ICoreApplication -> Application
	bool CoreApplication::OnCreate()
	{
		KXF_SCOPEDLOG_FUNC;

		if (!m_NativeAppInitialized)
		{
			if (auto app = wxAppConsole::GetInstance())
			{
				int argc = m_ArgC;
				m_NativeAppInitialized = app->Initialize(argc, m_ArgVW);
				if (!m_NativeAppInitialized)
				{
					KXF_SCOPEDLOG.SetFail();
					return false;
				}
			}
		}

		KXF_SCOPEDLOG.SetSuccess();
		return true;
	}
	void CoreApplication::OnDestroy()
	{
		KXF_SCOPEDLOG_FUNC;

		UninitDLLNotifications();

		// Destroy the main loop
		m_MainLoop = nullptr;

		// Clean up any still pending objects. Normally there shouldn't any as we
		// already do this in 'OnExit', but this could happen if the user code has
		// somehow managed to create more of them since then or just forgot to call
		// the base class 'OnExit'.
		FinalizeScheduledForDestruction();

		if (m_NativeAppInitialized && !m_NativeAppCleanedUp)
		{
			if (auto app = wxAppConsole::GetInstance())
			{
				app->wxAppConsole::CleanUp();
				m_NativeAppCleanedUp = true;
			}
		}

		KXF_SCOPEDLOG.SetSuccess();
	}

	void CoreApplication::OnExit()
	{
		// Finalize scheduled objects if there are anything left
		FinalizeScheduledForDestruction();
	}
	int CoreApplication::OnRun()
	{
		KXF_SCOPEDLOG_FUNC;

		if (auto mainLoop = CreateMainLoop())
		{
			// Save the old main loop pointer (if any), create a new loop and run it.
			// At the end of the loop, restore the original main loop.
			std::swap(m_MainLoop, mainLoop);
			Utility::ScopeGuard atExit = [&]()
			{
				m_MainLoop = std::move(mainLoop);
			};

			// Here we're running our newly created main loop saved in place of 'm_MainLoop' pointer.
			int code = m_MainLoop->Run();

			KXF_SCOPEDLOG.LogReturn(code);
			return code;
		}

		KXF_SCOPEDLOG.LogReturn(m_ExitCode);
		return m_ExitCode.value_or(-1);
	}

	void CoreApplication::Exit(int exitCode)
	{
		KXF_SCOPEDLOG_ARGS(exitCode);

		if (m_MainLoop)
		{
			ExitMainLoop(exitCode);
		}
		else
		{
			m_ExitCode = exitCode;
			std::terminate();
		}

		KXF_SCOPEDLOG.SetSuccess();
	}

	void CoreApplication::AddEventFilter(std::shared_ptr<IEventFilter> eventFilter)
	{
		WriteLockGuard lock(m_EventFiltersLock);

		m_EventFilters.emplace_back(std::move(eventFilter));
	}
	void CoreApplication::RemoveEventFilter(IEventFilter& eventFilter)
	{
		WriteLockGuard lock(m_EventFiltersLock);

		m_EventFilters.remove_if([&](auto& item)
		{
			return item.get() == &eventFilter;
		});
	}
	IEventFilter::Result CoreApplication::FilterEvent(IEvent& event)
	{
		using Result = IEventFilter::Result;

		ReadLockGuard lock(m_EventFiltersLock);
		for (auto& eventFilter: m_EventFilters)
		{
			const Result result = eventFilter->FilterEvent(event);
			if (result != Result::Skip)
			{
				return result;
			}
		}

		// Do nothing by default if there are no event filters
		return Result::Skip;
	}

	std::shared_ptr<wxWidgets::Application> CoreApplication::CreateWXApp()
	{
		return std::make_shared<wxWidgets::ApplicationWrapperConsole>(*this);
	}
}
