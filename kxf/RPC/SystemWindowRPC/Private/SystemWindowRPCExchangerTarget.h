#pragma once
#include "kxf/System/SystemWindow.h"
#include "kxf-gui/Widgets/Private/AnonymousNativeWindow.h"

namespace kxf
{
	class SystemWindowRPCExchanger;
}

namespace kxf
{
	class SystemWindowRPCExchangerTarget final
	{
		private:
			Private::AnonymousNativeWindow m_Window;
			SystemWindowRPCExchanger& m_Exchanger;

		public:
			SystemWindowRPCExchangerTarget(SystemWindowRPCExchanger& exchanger) noexcept
				:m_Exchanger(exchanger)
			{
			}

		public:
			SystemWindow GetWindow() const noexcept
			{
				return m_Window.GetWindow();
			}

			bool Create(const String& sessionID);
			bool Destroy() noexcept;
	};
}
