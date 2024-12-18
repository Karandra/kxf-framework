#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class KXF_API MouseEvent: public SciterEvent, public wxMouseState
	{
		public:
			kxf_EVENT_MEMBER(MouseEvent, MouseEnter);
			kxf_EVENT_MEMBER(MouseEvent, MouseLeave);
			kxf_EVENT_MEMBER(MouseEvent, MouseMove);
			kxf_EVENT_MEMBER(MouseEvent, MouseUp);
			kxf_EVENT_MEMBER(MouseEvent, MouseDown);

			kxf_EVENT_MEMBER(MouseEvent, MouseClick);
			kxf_EVENT_MEMBER(MouseEvent, MouseDoubleClick);
			kxf_EVENT_MEMBER(MouseEvent, MouseTick);
			kxf_EVENT_MEMBER(MouseEvent, MouseIdle);

		protected:
			Point m_RelativePosition = Point::UnspecifiedPosition();
			wxStockCursor m_CursorType = wxStockCursor::wxCURSOR_NONE;
			bool m_IsOnIcon = false;

		public:
			MouseEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<MouseEvent>(std::move(*this));
			}

			// SciterEvent
			Point GetRelativePosition() const
			{
				return m_RelativePosition;
			}
			void SetRelativePosition(const Point& pos)
			{
				m_RelativePosition = pos;
			}
	
			wxStockCursor GetCursorType() const
			{
				return m_CursorType;
			}
			void SetCursorType(wxStockCursor cursor)
			{
				m_CursorType = cursor;
			}

			bool IsOnIcon() const
			{
				return m_IsOnIcon;
			}
			void SetIsOnIcon(bool onIcon = true)
			{
				m_IsOnIcon = onIcon;
			}
	};
}

namespace kxf::Sciter
{
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseEnter);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseLeave);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseMove);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseUp);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseDown);

	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseClick);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseDoubleClick);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseTick);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseIdle);
}
