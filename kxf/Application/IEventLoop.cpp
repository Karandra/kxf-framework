#include "kxf-pch.h"
#include "IEventLoop.h"
#include "kxf/Application/ICoreApplication.h"

namespace
{
	std::atomic<size_t> g_EventLoopCount = 0;
}

namespace kxf
{
	size_t IEventLoop::GetEventLoopCount() noexcept
	{
		return g_EventLoopCount;
	}

	void IEventLoop::OnEnter()
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			app->OnEventLoopEnter(*this);
		}
	}
	void IEventLoop::OnExit()
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			app->OnEventLoopExit(*this);
		}
	}

	IEventLoop::IEventLoop()
	{
		++g_EventLoopCount;
	}
	IEventLoop::~IEventLoop()
	{
		if (--g_EventLoopCount == 0)
		{
			if (auto app = ICoreApplication::GetInstance())
			{
				app->SetActiveEventLoop(nullptr);
			}
		}
	}

	bool IEventLoop::IsMain() const
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			return app->GetMainLoop() == this;
		}
		return false;
	}
	bool IEventLoop::IsRunning() const
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			return app->GetActiveEventLoop() == this;
		}
		return false;
	}
}

namespace kxf
{
	EventLoopActivator::EventLoopActivator(IEventLoop& eventLoop) noexcept
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			m_PreviousLoop = app->GetActiveEventLoop();
			app->SetActiveEventLoop(&eventLoop);
		}
	}
	EventLoopActivator::~EventLoopActivator() noexcept
	{
		if (m_PreviousLoop)
		{
			if (auto app = ICoreApplication::GetInstance())
			{
				app->SetActiveEventLoop(m_PreviousLoop);
			}
		}
	}
}
