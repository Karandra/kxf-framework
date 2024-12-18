#include "kxf-pch.h"
#include "ICoreApplication.h"

namespace
{
	std::atomic<kxf::ICoreApplication*> g_AppInstance = nullptr;
}


namespace kxf::Application
{
	void IActiveEventLoop::CallOnEnterEventLoop(IEventLoop& eventLoop)
	{
		eventLoop.OnEnter();
	}
	void IActiveEventLoop::CallOnExitEventLoop(IEventLoop& eventLoop)
	{
		eventLoop.OnExit();
	}
}

namespace kxf
{
	ICoreApplication* ICoreApplication::GetInstance() noexcept
	{
		return g_AppInstance;
	}
	void ICoreApplication::SetInstance(ICoreApplication* instance) noexcept
	{
		g_AppInstance = instance;
	}
}
