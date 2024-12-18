#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class KXF_API BehaviorEvent: public SciterEvent
	{
		public:
			// Button
			kxf_EVENT_MEMBER(BehaviorEvent, ButtonClick);
			kxf_EVENT_MEMBER(BehaviorEvent, ButtonPress);
			kxf_EVENT_MEMBER(BehaviorEvent, ButtonStateChanged);

			// Edit
			kxf_EVENT_MEMBER(BehaviorEvent, EditValueChanging);
			kxf_EVENT_MEMBER(BehaviorEvent, EditValueChanged);

			// Select
			kxf_EVENT_MEMBER(BehaviorEvent, SelectValueChanged);
			kxf_EVENT_MEMBER(BehaviorEvent, SelectStateChanged);

			// Popup
			kxf_EVENT_MEMBER(BehaviorEvent, PopupReady);
			kxf_EVENT_MEMBER(BehaviorEvent, PopupDismissing);
			kxf_EVENT_MEMBER(BehaviorEvent, PopupDismissed);
			kxf_EVENT_MEMBER(BehaviorEvent, PopupShowRequest);
			kxf_EVENT_MEMBER(BehaviorEvent, PopupCloseRequest);

			// Menu
			kxf_EVENT_MEMBER(BehaviorEvent, ContextMenuRequest);
			kxf_EVENT_MEMBER(BehaviorEvent, MenuItemActive);
			kxf_EVENT_MEMBER(BehaviorEvent, MenuItemClick);
			
			// History
			kxf_EVENT_MEMBER(BehaviorEvent, HistoryPush);
			kxf_EVENT_MEMBER(BehaviorEvent, HistoryDrop);
			kxf_EVENT_MEMBER(BehaviorEvent, HistoryBackward);
			kxf_EVENT_MEMBER(BehaviorEvent, HistoryForward);
			kxf_EVENT_MEMBER(BehaviorEvent, HistoryStateChanged);

			// Document
			kxf_EVENT_MEMBER(BehaviorEvent, DocumentComplete);
			kxf_EVENT_MEMBER(BehaviorEvent, DocumentCreated);
			kxf_EVENT_MEMBER(BehaviorEvent, DocumentParsed);
			kxf_EVENT_MEMBER(BehaviorEvent, DocumentReady);
			kxf_EVENT_MEMBER(BehaviorEvent, DocumentClosing);
			kxf_EVENT_MEMBER(BehaviorEvent, DocumentClosed);

			// Video
			kxf_EVENT_MEMBER(BehaviorEvent, VideoInitialized);
			kxf_EVENT_MEMBER(BehaviorEvent, VideoStarted);
			kxf_EVENT_MEMBER(BehaviorEvent, VideoStopped);
			kxf_EVENT_MEMBER(BehaviorEvent, VideoBindingRequest);

			// Pagination
			kxf_EVENT_MEMBER(BehaviorEvent, PaginationStart);
			kxf_EVENT_MEMBER(BehaviorEvent, PaginationPage);
			kxf_EVENT_MEMBER(BehaviorEvent, PaginationEnd);

			// Generic
			kxf_EVENT_MEMBER(BehaviorEvent, GenericClick);
			kxf_EVENT_MEMBER(BehaviorEvent, GenericChange);
			kxf_EVENT_MEMBER(BehaviorEvent, HyperlinkClick);

			// Expand/Collapse
			kxf_EVENT_MEMBER(BehaviorEvent, ElementExpanded);
			kxf_EVENT_MEMBER(BehaviorEvent, ElementCollapsed);

			// Forms
			kxf_EVENT_MEMBER(BehaviorEvent, FormSubmit);
			kxf_EVENT_MEMBER(BehaviorEvent, FormReset);

			// Misc
			kxf_EVENT_MEMBER(BehaviorEvent, Animation);
			kxf_EVENT_MEMBER(BehaviorEvent, ActivateChild);

			kxf_EVENT_MEMBER(BehaviorEvent, VisualStatusChanged);
			kxf_EVENT_MEMBER(BehaviorEvent, DisabledStatusChanged);
			kxf_EVENT_MEMBER(BehaviorEvent, ContentChanged);
			kxf_EVENT_MEMBER(BehaviorEvent, UIStateChanged);

		protected:
			String m_EventName;
			ElementUID* m_SourceElement = nullptr;

		public:
			BehaviorEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<BehaviorEvent>(std::move(*this));
			}
			
			// SciterEvent
			String GetEventName() const
			{
				return m_EventName;
			}
			void SetEventName(const String& name)
			{
				m_EventName = name;
			}

			Element GetSourceElement() const
			{
				return RetrieveElement(m_SourceElement);
			}
			void SetSourceElement(const Element& element)
			{
				AcceptElement(m_SourceElement, element);
			}
	};
}
