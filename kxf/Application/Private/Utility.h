#pragma once
#include "../Common.h"
#include "kxf/Utility/Container.h"
class wxWindow;

namespace kxf::Application::Private
{
	void OnUnhandledException();
	void OnFatalException();

	bool OnMainLoopException();
	bool OnMainLoopExceptionGUI();
}

namespace kxf::Application::Private
{
	template<class TContainer>
	bool IsWindowInContainer(const TContainer& container, wxWindow& window) noexcept
	{
		return kxf::Utility::Container::Contains(container, [&](const auto& item)
		{
			return item.get() == &window;
		});
	};
}
