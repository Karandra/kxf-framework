#pragma once
#include "WidgetEvent.h"
#include "WidgetMouseEvent.h"
#include "../ITextWidget.h"
#include "kxf/Network/URI.h"

namespace kxf
{
	class KXF_API WidgetTextEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(WidgetTextEvent, LengthLimit);
			kxf_EVENT_MEMBER(WidgetTextEvent, Changed);
			kxf_EVENT_MEMBER(WidgetTextEvent, Commit);
			kxf_EVENT_MEMBER(WidgetTextEvent, URI);

			kxf_EVENT_MEMBER(WidgetTextEvent, Cut);
			kxf_EVENT_MEMBER(WidgetTextEvent, Copy);
			kxf_EVENT_MEMBER(WidgetTextEvent, Paste);

		private:
			std::shared_ptr<ITextEntry> m_TextEntry;
			WidgetMouseEvent m_MouseEvent;
			URI m_URI;
			String m_Text;
			size_t m_LengthLimit = String::npos;

		public:
			WidgetTextEvent() noexcept = default;

			WidgetTextEvent(IWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}
			WidgetTextEvent(IWidget& widget, size_t lengthLimit) noexcept
				:WidgetEvent(widget), m_LengthLimit(lengthLimit)
			{
			}
			WidgetTextEvent(IWidget& widget, String text) noexcept
				:WidgetEvent(widget), m_Text(std::move(text))
			{
			}
			WidgetTextEvent(IWidget& widget, URI uri, WidgetMouseEvent mouseEvent) noexcept
				:WidgetEvent(widget), m_MouseEvent(std::move(mouseEvent)), m_URI(std::move(uri))
			{
			}

			WidgetTextEvent(ITextWidget& widget) noexcept
				:WidgetTextEvent(static_cast<IWidget&>(widget))
			{
			}
			WidgetTextEvent(ITextWidget& widget, size_t lengthLimit) noexcept
				:WidgetTextEvent(static_cast<IWidget&>(widget), lengthLimit)
			{
			}
			WidgetTextEvent(ITextWidget& widget, String text) noexcept
				:WidgetTextEvent(static_cast<IWidget&>(widget), std::move(text))
			{
			}
			WidgetTextEvent(ITextWidget& widget, URI uri, WidgetMouseEvent mouseEvent) noexcept
				:WidgetTextEvent(static_cast<IWidget&>(widget), std::move(uri), std::move(mouseEvent))
			{
			}

			WidgetTextEvent(ITextEntry& textEntry) noexcept
				:m_TextEntry(textEntry.QueryInterface<ITextEntry>())
			{
			}
			WidgetTextEvent(ITextEntry& textEntry, size_t lengthLimit) noexcept
				:m_TextEntry(textEntry.QueryInterface<ITextEntry>()), m_LengthLimit(lengthLimit)
			{
			}
			WidgetTextEvent(ITextEntry& textEntry, String text) noexcept
				:m_TextEntry(textEntry.QueryInterface<ITextEntry>()), m_Text(std::move(text))
			{
			}
			WidgetTextEvent(ITextEntry& textEntry, URI uri, WidgetMouseEvent mouseEvent) noexcept
				:m_TextEntry(textEntry.QueryInterface<ITextEntry>()), m_MouseEvent(std::move(mouseEvent)), m_URI(std::move(uri))
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetTextEvent>(std::move(*this));
			}

		public:
			// WidgetTextEvent
			std::shared_ptr<ITextWidget> GetTextWidget() const noexcept
			{
				if (auto widget = GetWidget())
				{
					return widget->QueryInterface<ITextWidget>();
				}
				else if (m_TextEntry)
				{
					return m_TextEntry->QueryInterface<ITextWidget>();
				}
				return nullptr;
			}
			std::shared_ptr<ITextEntry> GetTextEntry() const
			{
				if (m_TextEntry)
				{
					return m_TextEntry;
				}
				else if (auto widget = GetWidget())
				{
					return widget->QueryInterface<ITextEntry>();
				}
				return nullptr;
			}

			const WidgetMouseEvent& GetMouseEvent() const noexcept
			{
				return m_MouseEvent;
			}
			WidgetMouseEvent& GetMouseEvent() noexcept
			{
				return m_MouseEvent;
			}

			URI GetURI() const
			{
				return m_URI;
			}
			String GetText() const
			{
				return m_Text;
			}
			size_t GetLengthLimit() const noexcept
			{
				return m_LengthLimit;
			}
	};
}
