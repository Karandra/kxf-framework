#include "KxfPCH.h"
#include "TaskScheduler.h"

#include <taskschd.h>
#include <wincred.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")

#include "Private/BeginIncludeCOM.h"
namespace
{
	Kx_MakeWinUnicodeCallWrapper(FormatMessage);
}
#include <comdef.h>
#include "Private/EndIncludeCOM.h"

namespace
{
	_variant_t VariantFromString(const kxf::String& value)
	{
		return value.wc_str().data();
	}
	_variant_t VariantFromString_Null(const kxf::String& value)
	{
		return value.IsEmpty() ? _variant_t() : VariantFromString(value);
	}

	_bstr_t BstrFromString(const kxf::String& value)
	{
		return value.wc_str().data();
	}
	_bstr_t BstrFromString_Null(const kxf::String& value)
	{
		return value.IsEmpty() ? _bstr_t() : BstrFromString(value);
	}

	kxf::String FormatTimeAsTaskBoundary(const kxf::DateTime& date)
	{
		return date.FormatISOCombined();
	}
	kxf::String FormatTimeAsTaskDelay(const kxf::TimeSpan& delay)
	{
		return delay.Format("PT%HH%MM%SS");
	}
}

namespace kxf::System
{
	ScheduledTask::ScheduledTask() noexcept
	{
	}
	ScheduledTask::ScheduledTask(COMPtr<ITaskDefinition> taskDefinition)
		:m_Task(std::move(taskDefinition))
	{
		if (m_Task)
		{
			// Get the registration info for setting the identification.
			m_Task->get_RegistrationInfo(&m_RegInfo);

			// Create the principal for the task - these credentials
			// are overwritten with the credentials passed to 'RegisterTaskDefinition'
			m_Task->get_Principal(&m_Principal);

			// Create the settings for the task
			m_Task->get_Settings(&m_Settings);
			if (m_Settings)
			{
				// Create the settings for the task
				m_Settings->get_IdleSettings(&m_IdleSettings);
			}

			// Get the trigger collection to insert the time trigger
			m_Task->get_Triggers(&m_TriggerCollection);

			// Get the task action collection pointer
			m_Task->get_Actions(&m_ActionCollection);
		}
	}
	ScheduledTask::~ScheduledTask() noexcept
	{
	}

	HResult ScheduledTask::SetExecutable(const FSPath& path, const String& arguments, const FSPath& workingDirectory)
	{
		HResult hr = E_INVALIDARG;

		if (m_ActionCollection)
		{
			COMPtr<IAction> action;
			if (hr = m_ActionCollection->Create(TASK_ACTION_EXEC, &action))
			{
				COMPtr<IExecAction> execAction;
				if (hr = action->QueryInterface(&execAction))
				{
					hr = execAction->put_Path(BstrFromString(path.GetFullPath()));
					if (!hr)
					{
						return hr;
					}

					if (hr && !arguments.IsEmpty())
					{
						hr = execAction->put_Arguments(BstrFromString_Null(arguments));
					}

					if (hr && workingDirectory)
					{
						hr = execAction->put_WorkingDirectory(BstrFromString_Null(workingDirectory.GetFullPath()));
					}
				}
			}
		}
		return hr;
	}
	HResult ScheduledTask::SetTimeTrigger(const String& id, const DateTime& start, const DateTime& end)
	{
		HResult hr = E_INVALIDARG;

		if (m_TriggerCollection)
		{
			// Add the time trigger to the task
			COMPtr<ITrigger> trigger;
			if (hr = m_TriggerCollection->Create(TASK_TRIGGER_TIME, &trigger))
			{
				COMPtr<ITimeTrigger> timeTrigger;
				if (hr = trigger->QueryInterface(&timeTrigger))
				{
					timeTrigger->put_Id(BstrFromString(id));

					// Set the task to start at a certain time. The time format should be YYYY-MM-DDTHH:MM:SS(+-)(timezone).
					if (hr = timeTrigger->put_StartBoundary(BstrFromString(FormatTimeAsTaskBoundary(start))))
					{
						if (end.IsValid())
						{
							hr = timeTrigger->put_EndBoundary(BstrFromString(FormatTimeAsTaskBoundary(end)));
						}
					}
				}
			}
		}
		return hr;
	}
	HResult ScheduledTask::SetRegistrationTrigger(const String& id, const TimeSpan& delay, const DateTime& end)
	{
		HResult hr = E_INVALIDARG;

		if (m_TriggerCollection)
		{
			// Add the registration trigger to the task.
			COMPtr<ITrigger> trigger;
			if (hr = m_TriggerCollection->Create(TASK_TRIGGER_REGISTRATION, &trigger))
			{
				COMPtr<IRegistrationTrigger> regTrigger;
				if (hr = trigger->QueryInterface(&regTrigger))
				{
					regTrigger->put_Id(BstrFromString(id));

					if (hr = regTrigger->put_Delay(BstrFromString(FormatTimeAsTaskDelay(delay))))
					{
						if (end.IsValid())
						{
							hr = regTrigger->put_EndBoundary(BstrFromString(FormatTimeAsTaskBoundary(end + delay)));
						}
					}
					return hr;
				}
			}
		}
		return hr;
	}
	HResult ScheduledTask::DeleteExpiredTaskAfter(const TimeSpan& delay)
	{
		return m_Settings->put_DeleteExpiredTaskAfter(BstrFromString(FormatTimeAsTaskDelay(delay)));
	}
}

namespace kxf::System
{
	TaskScheduler::TaskScheduler(const FSPath& taskFolder,
								 const String& serverName,
								 const UserCredentials& userCredentials
	)
	{
		// https://docs.microsoft.com/en-us/windows/desktop/TaskSchd/starting-an-executable-at-a-spcific-time
		// Doesn't work, no idea why. Doesn't seems to be required for the task scheduler to work.
		//HRESULT res = ::CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, 0, nullptr);

		if (HResult hr = COM::CreateInstance(CLSID_TaskScheduler, COMClassContext::InprocServer, &m_TaskService))
		{
			hr = m_TaskService->Connect(VariantFromString_Null(serverName),
										VariantFromString_Null(userCredentials.GetName()),
										VariantFromString_Null(userCredentials.GetDomain()),
										VariantFromString_Null(userCredentials.GetSecret().ToString())
			);
			if (hr)
			{
				m_TaskService->GetFolder(BstrFromString(!taskFolder ? '\\' : taskFolder.GetFullPath()), &m_TaskFolder);
			}
		}
	}
	TaskScheduler::~TaskScheduler() noexcept
	{
	}

	ScheduledTask TaskScheduler::NewTask()
	{
		COMPtr<ITaskDefinition> taskDefinition;
		if (HResult(m_TaskService->NewTask(0, &taskDefinition)))
		{
			return ScheduledTask(std::move(taskDefinition));
		}
		return {};
	}
	HResult TaskScheduler::SaveTask(const ScheduledTask& task, const String& name)
	{
		COMPtr<IRegisteredTask> registeredTask;
		HResult hr = m_TaskFolder->RegisterTaskDefinition(BstrFromString(name),
														  task.m_Task.Get(),
														  TASK_CREATE_OR_UPDATE,
														  _variant_t(),
														  _variant_t(),
														  TASK_LOGON_INTERACTIVE_TOKEN,
														  _variant_t(L""),
														  &registeredTask
		);
		return hr && registeredTask;
	}
	HResult TaskScheduler::DeleteTask(const String& taskName)
	{
		return m_TaskFolder->DeleteTask(BstrFromString(taskName), 0);
	}
}
