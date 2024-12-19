#pragma once
#include "ApplicationWrapper.h"
#include "kxf/Application/ICoreApplication.h"

namespace kxf::wxWidgets
{
	class KXF_API ApplicationWrapperConsole final: public ApplicationWrapperCommon<ApplicationWrapperConsole, ApplicationConsole>
	{
		public:
			ApplicationWrapperConsole(ICoreApplication& app)
				:ApplicationWrapperCommon(app)
			{
			}
	};
}
