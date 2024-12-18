#include "kxf-pch.h"
#include "AttachedApplication.h"

namespace kxf
{
	AttachedApplication::AttachedApplication()
	{
		m_CommandLineParser.FindCommandLine();
		ICoreApplication::SetInstance(this);
	}
	AttachedApplication::~AttachedApplication()
	{
		ICoreApplication::SetInstance(nullptr);
	}

	// ICoreApplication -> Main Event Loop
	std::shared_ptr<IEventLoop> AttachedApplication::CreateMainLoop()
	{
		return nullptr;
	}
}
