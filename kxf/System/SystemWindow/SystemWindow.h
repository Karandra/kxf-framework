#pragma once
#include "ISystemWindow.h"
#include "../SystemProcess/RunningSystemProcess.h"

namespace kxf
{
	class KXF_API SystemWindow: public ISystemWindow
	{
		public:
			static SystemWindow GetShellWindow();
			static SystemWindow GetDesktopWindow();
			static SystemWindow GetBroadcastWindow();
			static SystemWindow GetForegroundWindow();
			static SystemWindow GetFocusedWindow();
			static SystemWindow GetActiveWindow();

			static SystemWindow GetWindowFromPoint(const Point& position);
			static SystemWindow GetWindowFromPhysicalPoint(const Point& position);

		protected:
			void* m_Handle = nullptr;

		public:
			SystemWindow(void* hwnd = nullptr)
				:m_Handle(hwnd)
			{
			}

		public:
			// ISystemThread
			bool IsNull() const override
			{
				return m_Handle == nullptr;
			}
			uint32_t GetID() const override;
			void* GetHandle() const override
			{
				return m_Handle;
			}
			SystemThread GetOwningThread() const override;
			SystemProcess GetOwningProcess() const override;

			String GetText() const override;
			bool SetText(const String& label) override;

			Rect GetRect() const override;
			Point ScreenToClient(const Point& point) const override;
			Point ClientToScreen(const Point& point) const override;
			Size MapPoints(Rect& rect, const ISystemWindow& targetWindow) const override;

			std::optional<intptr_t> GetValue(int index) const override;
			std::optional<intptr_t> SetValue(int index, intptr_t value) override;

			bool PostMessage(uint32_t message, intptr_t wParam, intptr_t lParam) override;
			std::optional<intptr_t> SendMessage(uint32_t message, intptr_t wParam, intptr_t lParam, FlagSet<uint32_t> flags = {}, TimeSpan timeout = {}) override;

			bool IsVisible() const;
			bool Show(SHWindowCommand command, bool async) override;
			bool Close() override;
			bool Destroy() override;

			bool IsEnabled() const override;
			bool SetEnabled(bool enable = true) override;

			// SystemWindow
			RunningSystemProcess OpenOwningProcess() const;
			bool DoesExist() const;

			bool IsBoradcastWindow() const;
			void AttachHandle(void* hwnd)
			{
				m_Handle = hwnd;
			}
			void* DetachHandle() noexcept
			{
				void* handle = m_Handle;
				m_Handle = nullptr;

				return handle;
			}

			template<class TFunc>
			requires(std::is_convertible_v<std::invoke_result_t<TFunc, FlagSet<intptr_t>>, FlagSet<intptr_t>>)
			std::optional<intptr_t> ModWindowStyle(int index, TFunc&& func)
			{
				return GetValue(index).and_then([&](intptr_t value)
				{
					FlagSet<intptr_t> flags = std::invoke(func, FlagSet(value));
					return SetValue(index, *flags);
				});
			}

			std::optional<intptr_t> ModWindowStyle(int index, FlagSet<intptr_t> style, bool enable)
			{
				return ModWindowStyle(index, [&](auto flags)
				{
					return flags.Mod(style, enable);
				});
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			auto operator<=>(const SystemWindow& other) const noexcept
			{
				return m_Handle <=> other.m_Handle;
			}
			bool operator==(const SystemWindow& other) const noexcept
			{
				return m_Handle == other.m_Handle;
			}
	};
}

namespace kxf
{
	class KXF_API OwningSystemWindow final: public SystemWindow
	{
		public:
			OwningSystemWindow(void* hwnd = nullptr)
				:SystemWindow(hwnd)
			{
			}
			OwningSystemWindow(const OwningSystemWindow&) = delete;
			OwningSystemWindow(OwningSystemWindow&& other) noexcept
			{
				*this = std::move(other);
			}
			~OwningSystemWindow()
			{
				Destroy();
			}

		public:
			OwningSystemWindow& operator=(const OwningSystemWindow&) = delete;
			OwningSystemWindow& operator=(OwningSystemWindow&& other) noexcept
			{
				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;

				return *this;
			}

			auto operator<=>(const OwningSystemWindow& other) const noexcept
			{
				return m_Handle <=> other.m_Handle;
			}
			bool operator==(const OwningSystemWindow& other) const noexcept
			{
				return m_Handle == other.m_Handle;
			}
	};
}

namespace std
{
	template<>
	struct hash<kxf::SystemWindow> final
	{
		size_t operator()(const kxf::SystemWindow& window) const noexcept
		{
			std::hash<void*> calc;
			return calc(window.GetHandle());
		}
	};

	template<>
	struct hash<kxf::OwningSystemWindow> final
	{
		size_t operator()(const kxf::OwningSystemWindow& window) const noexcept
		{
			std::hash<void*> calc;
			return calc(window.GetHandle());
		}
	};
}
