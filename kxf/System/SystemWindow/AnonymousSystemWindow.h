#pragma once
#include "SystemWindow.h"
#include "kxf/Core/CallbackFunction.h"

namespace kxf
{
	class AnonymousSystemWindow: public SystemWindow
	{
		protected:
			uint32_t m_WindowClass = 0;
			std::move_only_function<CallbackResult<intptr_t>(uint32_t msg, intptr_t wParam, intptr_t lParam)> m_MessageHandler;

		protected:
			virtual void CleanUp();
			CallbackResult<intptr_t> HandleMessage(uint32_t msg, intptr_t wParam, intptr_t lParam) noexcept;

		public:
			AnonymousSystemWindow() noexcept = default;
			AnonymousSystemWindow(const AnonymousSystemWindow&) = delete;
			AnonymousSystemWindow(AnonymousSystemWindow&& other) noexcept
			{
				*this = std::move(other);
			}
			~AnonymousSystemWindow()
			{
				SystemWindow::Destroy();
				CleanUp();
			}

		public:
			// ISystemWindow
			bool Close() noexcept override;
			bool Destroy() noexcept override;

			// AnonymousSystemWindow
			bool Create(decltype(m_MessageHandler) messageHandler, const String& title = {});
			bool Create(decltype(m_MessageHandler) messageHandler, const String& title, FlagSet<uint32_t> style, FlagSet<uint32_t> exStyle);

		public:
			AnonymousSystemWindow& operator=(const AnonymousSystemWindow&) = delete;
			AnonymousSystemWindow& operator=(AnonymousSystemWindow&& other) noexcept
			{
				if (this != &other)
				{
					m_Handle = std::exchange(other.m_Handle, nullptr);
					m_WindowClass = std::exchange(other.m_WindowClass, 0);
					m_MessageHandler = std::move(other.m_MessageHandler);
				}
				return *this;
			}
	};
}
