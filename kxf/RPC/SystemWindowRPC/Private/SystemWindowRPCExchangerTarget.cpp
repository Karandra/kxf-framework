#include "kxf-pch.h"
#include "SystemWindowRPCExchangerTarget.h"
#include "SystemWindowRPCExchanger.h"
#include "kxf/IO/MemoryStream.h"
#include <Windows.h>

namespace kxf
{
	bool SystemWindowRPCExchangerTarget::Create(const String& sessionID)
	{
		return m_Window.Create([&](uint32_t msg, intptr_t wParam, intptr_t lParam) -> CallbackResult<intptr_t>
		{
			if (msg == WM_COPYDATA)
			{
				const COPYDATASTRUCT* copyData = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
				MemoryInputStream stream(copyData->lpData, copyData->cbData);
				m_Exchanger.OnDataRecieved(stream);

				return {CallbackCommand::Continue, TRUE};
			}
			return {CallbackCommand::Discard, 0};
		}, Format("kxf::SystemWindowRPCExchangerTarget-{}", sessionID));
	}
	bool SystemWindowRPCExchangerTarget::Destroy() noexcept
	{
		return m_Window.Destroy();
	}
}
