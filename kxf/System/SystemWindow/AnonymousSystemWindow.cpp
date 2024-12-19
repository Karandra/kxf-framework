#include "kxf-pch.h"
#include "AnonymousSystemWindow.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/System/Win32Error.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/Utility/ScopeGuard.h"

#include <Windows.h>
#include "kxf/Win32/UndefMacros.h"

namespace
{
	constexpr wchar_t g_WindowClassName[] = L"kxf::AnonymousSystemWindow";

	HMODULE GetCurrentModule() noexcept
	{
		return reinterpret_cast<HMODULE>(kxf::DynamicLibrary::GetCurrentModule().GetHandle());
	}
}

namespace kxf
{
	void AnonymousSystemWindow::CleanUp()
	{
		if (m_WindowClass != 0)
		{
			::UnregisterClassW(g_WindowClassName, GetCurrentModule());
			m_WindowClass = 0;
		}
		m_Handle = nullptr;
	}
	CallbackResult<intptr_t> AnonymousSystemWindow::HandleMessage(uint32_t msg, intptr_t wParam, intptr_t lParam) noexcept
	{
		Utility::ScopeGuard atExit = [&]()
		{
			if (msg == WM_DESTROY)
			{
				CleanUp();
			}
		};

		if (m_MessageHandler)
		{
			try
			{
				return std::invoke(m_MessageHandler, msg, wParam, lParam);
			}
			catch (...)
			{
				if (auto app = ICoreApplication::GetInstance())
				{
					app->StoreCurrentException();
					app->OnUnhandledException();
				}
				else
				{
					Log::Critical(__FUNCTION__ ": unhandled exception occurred. WindowHandle=[{}], WindowClass=[{}]", m_Handle, m_WindowClass);
					std::terminate();
				}
			}
		}
		return {CallbackCommand::Discard, FALSE};
	}

	// ISystemWindow
	bool AnonymousSystemWindow::Close() noexcept
	{
		bool result = SystemWindow::Close();
		CleanUp();

		return result;
	}
	bool AnonymousSystemWindow::Destroy() noexcept
	{
		bool result = SystemWindow::Destroy();
		CleanUp();
		
		return result;
	}

	// AnonymousSystemWindow
	bool AnonymousSystemWindow::Create(decltype(m_MessageHandler) messageHandler, const String& title)
	{
		return Create(std::move(messageHandler), title, WS_MINIMIZE|WS_DISABLED, WS_EX_TRANSPARENT);
	}
	bool AnonymousSystemWindow::Create(decltype(m_MessageHandler) messageHandler, const String& title, FlagSet<uint32_t> style, FlagSet<uint32_t> exStyle)
	{
		if (!m_Handle)
		{
			m_MessageHandler = std::move(messageHandler);

			WNDCLASSEXW windowClass = {};
			windowClass.cbSize = sizeof(windowClass);
			windowClass.lpszClassName = g_WindowClassName;
			windowClass.hInstance = GetCurrentModule();
			windowClass.hbrBackground = static_cast<HBRUSH>(::GetStockObject(GRAY_BRUSH));
			windowClass.lpfnWndProc = [](HWND handle, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				SystemWindow window(handle);

				if (auto self = reinterpret_cast<AnonymousSystemWindow*>(window.GetValue(GWLP_USERDATA).value_or(0)))
				{
					auto result = self->HandleMessage(msg, wParam, lParam);
					if (result.GetLastCommand() == CallbackCommand::Continue)
					{
						return result.Take();
					}
					else if (result.GetLastCommand() == CallbackCommand::Terminate)
					{
						self->Destroy();
						return FALSE;
					}
				}
				return ::DefWindowProcW(handle, msg, wParam, lParam);
			};
			m_WindowClass = ::RegisterClassExW(&windowClass);

			if (m_WindowClass != 0)
			{
				const wchar_t* windowTitle = !title.IsEmpty() ? title.wc_str() : windowClass.lpszClassName;
				if (m_Handle = ::CreateWindowExW(*exStyle, windowClass.lpszClassName, windowTitle, *style, 100, 100, 512, 256, nullptr, nullptr, windowClass.hInstance, nullptr))
				{
					SetValue(GWLP_USERDATA, reinterpret_cast<intptr_t>(this));
					SendMessage(WM_SETREDRAW, FALSE, 0);

					return true;
				}
			}
		}
		return false;
	}
}
