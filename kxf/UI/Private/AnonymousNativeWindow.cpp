#include "kxf-pch.h"
#include "AnonymousNativeWindow.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/System/Win32Error.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/IO/MemoryStream.h"
#include <wx/nativewin.h>
#include <Windows.h>
#include <UxTheme.h>

namespace
{
	constexpr wchar_t g_WindowClassName[] = L"kxf::AnonymousNativeWindow";

	HMODULE GetCurrentModule() noexcept
	{
		return reinterpret_cast<HMODULE>(kxf::DynamicLibrary::GetCurrentModule().GetHandle());
	}
}

namespace kxf::Private
{
	bool AnonymousNativeWindow::HandleMessage(intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam) noexcept
	{
		if (msg == WM_DESTROY)
		{
			m_WxWindow = nullptr;
		}

		if (m_MessageHandler)
		{
			try
			{
				return std::invoke(m_MessageHandler, result, msg, wParam, lParam);
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
					Log::Critical(__FUNCTION__ ": Unhandled exception occurred");
					std::terminate();
				}
			}
		}
		return false;
	}

	AnonymousNativeWindow::AnonymousNativeWindow() noexcept = default;
	AnonymousNativeWindow::~AnonymousNativeWindow() noexcept
	{
		Destroy();
	}

	wxWindow* AnonymousNativeWindow::GetWxWindow()
	{
		if (!m_WxWindow && m_Window)
		{
			m_WxWindow = std::make_unique<wxNativeContainerWindow>(static_cast<HWND>(m_Window.GetHandle()));
		}
		return m_WxWindow.get();
	}

	bool AnonymousNativeWindow::Create(decltype(m_MessageHandler) messageHandler, const String& title)
	{
		return Create(std::move(messageHandler), title, WS_MINIMIZE|WS_DISABLED, WS_EX_TRANSPARENT);
	}
	bool AnonymousNativeWindow::Create(decltype(m_MessageHandler) messageHandler, const String& title, FlagSet<uint32_t> style, FlagSet<uint32_t> exStyle)
	{
		if (!m_Window)
		{
			m_MessageHandler = std::move(messageHandler);

			WNDCLASSEXW windowClass = {};
			windowClass.cbSize = sizeof(windowClass);
			windowClass.lpszClassName = g_WindowClassName;
			windowClass.hInstance = GetCurrentModule();
			windowClass.hbrBackground = static_cast<HBRUSH>(::GetStockObject(GRAY_BRUSH));
			windowClass.lpfnWndProc = [](HWND handle, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				if (auto window = reinterpret_cast<AnonymousNativeWindow*>(::GetWindowLongPtrW(handle, GWLP_USERDATA)))
				{
					intptr_t result = FALSE;
					if (window->HandleMessage(result, msg, wParam, lParam))
					{
						return result;
					}
				}
				return ::DefWindowProcW(handle, msg, wParam, lParam);
			};
			m_WindowClass = ::RegisterClassExW(&windowClass);

			if (m_WindowClass != 0)
			{
				const wchar_t* windowTitle = !title.IsEmpty() ? title.wc_str() : windowClass.lpszClassName;
				if (m_Window = ::CreateWindowExW(*exStyle, windowClass.lpszClassName, windowTitle, *style, 100, 100, 512, 256, nullptr, nullptr, windowClass.hInstance, nullptr))
				{
					m_Window.SetValue(GWLP_USERDATA, reinterpret_cast<intptr_t>(this));
					m_Window.SendMessage(WM_SETREDRAW, FALSE, 0);
					::SetWindowTheme(reinterpret_cast<HWND>(m_Window.GetHandle()), L"Explorer", nullptr);

					return true;
				}
			}
		}
		return false;
	}
	bool AnonymousNativeWindow::Destroy() noexcept
	{
		m_WxWindow = nullptr;
		bool destroyed = m_Window.Destroy();

		if (m_WindowClass != 0)
		{
			::UnregisterClassW(g_WindowClassName, GetCurrentModule());
			m_WindowClass = 0;
		}
		return destroyed;
	}
}
