#pragma once
#include "Common.h"
#include "kxf/System/SystemWindow.h"
class wxWindow;
class wxNativeContainerWindow;

namespace kxf::Private
{
	class AnonymousNativeWindow final
	{
		private:
			std::move_only_function<bool(intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam)> m_MessageHandler;
			OwningSystemWindow m_Window;
			uint32_t m_WindowClass = 0;

			std::unique_ptr<wxNativeContainerWindow> m_WxWindow;

		private:
			bool HandleMessage(intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam) noexcept;

		public:
			AnonymousNativeWindow() noexcept;
			AnonymousNativeWindow(const AnonymousNativeWindow&) = delete;
			~AnonymousNativeWindow() noexcept;

		public:
			bool IsNull() const noexcept
			{
				return m_Window.IsNull();
			}
			void* GetHandle() const noexcept
			{
				return m_Window.GetHandle();
			}
			SystemWindow GetWindow() const noexcept
			{
				return m_Window;
			}
			wxWindow* GetWxWindow();

			bool Create(decltype(m_MessageHandler) messageHandler, const String& title = {});
			bool Create(decltype(m_MessageHandler) messageHandler, const String& title, FlagSet<uint32_t> style, FlagSet<uint32_t> exStyle);
			bool Destroy() noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			AnonymousNativeWindow& operator=(const AnonymousNativeWindow&) = delete;
	};
}
