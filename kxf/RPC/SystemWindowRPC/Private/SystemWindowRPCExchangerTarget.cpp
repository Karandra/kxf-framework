#include "kxf-pch.h"
#include "SystemWindowRPCExchangerTarget.h"
#include "SystemWindowRPCExchanger.h"
#include "kxf/IO/MemoryStream.h"
#include <Windows.h>

namespace kxf
{
	bool SystemWindowRPCExchangerTarget::Create(const String& sessionID)
	{
		return m_Window.Create([&](intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam) -> bool
		{
			if (msg == WM_COPYDATA)
			{
				const COPYDATASTRUCT* copyData = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
				MemoryInputStream stream(copyData->lpData, copyData->cbData);
				m_Exchanger.OnDataRecieved(stream);

				result = TRUE;
				return true;
			}
			return false;
		}, Format("kxf::SystemWindowRPCExchangerTarget-{}", sessionID));
	}
	bool SystemWindowRPCExchangerTarget::Destroy() noexcept
	{
		return m_Window.Destroy();
	}
}
