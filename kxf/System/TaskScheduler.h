#pragma once
#include "Common.h"
#include "COM.h"
#include "kxf/Core/DateTime.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Crypto/SecretValue.h"
#include "kxf/Crypto/UserCredentials.h"
struct ITaskDefinition;
struct IRegistrationInfo;
struct IPrincipal;
struct ITaskSettings;
struct IIdleSettings;
struct ITriggerCollection;
struct IActionCollection;
struct IAction;
struct ITaskService;
struct ITaskFolder;

namespace kxf::System
{
	class KXF_API ScheduledTask final
	{
		friend class TaskScheduler;

		private:
			COMPtr<ITaskDefinition> m_Task;
			COMPtr<IRegistrationInfo> m_RegInfo;
			COMPtr<IPrincipal> m_Principal;
			COMPtr<ITaskSettings> m_Settings;
			COMPtr<IIdleSettings> m_IdleSettings;
			
			COMPtr<ITriggerCollection> m_TriggerCollection;
			COMPtr<IActionCollection> m_ActionCollection;

		private:
			ScheduledTask(COMPtr<ITaskDefinition> taskDefinition);

		public:
			ScheduledTask() noexcept;
			ScheduledTask(const ScheduledTask&) = delete;
			~ScheduledTask() noexcept;

		public:
			bool IsNull() const noexcept
			{
				return !m_Task || !m_RegInfo || !m_Principal || !m_Settings || !m_IdleSettings;
			}

			HResult SetExecutable(const FSPath& path, const String& arguments = {}, const FSPath& workingDirectory = {});
			HResult SetTimeTrigger(const String& id, const DateTime& start, const DateTime& end);
			HResult SetRegistrationTrigger(const String& id, const TimeSpan& delay, const DateTime& end = {});
			HResult DeleteExpiredTaskAfter(const TimeSpan& delay);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			ScheduledTask& operator=(const ScheduledTask&) = delete;
	};
}

namespace kxf::System
{
	class KXF_API TaskScheduler final
	{
		private:
			COMPtr<ITaskService> m_TaskService;
			COMPtr<ITaskFolder> m_TaskFolder;

		public:
			TaskScheduler(const FSPath& taskFolder = {},
						  const String& serverName = {},
						  const UserCredentials& userCredentials = {}
			);
			TaskScheduler(const TaskScheduler&) = delete;
			~TaskScheduler() noexcept;

		public:
			bool IsNull() const noexcept
			{
				return !m_TaskService || !m_TaskFolder;
			}

			ScheduledTask NewTask();
			HResult SaveTask(const ScheduledTask& task, const String& name);
			HResult DeleteTask(const String& taskName);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			TaskScheduler& operator=(const TaskScheduler&) = delete;
	};
}
