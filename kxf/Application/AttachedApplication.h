#pragma once
#include "Common.h"
#include "CoreApplication.h"

namespace kxf
{
	class KXF_API AttachedApplication: public RTTI::Implementation<AttachedApplication, CoreApplication>
	{
		public:
			AttachedApplication();
			~AttachedApplication();

		public:
			// Application::IMainEventLoop
			std::shared_ptr<IEventLoop> CreateMainLoop() override;
	};
}
