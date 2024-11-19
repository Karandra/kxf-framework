#include "KxfPCH.h"
#include "CoroutineImpl.h"
#include "kxf/Application/ICoreApplication.h"
#include <chrono>

namespace kxf::Async
{
	void CoroutineTimer::OnNotify()
	{
		CoroutineBase::QueueExecution(std::move(m_Coroutine));
	}

	std::unique_ptr<CoroutineBase> CoroutineTimer::Relinquish()
	{
		Stop();
		return std::move(m_Coroutine);
	}
	void CoroutineTimer::Wait(std::unique_ptr<CoroutineBase> coroutine, const TimeSpan& time)
	{
		m_Coroutine = std::move(coroutine);
		Start(time);
	}
}

namespace kxf::Async
{
	CoroutineExecutor::CoroutineExecutor(std::unique_ptr<CoroutineBase> coroutine)
		:m_Coroutine(std::move(coroutine))
	{
	}

	void CoroutineExecutor::Execute()
	{
		m_Coroutine->RunExecute(std::move(m_Coroutine));
	}
}

namespace kxf::Async
{
	CoroutineBase* CoroutineBase::Run(std::unique_ptr<CoroutineBase> coroutine)
	{
		if (coroutine)
		{
			CoroutineBase& ref = *coroutine;
			QueueExecution(std::move(coroutine));
			return &ref;
		}
		return nullptr;
	}

	void CoroutineBase::QueueExecution(std::unique_ptr<CoroutineBase> coroutine)
	{
		ICoreApplication::GetInstance()->QueueEvent(std::make_unique<CoroutineExecutor>(std::move(coroutine)));
	}
	void CoroutineBase::DelayExecution(std::unique_ptr<CoroutineBase> coroutine, const TimeSpan& time)
	{
		if (time.IsPositive())
		{
			CoroutineBase& ref = *coroutine;
			ref.m_DelayTimer.Wait(std::move(coroutine), time);
		}
		else
		{
			QueueExecution(std::move(coroutine));
		}
	}
	void CoroutineBase::AbortExecution(std::unique_ptr<CoroutineBase> coroutine)
	{
		wxApp::GetInstance()->ScheduleForDestruction(coroutine.release());
	}

	void CoroutineBase::BeforeExecute()
	{
		// Save starting time
		if (m_TimeStampStart.IsNull())
		{
			m_TimeStampStart = TimeSpan::Now();
		}

		// Save before-after timestamps
		m_TimeStampBefore = GetCurrentExecutionTime();
		if (m_TimeStampAfter.IsNull())
		{
			m_TimeStampAfter = m_TimeStampBefore;
		}
	}
	void CoroutineBase::AfterExecute()
	{
		m_TimeStampAfter = GetCurrentExecutionTime();
	}
	void CoroutineBase::RunExecute(std::unique_ptr<CoroutineBase> coroutine)
	{
		if (m_Instruction.GetType() == InstructionType::Terminate)
		{
			AbortExecution(std::move(coroutine));
			return;
		}

		BeforeExecute();
		m_Instruction = Execute();
		AfterExecute();

		switch (m_Instruction.GetType())
		{
			case InstructionType::Delay:
			{
				DelayExecution(std::move(coroutine), m_Instruction.GetDelay());
				break;
			}
			case InstructionType::Continue:
			{
				QueueExecution(std::move(coroutine));
				break;
			}
			default:
			{
				AbortExecution(std::move(coroutine));
				break;
			}
		};
	}

	TimeSpan CoroutineBase::GetCurrentExecutionTime() const
	{
		return TimeSpan::Now() - m_TimeStampStart;
	}

	CoroutineBase::CoroutineBase()
		:m_Instruction(InstructionType::Continue)
	{
	}
	CoroutineBase::~CoroutineBase()
	{
	}

	void CoroutineBase::Terminate()
	{
		m_Instruction = CoroutineBase::YieldStop();

		if (m_DelayTimer.IsRunning())
		{
			AbortExecution(m_DelayTimer.Relinquish());
		}
	}
	
	TimeSpan CoroutineBase::GetTimeDelta() const
	{
		return m_TimeStampBefore - m_TimeStampAfter;
	}
	TimeSpan CoroutineBase::GetElapsedTime() const
	{
		return std::max(m_TimeStampBefore, m_TimeStampAfter);
	}
}
