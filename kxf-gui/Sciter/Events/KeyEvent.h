#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class KXF_API KeyEvent: public SciterEvent, public wxKeyboardState
	{
		public:
			kxf_EVENT_MEMBER(KeyEvent, KeyChar);
			kxf_EVENT_MEMBER(KeyEvent, KeyUp);
			kxf_EVENT_MEMBER(KeyEvent, KeyDown);

		protected:
			Point m_Position = Point::UnspecifiedPosition();
			wxKeyCode m_KeyCode = wxKeyCode::WXK_NONE;
			UniChar m_UnicodeKey;

		public:
			KeyEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<KeyEvent>(std::move(*this));
			}

			// SciterEvent
			wxKeyCode GetKeyCode() const
			{
				return m_KeyCode;
			}
			void SetKeyCode(wxKeyCode keyCode)
			{
				m_KeyCode = keyCode;
			}

			UniChar GetUnicodeKey() const
			{
				return m_UnicodeKey;
			}
			void SetUnicodeKey(UniChar unicodeKey)
			{
				m_UnicodeKey = unicodeKey;
			}
	
			wxCoord GetX() const
			{
				return m_Position.GetX();
			}
			wxCoord GetY() const
			{
				return m_Position.GetY();
			}
			Point GetPosition() const
			{
				return m_Position;
			}
			void SetPosition(const Point& pos)
			{
				m_Position = pos;
			}
	};
}

namespace kxf::Sciter
{
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyChar);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyUp);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyDown);
}
