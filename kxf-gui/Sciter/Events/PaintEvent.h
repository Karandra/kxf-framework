#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class GraphicsContext;
	struct GraphicsContextHandle;
}

namespace kxf::Sciter
{
	class KXF_API PaintEvent: public SciterEvent
	{
		public:
			kxf_EVENT_MEMBER(PaintEvent, PaintBackground);
			kxf_EVENT_MEMBER(PaintEvent, PaintForeground);
			kxf_EVENT_MEMBER(PaintEvent, PaintOutline);
			kxf_EVENT_MEMBER(PaintEvent, PaintContent);

		protected:
			GraphicsContextHandle* m_GraphicsContext = nullptr;
			Rect m_Rect;

		public:
			PaintEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<PaintEvent>(std::move(*this));
			}

			// SciterEvent
			GraphicsContext GetGraphicsContext() const;
			void SetGraphicsContext(GraphicsContextHandle* handle);

			Rect GetRect() const
			{
				return m_Rect;
			}
			void SetRect(const Rect& rect)
			{
				m_Rect = rect;
			}
	};
}

namespace kxf::Sciter
{
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintBackground);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintForeground);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintOutline);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintContent);
}
