#pragma once
#include "WidgetEvent.h"
#include "kxf/Core/UniChar.h"
class wxKeyEvent;

namespace kxf
{
	class KXF_API WidgetKeyEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(WidgetKeyEvent, KeyUp);
			kxf_EVENT_MEMBER(WidgetKeyEvent, KeyDown);

			kxf_EVENT_MEMBER(WidgetKeyEvent, Char);
			kxf_EVENT_MEMBER(WidgetKeyEvent, CharHook);

		private:
			KeyboardState m_KeyboardState;
			Point m_Position = Point::UnspecifiedPosition();
			KeyCode m_KeyCode = KeyCode::None;
			UniChar m_UnicodeKey;
			uint32_t m_NativeKeyCode = 0;
			FlagSet<uint32_t> m_NativeKeyFlags;
			bool m_IsNextEventAllowed = false;
			bool m_IsAutoRepeat = false;

		public:
			WidgetKeyEvent() noexcept = default;
			WidgetKeyEvent(IWidget& widget, KeyCode keyCode) noexcept;
			WidgetKeyEvent(IWidget& widget, uint32_t nativeKeyCode, FlagSet<uint32_t> nativeKeyFlags) noexcept;
			WidgetKeyEvent(IWidget& widget, const wxKeyEvent& keyEventWX) noexcept;

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetKeyEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const noexcept override
			{
				return EventCategory::UserInput;
			}

			// WidgetKeyEvent
			KeyboardState GetKeyboardState() const noexcept
			{
				return m_KeyboardState;
			}
			KeyCode GetKeyCode() const noexcept
			{
				return m_KeyCode;
			}
			UniChar GetUnicodeKey() const noexcept
			{
				return m_UnicodeKey;
			}
			Point GetPosition() const noexcept
			{
				return m_Position;
			}
			bool IsAutoRepeat() const noexcept
			{
				return m_IsAutoRepeat;
			}
			bool IsKeyInCategory(FlagSet<KeyCategory> category) const noexcept;

			bool IsNextEventAllowed() const noexcept
			{
				return m_IsNextEventAllowed;
			}
			void AllowNextEvent(bool allow = true) noexcept
			{
				m_IsNextEventAllowed = allow;
			}

			uint32_t GetNativeKeyCode() const noexcept
			{
				return m_NativeKeyCode;
			}
			FlagSet<uint32_t> GetNativeKeyFlags() const noexcept
			{
				return m_NativeKeyFlags;
			}
	};
}
