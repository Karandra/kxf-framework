#pragma once
#include "Common.h"
#include "kxf/System/SystemWindow/AnonymousSystemWindow.h"
class wxWindow;
class wxNativeContainerWindow;

namespace kxf::Private
{
	class AnonymousNativeWindow final: public AnonymousSystemWindow
	{
		private:
			std::unique_ptr<wxNativeContainerWindow> m_WxWindow;

		private:
			void CleanUp() override;

		public:
			AnonymousNativeWindow() noexcept;
			using AnonymousSystemWindow::AnonymousSystemWindow;

		public:
			wxWindow* GetWxWindow();
	};
}
