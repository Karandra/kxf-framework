#include "kxf-pch.h"
#include "EventDispatcher.h"
#include "SciterAPI.h"
#include "Widget.h"
#include "WidgetFactory.h"
#include "Host.h"
#include "ScriptValue.h"
#include "kxf/Sciter/Private/Conversion.h"
#include "kxf/Utility/Drawing.h"

namespace kxf::Sciter
{
	template<class TEvent>
	TEvent MakeEvent(BasicEventDispatcher& evtHandler)
	{
		TEvent event(evtHandler.GetHost());
		event.SetEventSource(evtHandler.GetEvtHandler().QueryInterface<IEvtHandler>());
		event.Allow();

		return event;
	}

	void AssignKeyboardState(wxKeyboardState& state, UINT sciterValues)
	{
		state.SetAltDown(sciterValues & KEYBOARD_STATES::ALT_KEY_PRESSED);
		state.SetShiftDown(sciterValues & KEYBOARD_STATES::SHIFT_KEY_PRESSED);
		state.SetControlDown(sciterValues & KEYBOARD_STATES::CONTROL_KEY_PRESSED);
		state.SetMetaDown(wxGetKeyState(wxKeyCode::WXK_WINDOWS_LEFT) || wxGetKeyState(wxKeyCode::WXK_WINDOWS_RIGHT));
	}
	void AssignMouseState(wxMouseState& state, UINT sciterValues)
	{
		state.SetLeftDown(sciterValues & MOUSE_BUTTONS::MAIN_MOUSE_BUTTON);
		state.SetRightDown(sciterValues & MOUSE_BUTTONS::PROP_MOUSE_BUTTON);
		state.SetMiddleDown(sciterValues & MOUSE_BUTTONS::MIDDLE_MOUSE_BUTTON);
		state.SetAux1Down(::GetKeyState(VK_XBUTTON1) < 0);
		state.SetAux2Down(::GetKeyState(VK_XBUTTON2) < 0);
	}
	wxStockCursor MapCursorType(CURSOR_TYPE type)
	{
		switch (type)
		{
			case CURSOR_TYPE::CURSOR_ARROW:
			case CURSOR_TYPE::CURSOR_UPARROW:
			case CURSOR_TYPE::CURSOR_DRAG_MOVE:
			case CURSOR_TYPE::CURSOR_DRAG_COPY:
			{
				return wxStockCursor::wxCURSOR_ARROW;
			}
			case CURSOR_TYPE::CURSOR_IBEAM:
			{
				return wxStockCursor::wxCURSOR_IBEAM;
			}
			case CURSOR_TYPE::CURSOR_WAIT:
			{
				return wxStockCursor::wxCURSOR_WAIT;
			}
			case CURSOR_TYPE::CURSOR_CROSS:
			{
				return wxStockCursor::wxCURSOR_CROSS;
			}
			case CURSOR_TYPE::CURSOR_SIZENWSE:
			{
				return wxStockCursor::wxCURSOR_SIZENWSE;
			}
			case CURSOR_TYPE::CURSOR_SIZENESW:
			{
				return wxStockCursor::wxCURSOR_SIZENESW;
			}
			case CURSOR_TYPE::CURSOR_SIZEWE:
			{
				return wxStockCursor::wxCURSOR_SIZEWE;
			}
			case CURSOR_TYPE::CURSOR_SIZENS:
			{
				return wxStockCursor::wxCURSOR_SIZENS;
			}
			case CURSOR_TYPE::CURSOR_SIZEALL:
			{
				return wxStockCursor::wxCURSOR_SIZING;
			}
			case CURSOR_TYPE::CURSOR_NO:
			{
				return wxStockCursor::wxCURSOR_NO_ENTRY;
			}
			case CURSOR_TYPE::CURSOR_APPSTARTING:
			{
				return wxStockCursor::wxCURSOR_ARROWWAIT;
			}
			case CURSOR_TYPE::CURSOR_HELP:
			{
				return wxStockCursor::wxCURSOR_QUESTION_ARROW;
			}
			case CURSOR_TYPE::CURSOR_HAND:
			{
				return wxStockCursor::wxCURSOR_HAND;
			}
		}
		return wxStockCursor::wxCURSOR_NONE;
	}
	ScrollSource MapScrollSource(SCROLL_SOURCE source)
	{
		switch (source)
		{
			case SCROLL_SOURCE::SCROLL_SOURCE_KEYBOARD:
			{
				return ScrollSource::Keyboard;
			}
			case SCROLL_SOURCE::SCROLL_SOURCE_SCROLLBAR:
			{
				return ScrollSource::Scrollbar;
			}
			case SCROLL_SOURCE::SCROLL_SOURCE_ANIMATOR:
			{
				return ScrollSource::Animator;
			}
			case SCROLL_SOURCE::SCROLL_SOURCE_WHEEL:
			{
				return ScrollSource::Wheel;
			}
		};
		return ScrollSource::Unknown;
	}
}

namespace kxf::Sciter
{
	BOOL BasicEventDispatcher::CallHandleEvent(void* context, ElementHandle* element, uint32_t eventGroupID, void* parameters)
	{
		if (context && element)
		{
			return reinterpret_cast<BasicEventDispatcher*>(context)->SciterHandleEvent(element, eventGroupID, parameters);
		}
		return FALSE;
	}
	UINT BasicEventDispatcher::CallHandleNotification(void* notification, void* context)
	{
		if (notification && context)
		{
			return reinterpret_cast<BasicEventDispatcher*>(context)->SciterHandleNotification(notification);
		}
		return 0;
	}

	int BasicEventDispatcher::HandleLoadDataNotification(void* context)
	{
		SCN_LOAD_DATA& notification = *reinterpret_cast<SCN_LOAD_DATA*>(context);

		aux::wchars uri = aux::chars_of(notification.uri);
		if (uri.like(L"res:*"))
		{
			LPCBYTE data = nullptr;
			UINT size = 0;
			if (sciter::load_resource_data(nullptr, uri.start + 4, data, size))
			{
				GetSciterAPI()->SciterDataReady(GetSciterHandle(), notification.uri, data, size);
			}
			return LOAD_DISCARD;
		}
		return LOAD_OK;
	}
	int BasicEventDispatcher::HandleDataLoadedNotification(void* context)
	{
		SCN_DATA_LOADED& notification = *reinterpret_cast<SCN_DATA_LOADED*>(context);
		return 0;
	}
	int BasicEventDispatcher::HandlePostedNotification(void* context)
	{
		SCN_POSTED_NOTIFICATION& notification = *reinterpret_cast<SCN_POSTED_NOTIFICATION*>(context);
		return 0;
	}
	bool BasicEventDispatcher::HandleAttachBehaviorNotification(void* context)
	{
		if (IsHostLevelDispatcher())
		{
			SCN_ATTACH_BEHAVIOR& notification = *reinterpret_cast<SCN_ATTACH_BEHAVIOR*>(context);

			if (auto widget = WidgetFactory::NewWidget(m_Host, FromSciterElement(notification.element), notification.behaviorName))
			{
				BasicEventDispatcher& dispatcher = widget.release()->m_EventDispatcher;

				notification.elementTag = &dispatcher;
				notification.elementProc = reinterpret_cast<LPELEMENT_EVENT_PROC>(CallHandleEvent);
				return true;
			}
			return sciter::create_behavior(&notification);
		}
		return false;
	}
	void BasicEventDispatcher::HandleCriticalFailureNotification()
	{
	}
	void BasicEventDispatcher::HandleDestroyedNotification()
	{
	}

	bool BasicEventDispatcher::HandleInitializationEvent(ElementHandle* element, void* context)
	{
		INITIALIZATION_PARAMS& parameters = *reinterpret_cast<INITIALIZATION_PARAMS*>(context);

		if (parameters.cmd == INITIALIZATION_EVENTS::BEHAVIOR_ATTACH)
		{
			Attached();

			SciterEvent event = MakeEvent<SciterEvent>(*this);
			event.SetElement(element);
			return ProcessEvent(event, EvtAttached);
		}
		else if (parameters.cmd == INITIALIZATION_EVENTS::BEHAVIOR_DETACH)
		{
			Detached();

			SciterEvent event = MakeEvent<SciterEvent>(*this);
			event.SetElement(element);
			return ProcessEvent(event, EvtDetached);
		}
		return true;
	}
	bool BasicEventDispatcher::HandleKeyEvent(ElementHandle* element, void* context)
	{
		KEY_PARAMS& parameters = *reinterpret_cast<KEY_PARAMS*>(context);

		EventID eventID;
		KeyEvent event = MakeEvent<KeyEvent>(*this);
		switch (parameters.cmd)
		{
			case KEY_EVENTS::KEY_CHAR:
			{
				eventID = EvtKeyChar;
				event.SetUnicodeKey(parameters.key_code);
				break;
			}
			case KEY_EVENTS::KEY_UP:
			{
				eventID = EvtKeyUp;
				break;
			}
			case KEY_EVENTS::KEY_DOWN:
			{
				eventID = EvtKeyDown;
				break;
			}
		};

		if (eventID)
		{
			event.SetElement(element);
			event.SetTargetElement(FromSciterElement(parameters.target));
			event.SetKeyCode(static_cast<wxKeyCode>(parameters.key_code));
			event.SetPosition(Point(m_Host.GetWindow().ScreenToClient(wxGetMousePosition())));
			AssignKeyboardState(event, parameters.alt_state);

			return ProcessEvent(event, eventID);
		}
		return false;
	}
	bool BasicEventDispatcher::HandleMouseEvent(ElementHandle* element, void* context)
	{
		MOUSE_PARAMS& parameters = *reinterpret_cast<MOUSE_PARAMS*>(context);

		EventID eventID;
		switch (parameters.cmd)
		{
			case MOUSE_EVENTS::MOUSE_ENTER:
			{
				eventID = EvtMouseEnter;
				break;
			}
			case MOUSE_EVENTS::MOUSE_LEAVE:
			{
				eventID = EvtMouseLeave;
				break;
			}
			case MOUSE_EVENTS::MOUSE_MOVE:
			{
				eventID = EvtMouseMove;
				break;
			}
			case MOUSE_EVENTS::MOUSE_UP:
			{
				eventID = EvtMouseUp;
				break;
			}
			case MOUSE_EVENTS::MOUSE_DOWN:
			{
				eventID = EvtMouseDown;
				break;
			}
			case MOUSE_EVENTS::MOUSE_CLICK:
			{
				eventID = EvtMouseClick;
				break;
			}
			case MOUSE_EVENTS::MOUSE_DCLICK:
			{
				eventID = EvtMouseDoubleClick;
				break;
			}
			case MOUSE_EVENTS::MOUSE_TICK:
			{
				eventID = EvtMouseTick;
				break;
			}
			case MOUSE_EVENTS::MOUSE_IDLE:
			{
				eventID = EvtMouseIdle;
				break;
			}

			// Skipping drag events for now. Sciter also has "exchange" event and it seems also have something to do with drag and drop.
			// Further investigation required.
		};

		if (eventID)
		{
			MouseEvent event = MakeEvent<MouseEvent>(*this);
			event.SetElement(element);
			event.SetTargetElement(FromSciterElement(parameters.target));
			event.SetPosition({parameters.pos_view.x, parameters.pos_view.y});
			event.SetRelativePosition({parameters.pos.x, parameters.pos.y});
			event.SetIsOnIcon(parameters.is_on_icon);
			event.SetCursorType(MapCursorType(static_cast<CURSOR_TYPE>(parameters.cursor_type)));
			AssignKeyboardState(event, parameters.alt_state);
			AssignMouseState(event, parameters.button_state);

			return ProcessEvent(event, eventID);
		}
		return false;
	}
	bool BasicEventDispatcher::HandleFocusEvent(ElementHandle* element, void* context)
	{
		FOCUS_PARAMS& parameters = *reinterpret_cast<FOCUS_PARAMS*>(context);

		EventID eventID;
		switch (parameters.cmd)
		{
			case FOCUS_EVENTS::FOCUS_GOT:
			{
				eventID = EvtSetFocus;
				break;
			}
			case FOCUS_EVENTS::FOCUS_LOST:
			{
				eventID = EvtKillFocus;
				break;
			}
			case FOCUS_EVENTS::FOCUS_IN:
			{
				eventID = EvtContainerSetFocus;
				break;
			}
			case FOCUS_EVENTS::FOCUS_OUT:
			{
				eventID = EvtContainerKillFocus;
				break;
			}
			case FOCUS_EVENTS::FOCUS_REQUEST:
			{
				eventID = EvtRequestFocus;
				break;
			}
			case FOCUS_EVENTS::FOCUS_ADVANCE_REQUEST:
			{
				eventID = EvtRequestFocusAdvance;
				break;
			}
		};

		if (eventID)
		{
			FocusEvent event = MakeEvent<FocusEvent>(*this);
			event.SetElement(element);
			event.SetTargetElement(FromSciterElement(parameters.target));

			const bool result = ProcessEvent(event, eventID);
			parameters.cancel = !event.IsAllowed();
			return result;
		}
		return false;
	}
	bool BasicEventDispatcher::HandleSizeEvent(ElementHandle* element, void* context)
	{
		SciterEvent event = MakeEvent<SizeEvent>(*this);
		return ProcessEvent(event, EvtSize);
	}
	bool BasicEventDispatcher::HandleTimerEvent(ElementHandle* element, void* context)
	{
		TIMER_PARAMS& parameters = *reinterpret_cast<TIMER_PARAMS*>(context);

		TimerEvent event = MakeEvent<TimerEvent>(*this);
		event.SetTimerID(parameters.timerId);
		return ProcessEvent(event, EvtTimer);
	}
	bool BasicEventDispatcher::HandleScrollEvent(ElementHandle* element, void* context)
	{
		SCROLL_PARAMS& parameters = *reinterpret_cast<SCROLL_PARAMS*>(context);

		EventID eventID;
		switch (parameters.cmd)
		{
			case SCROLL_EVENTS::SCROLL_HOME:
			{
				eventID = EvtScrollHome;
				break;
			}
			case SCROLL_EVENTS::SCROLL_END:
			{
				eventID = EvtScrollEnd;
				break;
			}
			case SCROLL_EVENTS::SCROLL_STEP_PLUS:
			{
				eventID = EvtScrollStepPlus;
				break;
			}
			case SCROLL_EVENTS::SCROLL_STEP_MINUS:
			{
				eventID = EvtScrollStepMinus;
				break;
			}
			case SCROLL_EVENTS::SCROLL_PAGE_PLUS:
			{
				eventID = EvtScrollPagePlus;
				break;
			}
			case SCROLL_EVENTS::SCROLL_PAGE_MINUS:
			{
				eventID = EvtScrollPageMinus;
				break;
			}
			case SCROLL_EVENTS::SCROLL_SLIDER_PRESSED:
			{
				eventID = EvtScrollSliderPressed;
				break;
			}
			case SCROLL_EVENTS::SCROLL_SLIDER_RELEASED:
			{
				eventID = EvtScrollSliderReleased;
				break;
			}
			case SCROLL_EVENTS::SCROLL_CORNER_PRESSED:
			{
				eventID = EvtScrollCornerPressed;
				break;
			}
			case SCROLL_EVENTS::SCROLL_CORNER_RELEASED:
			{
				eventID = EvtScrollCornerReleased;
				break;
			}
		};

		if (eventID)
		{
			ScrollEvent event = MakeEvent<ScrollEvent>(*this);
			event.SetElement(element);
			event.SetTargetElement(FromSciterElement(parameters.target));
			event.SetOrientation(parameters.vertical ? Orientation::Vertical : Orientation::Horizontal);
			event.SetPosition(parameters.pos);
			event.SetSource(MapScrollSource(static_cast<SCROLL_SOURCE>(parameters.source)));

			return ProcessEvent(event, eventID);
		}
		return false;
	}
	bool BasicEventDispatcher::HandleDrawEvent(ElementHandle* element, void* context)
	{
		DRAW_PARAMS& parameters = *reinterpret_cast<DRAW_PARAMS*>(context);

		EventID eventID;
		switch (parameters.cmd)
		{
			case DRAW_EVENTS::DRAW_BACKGROUND:
			{
				eventID = EvtPaintBackground;
				break;
			}
			case DRAW_EVENTS::DRAW_FOREGROUND:
			{
				eventID = EvtPaintForeground;
				break;
			}
			case DRAW_EVENTS::DRAW_OUTLINE:
			{
				eventID = EvtPaintOutline;
				break;
			}
			case DRAW_EVENTS::DRAW_CONTENT:
			{
				eventID = EvtPaintContent;
				break;
			}
		};

		if (eventID)
		{
			PaintEvent event = MakeEvent<PaintEvent>(*this);
			event.SetElement(element);
			event.SetGraphicsContext(FromSciterGraphicsContext(parameters.gfx));
			event.SetRect(Utility::FromWindowsRect(parameters.area));

			return ProcessEvent(event, eventID);
		}
		return false;
	}
	bool BasicEventDispatcher::HandleBehaviorEvent(ElementHandle* element, void* context)
	{
		BEHAVIOR_EVENT_PARAMS& parameters = *reinterpret_cast<BEHAVIOR_EVENT_PARAMS*>(context);

		EventID eventID;
		switch (parameters.cmd)
		{
			case BEHAVIOR_EVENTS::BUTTON_CLICK:
			{
				eventID = BehaviorEvent::EvtButtonClick;
				break;
			}
			case BEHAVIOR_EVENTS::BUTTON_PRESS:
			{
				eventID = BehaviorEvent::EvtButtonPress;
				break;
			}
			case BEHAVIOR_EVENTS::BUTTON_STATE_CHANGED:
			{
				eventID = BehaviorEvent::EvtButtonStateChanged;
				break;
			}

			// Edit
			case BEHAVIOR_EVENTS::EDIT_VALUE_CHANGING:
			{
				eventID = BehaviorEvent::EvtEditValueChanging;
				break;
			}
			case BEHAVIOR_EVENTS::EDIT_VALUE_CHANGED:
			{
				eventID = BehaviorEvent::EvtEditValueChanged;
				break;
			}

			// Select
			case BEHAVIOR_EVENTS::SELECT_SELECTION_CHANGED:
			{
				eventID = BehaviorEvent::EvtSelectValueChanged;
				break;
			}
			case BEHAVIOR_EVENTS::SELECT_STATE_CHANGED:
			{
				eventID = BehaviorEvent::EvtSelectStateChanged;
				break;
			}

			// Popup
			case BEHAVIOR_EVENTS::POPUP_READY:
			{
				eventID = BehaviorEvent::EvtPopupReady;
				break;
			}
			case BEHAVIOR_EVENTS::POPUP_DISMISSING:
			{
				eventID = BehaviorEvent::EvtPopupDismissing;
				break;
			}
			case BEHAVIOR_EVENTS::POPUP_DISMISSED:
			{
				eventID = BehaviorEvent::EvtPopupDismissed;
				break;
			}
			case BEHAVIOR_EVENTS::POPUP_REQUEST:
			{
				eventID = BehaviorEvent::EvtPopupShowRequest;
				break;
			}
			case BEHAVIOR_EVENTS::CLOSE_POPUP:
			{
				eventID = BehaviorEvent::EvtPopupCloseRequest;
				break;
			}

			// Menu
			case BEHAVIOR_EVENTS::CONTEXT_MENU_REQUEST:
			{
				eventID = BehaviorEvent::EvtContextMenuRequest;
				break;
			}
			case BEHAVIOR_EVENTS::MENU_ITEM_ACTIVE:
			{
				eventID = BehaviorEvent::EvtMenuItemActive;
				break;
			}
			case BEHAVIOR_EVENTS::MENU_ITEM_CLICK:
			{
				eventID = BehaviorEvent::EvtMenuItemClick;
				break;
			}

			// History
			case BEHAVIOR_EVENTS::HISTORY_PUSH:
			{
				eventID = BehaviorEvent::EvtHistoryPush;
				break;
			}
			case BEHAVIOR_EVENTS::HISTORY_DROP:
			{
				eventID = BehaviorEvent::EvtHistoryDrop;
				break;
			}
			case BEHAVIOR_EVENTS::HISTORY_PRIOR:
			{
				eventID = BehaviorEvent::EvtHistoryBackward;
				break;
			}
			case BEHAVIOR_EVENTS::HISTORY_NEXT:
			{
				eventID = BehaviorEvent::EvtHistoryForward;
				break;
			}
			case BEHAVIOR_EVENTS::HISTORY_STATE_CHANGED:
			{
				eventID = BehaviorEvent::EvtHistoryStateChanged;
				break;
			}

			// Document
			case BEHAVIOR_EVENTS::DOCUMENT_COMPLETE:
			{
				eventID = BehaviorEvent::EvtDocumentComplete;
				break;
			}
			case BEHAVIOR_EVENTS::DOCUMENT_CREATED:
			{
				eventID = BehaviorEvent::EvtDocumentCreated;
				break;
			}
			case BEHAVIOR_EVENTS::DOCUMENT_PARSED:
			{
				eventID = BehaviorEvent::EvtDocumentParsed;
				break;
			}
			case BEHAVIOR_EVENTS::DOCUMENT_READY:
			{
				eventID = BehaviorEvent::EvtDocumentReady;
				break;
			}
			case BEHAVIOR_EVENTS::DOCUMENT_CLOSE_REQUEST:
			{
				eventID = BehaviorEvent::EvtDocumentClosing;
				break;
			}
			case BEHAVIOR_EVENTS::DOCUMENT_CLOSE:
			{
				eventID = BehaviorEvent::EvtDocumentClosed;
				break;
			}

			// Video
			case BEHAVIOR_EVENTS::VIDEO_INITIALIZED:
			{
				eventID = BehaviorEvent::EvtVideoInitialized;
				break;
			}
			case BEHAVIOR_EVENTS::VIDEO_STARTED:
			{
				eventID = BehaviorEvent::EvtVideoStarted;
				break;
			}
			case BEHAVIOR_EVENTS::VIDEO_STOPPED:
			{
				eventID = BehaviorEvent::EvtVideoStopped;
				break;
			}
			case BEHAVIOR_EVENTS::VIDEO_BIND_RQ:
			{
				eventID = BehaviorEvent::EvtVideoBindingRequest;
				break;
			}

			// Pagination
			case BEHAVIOR_EVENTS::PAGINATION_STARTS:
			{
				eventID = BehaviorEvent::EvtPaginationStart;
				break;
			}
			case BEHAVIOR_EVENTS::PAGINATION_PAGE:
			{
				eventID = BehaviorEvent::EvtPaginationPage;
				break;
			}
			case BEHAVIOR_EVENTS::PAGINATION_ENDS:
			{
				eventID = BehaviorEvent::EvtPaginationEnd;
				break;
			}

			// Generic
			case BEHAVIOR_EVENTS::CLICK:
			{
				eventID = BehaviorEvent::EvtGenericClick;
				break;
			}
			case BEHAVIOR_EVENTS::CHANGE:
			{
				eventID = BehaviorEvent::EvtGenericChange;
				break;
			}
			case BEHAVIOR_EVENTS::HYPERLINK_CLICK:
			{
				eventID = BehaviorEvent::EvtHyperlinkClick;
				break;
			}

			// Expand/Collapse
			case BEHAVIOR_EVENTS::ELEMENT_EXPANDED:
			{
				eventID = BehaviorEvent::EvtElementExpanded;
				break;
			}
			case BEHAVIOR_EVENTS::ELEMENT_COLLAPSED:
			{
				eventID = BehaviorEvent::EvtElementCollapsed;
				break;
			}

			// Forms
			case BEHAVIOR_EVENTS::FORM_SUBMIT:
			{
				eventID = BehaviorEvent::EvtFormSubmit;
				break;
			}
			case BEHAVIOR_EVENTS::FORM_RESET:
			{
				eventID = BehaviorEvent::EvtFormReset;
				break;
			}

			// Misc
			case BEHAVIOR_EVENTS::ANIMATION:
			{
				eventID = BehaviorEvent::EvtAnimation;
				break;
			}
			case BEHAVIOR_EVENTS::ACTIVATE_CHILD:
			{
				eventID = BehaviorEvent::EvtActivateChild;
				break;
			}

			case BEHAVIOR_EVENTS::VISUAL_STATUS_CHANGED:
			{
				eventID = BehaviorEvent::EvtVisualStatusChanged;
				break;
			}
			case BEHAVIOR_EVENTS::DISABLED_STATUS_CHANGED:
			{
				eventID = BehaviorEvent::EvtDisabledStatusChanged;
				break;
			}
			case BEHAVIOR_EVENTS::CONTENT_CHANGED:
			{
				eventID = BehaviorEvent::EvtContentChanged;
				break;
			}
			case BEHAVIOR_EVENTS::UI_STATE_CHANGED:
			{
				eventID = BehaviorEvent::EvtUIStateChanged;
				break;
			}
		};

		if (eventID)
		{
			BehaviorEvent event = MakeEvent<BehaviorEvent>(*this);
			event.SetElement(element);
			event.SetSourceElement(FromSciterElement(parameters.he));
			event.SetTargetElement(FromSciterElement(parameters.heTarget));
			event.SetEventName(parameters.name);

			// Notify host about document change
			if (parameters.cmd == BEHAVIOR_EVENTS::DOCUMENT_COMPLETE)
			{
				m_Host.OnDocumentChanged();
			}

			const bool processed = ProcessEvent(event, eventID);
			if (!event.IsAllowed())
			{
				// This event can be canceled this way
				if (parameters.cmd == DOCUMENT_CLOSE_REQUEST)
				{
					ScriptValue value;
					value.SetString("cancel", ScriptValue::StringType::Symbol);
					GetSciterAPI()->ValueCopy(&parameters.data, ToSciterScriptValue(value.GetNativeValue()));
				}
			}
			return processed;
		}
		return false;
	}

	WXHWND BasicEventDispatcher::GetSciterHandle() const
	{
		return m_Host.GetWindow().GetHandle();
	}
	bool BasicEventDispatcher::IsHostLevelDispatcher() const
	{
		return &m_Host.m_EventDispatcher == this;
	}
	bool BasicEventDispatcher::ProcessEvent(IEvent& event, const EventID& eventID)
	{
		return GetEvtHandler().ProcessEvent(event, eventID) && !event.IsSkipped();
	}
	void BasicEventDispatcher::QueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID)
	{
		GetEvtHandler().QueueEvent(std::move(event), eventID);
	}

	void BasicEventDispatcher::AttachHost()
	{
		GetSciterAPI()->SciterWindowAttachEventHandler(GetSciterHandle(), reinterpret_cast<LPELEMENT_EVENT_PROC>(CallHandleEvent), this, HANDLE_ALL);
		GetSciterAPI()->SciterSetCallback(GetSciterHandle(), reinterpret_cast<LPSciterHostCallback>(CallHandleNotification), this);
	}
	void BasicEventDispatcher::DetachHost()
	{
		GetSciterAPI()->SciterWindowDetachEventHandler(GetSciterHandle(), reinterpret_cast<LPELEMENT_EVENT_PROC>(CallHandleEvent), this);
		GetSciterAPI()->SciterSetCallback(GetSciterHandle(), nullptr, nullptr);
	}
	void BasicEventDispatcher::AttachElement(Element& element)
	{
		GetSciterAPI()->SciterAttachEventHandler(ToSciterElement(element.GetHandle()), reinterpret_cast<LPELEMENT_EVENT_PROC>(CallHandleEvent), this);
	}
	void BasicEventDispatcher::DetachElement(Element& element)
	{
		GetSciterAPI()->SciterDetachEventHandler(ToSciterElement(element.GetHandle()), reinterpret_cast<LPELEMENT_EVENT_PROC>(CallHandleEvent), this);
	}

	bool BasicEventDispatcher::SciterHandleEvent(ElementHandle* element, uint32_t eventGroupID, void* context)
	{
		switch (eventGroupID)
		{
			case EVENT_GROUPS::SUBSCRIPTIONS_REQUEST:
			{
				// Defines list of event groups this event handler is subscribed to
				*reinterpret_cast<UINT*>(context) = HANDLE_ALL;
				return true;
			}
			case EVENT_GROUPS::HANDLE_INITIALIZATION:
			{
				return HandleInitializationEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_KEY:
			{
				return HandleKeyEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_MOUSE:
			{
				return HandleMouseEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_FOCUS:
			{
				return HandleFocusEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_SIZE:
			{
				return HandleSizeEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_TIMER:
			{
				return HandleTimerEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_SCROLL:
			{
				return HandleScrollEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_DRAW:
			{
				return HandleDrawEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_BEHAVIOR_EVENT:
			{
				return HandleBehaviorEvent(element, context);
			}
		};
		return false;
	}
	int BasicEventDispatcher::SciterHandleNotification(void* context)
	{
		SCITER_CALLBACK_NOTIFICATION& notification = *reinterpret_cast<SCITER_CALLBACK_NOTIFICATION*>(context);

		switch (notification.code)
		{
			case SC_LOAD_DATA:
			{
				return HandleLoadDataNotification(context);
			}
			case SC_DATA_LOADED:
			{
				return HandleDataLoadedNotification(context);
			}
			case SC_ATTACH_BEHAVIOR:
			{
				return static_cast<int>(HandleAttachBehaviorNotification(context));
			}
			case SC_POSTED_NOTIFICATION:
			{
				return HandlePostedNotification(context);
			}
			case SC_GRAPHICS_CRITICAL_FAILURE:
			{
				HandleCriticalFailureNotification();
				break;
			}
			case SC_ENGINE_DESTROYED:
			{
				HandleDestroyedNotification();
				break;
			}
		};
		return 0;
	}
}

namespace kxf::Sciter
{
	void WidgetEventDispatcher::Attached()
	{
		m_Widget.OnAttached();
	}
	void WidgetEventDispatcher::Detached()
	{
		m_Widget.OnDetached();
	}

	IEvtHandler& WidgetEventDispatcher::GetEvtHandler()
	{
		return m_Widget.GetEventHandler();
	}
}
