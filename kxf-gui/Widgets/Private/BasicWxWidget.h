#pragma once
#include "Common.h"
#include "../IWidget.h"
#include "../INativeWidget.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/EvtHandlerAccessor.h"
#include "kxf/EventSystem/EventHandlerStack.h"
class wxWindow;
class wxWindowCreateEvent;
class wxWindowDestroyEvent;

namespace kxf::Private
{
	class KXF_API BasicWxWidgetBase
	{
		private:
			IWidget& m_Widget;
			std::shared_ptr<IObject> m_RefLock;
			wxWindow* m_Window = nullptr;
			bool m_ShouldDelete = false;

			float m_Transparency = 0.0f;
			bool m_VisibleFocusEnabled = true;

		private:
			void OnWindowCreate(wxWindowCreateEvent& event);
			void OnWindowDestroy(wxWindowDestroyEvent& event);

		public:
			BasicWxWidgetBase(IWidget& widget) noexcept
				:m_Widget(widget)
			{
			}
			~BasicWxWidgetBase()
			{
				Uninitialize();
			}

		public:
			void Initialize(std::unique_ptr<wxWindow> window);
			void Uninitialize();

		public:
			// Native interface
			void* GetHandle() const noexcept;
			wxWindow* GetWxWindow() const noexcept
			{
				return m_Window;
			}

			// Lifetime management
			bool IsWidgetAlive() const noexcept;
			bool CloseWidget(bool force);
			bool DestroyWidget();

			// HiDPI support
			float GetContentScaleFactor() const;
			void FromDIP(int& x, int& y) const;
			void ToDIP(int& x, int& y) const;

			// Positioning functions
			Point GetPosition() const;
			void SetPosition(const Point& pos);

			void Center(FlagSet<Orientation> orientation);
			void CenterOnParent(FlagSet<Orientation> orientation);

			// Size functions
			Rect GetRect(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const;
			Size GetSize(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const;
			void SetSize(const Size& size, FlagSet<WidgetSizeFlag> flags = WidgetSizeFlag::Widget);

			// Coordinate conversion functions
			void ScreenToClient(int& x, int& y) const;
			void ClientToScreen(int& x, int& y) const;

			void DialogUnitsToPixels(int& x, int& y) const;
			void PixelsToDialogUnits(int& x, int& y) const;

			// Focus
			bool IsFocusable() const;
			bool HasFocus() const;
			void SetFocus();

			bool IsFocusVisible() const;
			void SetFocusVisible(bool visible);

			// Layout
			LayoutDirection GetLayoutDirection() const;
			void SetLayoutDirection(LayoutDirection direction);

			void Fit();
			void FitInterior();
			bool Layout();

			// Child management functions
			void AddChildWidget(IWidget& widget);
			void RemoveChildWidget(const IWidget& widget);
			void DestroyChildWidgets();

			std::shared_ptr<IWidget> FindChildWidgetByID(WidgetID id) const;
			std::shared_ptr<IWidget> FindChildWidgetByName(const String& widgetName) const;
			Enumerator<std::shared_ptr<IWidget>> EnumChildWidgets() const;

			// Sibling and parent management functions
			std::shared_ptr<IWidget> GetParentWidget() const;
			void SetParentWidget(IWidget& widget);

			std::shared_ptr<IWidget> GetPrevSiblingWidget() const;
			std::shared_ptr<IWidget> GetNextSiblingWidget() const;

			// Drawing-related functions
			bool IsFrozen() const;
			void Freeze();
			void Thaw();

			Rect GetRefreshRect() const;
			void Refresh(const Rect& rect);

			Font GetFont() const;
			void SetFont(const Font& font);

			Color GetColor(WidgetColorFlag colorType) const;
			void SetColor(const Color& color, FlagSet<WidgetColorFlag> flags);

			float GetTransparency() const;
			bool SetTransparency(float value);

			// Widget state and visibility functions
			bool IsEnabled() const;
			bool IsIntrinsicallyEnabled() const;
			void SetEnabled(bool enabled);

			bool IsVisible() const;
			bool IsDisplayed() const;
			void SetVisible(bool visible);

			// Widget style functions
			FlagSet<WidgetStyle> GetWidgetStyle() const;
			void SetWidgetStyle(FlagSet<WidgetStyle> style);

			FlagSet<WidgetExStyle> GetWidgetExStyle() const;
			void SetWidgetExStyle(FlagSet<WidgetExStyle> style);

			WidgetBorder GetWidgetBorder() const;
			void SetWidgetBorder(WidgetBorder border);

			// Widget properties
			WidgetID GetWidgetID() const;
			void SetWidgetID(WidgetID id);

			String GetWidgetName() const;
			void SetWidgetName(const String& widgetName);

			String GetWidgetText(FlagSet<WidgetTextFlag> flags) const;
			void SetWidgetText(const String& widgetText, FlagSet<WidgetTextFlag> flags);

		public:
			// INativeWidget
			void* GetNativeHandle() const;
			String GetIntrinsicText() const;

			intptr_t GetWindowProperty(NativeWidgetProperty index) const;
			intptr_t SetWindowProperty(NativeWidgetProperty index, intptr_t value);

			bool PostMessage(uint32_t messageID, intptr_t wParam, intptr_t lParam);
			bool NotifyMessage(uint32_t messageID, intptr_t wParam, intptr_t lParam);
			bool SendMessageSignal(uint32_t messageID, intptr_t wParam, intptr_t lParam);
			intptr_t SendMessage(uint32_t messageID, intptr_t wParam, intptr_t lParam, TimeSpan timeout);

			HResult SetWindowTheme(const String& applicationName, const std::vector<String>& subIDs);
			bool EnableNonClientDPIScaling();

			bool IsForegroundWindow() const;
			bool SetForegroundWindow();
	};
}

namespace kxf::Private
{
	template<class TDerived, class TWindow, class TInterface, class TWindowImpl = BasicWxWidgetBase>
	class BasicWxWidget: public RTTI::PrivateStub<BasicWxWidget<TDerived, TWindow, TInterface, TWindowImpl>, TInterface, INativeWidget>
	{
		private:
			TWindowImpl m_Window;
			EvtHandler m_EvtHandler;
			EvtHandlerStack m_EventHandlerStack;

		private:
			TDerived& Self() noexcept
			{
				return static_cast<TDerived&>(*this);
			}
			const TDerived& Self() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

		private:
			auto AccessThisEvtHandler() noexcept
			{
				return EventSystem::EvtHandlerAccessor(GetThisEvtHandler());
			}
			auto AccessTopEvtHandler() noexcept
			{
				return EventSystem::EvtHandlerAccessor(GetTopEvtHandler());
			}

		protected:
			// BasicWxWidget
			IEvtHandler& GetThisEvtHandler() noexcept
			{
				return m_EvtHandler;
			}
			IEvtHandler& GetTopEvtHandler() noexcept
			{
				return *m_EventHandlerStack.GetTop();
			}

			template<class T = TWindow>
			TWindow* Get() noexcept
			{
				return static_cast<TWindow*>(m_Window.GetWxWindow());
			}

			template<class T = TWindow>
			const TWindow* Get() const noexcept
			{
				return static_cast<const TWindow*>(m_Window.GetWxWindow());
			}

			TWindowImpl& GetImpl() noexcept
			{
				return m_Window;
			}
			const TWindowImpl& GetImpl() const noexcept
			{
				return m_Window;
			}

			template<class T>
			void AssignWxWidget(std::unique_ptr<T> window)
			{
				m_Window.Initialize(std::move(window));
			}

			template<class... Args> requires(std::is_constructible_v<TWindow, TDerived&, Args...>)
			void InitializeWxWidget(Args&&... arg)
			{
				m_Window.Initialize(std::make_unique<TWindow>(Self(), std::forward<Args>(arg)...));
			}

			// IWidget
			void InheritVisualAttributes(const IWidget& parent) override
			{
				IWidget::InheritVisualAttributes(parent);
			}

		protected:
			// IEvtHandler
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<BindEventFlag> flags = {}) override
			{
				return AccessThisEvtHandler().DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return AccessThisEvtHandler().DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return AccessThisEvtHandler().DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventSystem::EventItem& eventItem) override
			{
				return AccessThisEvtHandler().OnDynamicBind(eventItem);
			}
			bool OnDynamicUnbind(EventSystem::EventItem& eventItem) override
			{
				return AccessThisEvtHandler().OnDynamicUnbind(eventItem);
			}

			std::unique_ptr<IEvent> DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}) override
			{
				return AccessTopEvtHandler().DoQueueEvent(std::move(event), eventID, uuid, flags);
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) override
			{
				return AccessTopEvtHandler().DoProcessEvent(event, eventID, uuid, flags, onlyIn);
			}

			bool TryBefore(IEvent& event) override
			{
				return AccessThisEvtHandler().TryBefore(event);
			}
			bool TryAfter(IEvent& event) override
			{
				return AccessThisEvtHandler().TryAfter(event);
			}

		public:
			BasicWxWidget()
				:m_EventHandlerStack(m_EvtHandler), m_Window(static_cast<IWidget&>(*this))
			{
			}
			BasicWxWidget(const BasicWxWidget&) = delete;
			~BasicWxWidget() = default;

		public:
			// IEvtHandler
			bool ProcessPendingEvents() override
			{
				return m_EvtHandler.ProcessPendingEvents();
			}
			size_t DiscardPendingEvents() override
			{
				return m_EvtHandler.DiscardPendingEvents();
			}

			IEvtHandler* GetPrevHandler() const override
			{
				return m_EvtHandler.GetPrevHandler();
			}
			IEvtHandler* GetNextHandler() const override
			{
				return m_EvtHandler.GetNextHandler();
			}
			void SetPrevHandler(IEvtHandler* evtHandler) override
			{
				// Can't chain widgets
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				// Can't chain widgets
			}

			void Unlink() override
			{
				m_EvtHandler.Unlink();
			}
			bool IsUnlinked() const override
			{
				return m_EvtHandler.IsUnlinked();
			}

			bool IsEventProcessingEnabled() const override
			{
				return m_EvtHandler.IsEventProcessingEnabled();
			}
			void EnableEventProcessing(bool enable = true) override
			{
				m_EvtHandler.EnableEventProcessing(enable);
			}

			// --- IWidget ---
		public:
			// Native interface
			void* GetHandle() const override
			{
				return m_Window.GetHandle();
			}
			wxWindow* GetWxWindow() const override
			{
				return static_cast<const BasicWxWidgetBase&>(m_Window).GetWxWindow();
			}

			// Lifetime management
			bool IsWidgetAlive() const override
			{
				return m_Window.IsWidgetAlive();
			}
			bool CloseWidget(bool force = false) override
			{
				return m_Window.CloseWidget(force);
			}
			bool DestroyWidget() override
			{
				return m_Window.DestroyWidget();
			}

			// Event handling
			IEvtHandler& GetEventHandler() override
			{
				return GetTopEvtHandler();
			}

			void PushEventHandler(IEvtHandler& evtHandler) override
			{
				m_EventHandlerStack.Push(evtHandler);
			}
			IEvtHandler* PopEventHandler() override
			{
				return m_EventHandlerStack.Pop();
			}
			bool RemoveEventHandler(IEvtHandler& evtHandler) override
			{
				if (&evtHandler != &m_EvtHandler)
				{
					return m_EventHandlerStack.Remove(evtHandler);
				}
				return false;
			}

			// HiDPI support
			float GetContentScaleFactor() const override
			{
				return m_Window.GetContentScaleFactor();
			}

			void FromDIP(int& x, int& y) const override
			{
				m_Window.FromDIP(x, y);
			}
			using IWidget::FromDIP;

			void ToDIP(int& x, int& y) const override
			{
				m_Window.ToDIP(x, y);
			}
			using IWidget::ToDIP;

			// Positioning functions
			Point GetPosition() const override
			{
				return m_Window.GetPosition();
			}
			void SetPosition(const Point& pos) override
			{
				m_Window.SetPosition(pos);
			}

			void Center(FlagSet<Orientation> orientation = Orientation::Both) override
			{
				m_Window.Center(orientation);
			}
			void CenterOnParent(FlagSet<Orientation> orientation = Orientation::Both) override
			{
				m_Window.CenterOnParent(orientation);
			}

			// Size functions
			Rect GetRect(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const override
			{
				return m_Window.GetRect(sizeType);
			}
			Size GetSize(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const override
			{
				return m_Window.GetSize(sizeType);
			}
			void SetSize(const Size& size, FlagSet<WidgetSizeFlag> flags = WidgetSizeFlag::Widget) override
			{
				m_Window.SetSize(size, flags);
			}

			// Coordinate conversion functions
			void ScreenToClient(int& x, int& y) const override
			{
				m_Window.ScreenToClient(x, y);
			}
			using IWidget::ScreenToClient;

			using IWidget::ClientToScreen;
			void ClientToScreen(int& x, int& y) const override
			{
				m_Window.ClientToScreen(x, y);
			}

			void DialogUnitsToPixels(int& x, int& y) const override
			{
				m_Window.DialogUnitsToPixels(x, y);
			}
			using IWidget::DialogUnitsToPixels;

			using IWidget::PixelsToDialogUnits;
			void PixelsToDialogUnits(int& x, int& y) const override
			{
				m_Window.PixelsToDialogUnits(x, y);
			}

			// Focus
			bool IsFocusable() const override
			{
				return m_Window.IsFocusable();
			}
			bool HasFocus() const override
			{
				return m_Window.HasFocus();
			}
			void SetFocus() override
			{
				m_Window.SetFocus();
			}

			bool IsFocusVisible() const override
			{
				return m_Window.IsFocusVisible();
			}
			void SetFocusVisible(bool visible = true) override
			{
				m_Window.SetFocusVisible(visible);
			}

			// Layout
			LayoutDirection GetLayoutDirection() const override
			{
				return m_Window.GetLayoutDirection();
			}
			void SetLayoutDirection(LayoutDirection direction) override
			{
				m_Window.SetLayoutDirection(direction);
			}

			void Fit() override
			{
				m_Window.Fit();
			}
			void FitInterior() override
			{
				m_Window.FitInterior();
			}
			bool Layout() override
			{
				return m_Window.Layout();
			}

			// Child management functions
			void AddChildWidget(IWidget& widget) override
			{
				m_Window.AddChildWidget(widget);
			}
			void RemoveChildWidget(const IWidget& widget) override
			{
				m_Window.RemoveChildWidget(widget);
			}
			void DestroyChildWidgets() override
			{
				m_Window.DestroyChildWidgets();
			}

			std::shared_ptr<IWidget> FindChildWidgetByID(WidgetID id) const override
			{
				return m_Window.FindChildWidgetByID(id);
			}
			std::shared_ptr<IWidget> FindChildWidgetByName(const String& widgetName) const override
			{
				return m_Window.FindChildWidgetByName(widgetName);
			}
			Enumerator<std::shared_ptr<IWidget>> EnumChildWidgets() const override
			{
				return m_Window.EnumChildWidgets();
			}

			// Sibling and parent management functions
			std::shared_ptr<IWidget> GetParentWidget() const override
			{
				return m_Window.GetParentWidget();
			}
			void SetParentWidget(IWidget& widget) override
			{
				m_Window.SetParentWidget(widget);
			}

			std::shared_ptr<IWidget> GetPrevSiblingWidget() const override
			{
				return m_Window.GetPrevSiblingWidget();
			}
			std::shared_ptr<IWidget> GetNextSiblingWidget() const override
			{
				return m_Window.GetNextSiblingWidget();
			}

			// Drawing-related functions
			bool IsFrozen() const override
			{
				return m_Window.IsFrozen();
			}
			void Freeze() override
			{
				m_Window.Freeze();
			}
			void Thaw() override
			{
				m_Window.Thaw();
			}

			Rect GetRefreshRect() const override
			{
				return m_Window.GetRefreshRect();
			}
			void Refresh(const Rect& rect = Rect::UnspecifiedRect()) override
			{
				m_Window.Refresh(rect);
			}

			Font GetFont() const override
			{
				return m_Window.GetFont();
			}
			void SetFont(const Font& font) override
			{
				m_Window.SetFont(font);
			}

			Color GetColor(WidgetColorFlag colorType) const override
			{
				return m_Window.GetColor(colorType);
			}
			void SetColor(const Color& color, FlagSet<WidgetColorFlag> flags) override
			{
				m_Window.SetColor(color, flags);
			}

			float GetTransparency() const override
			{
				return m_Window.GetTransparency();
			}
			bool SetTransparency(float value) override
			{
				return m_Window.SetTransparency(value);
			}

			// Widget state and visibility functions
			bool IsEnabled() const override
			{
				return m_Window.IsEnabled();
			}
			bool IsIntrinsicallyEnabled() const override
			{
				return m_Window.IsIntrinsicallyEnabled();
			}
			void SetEnabled(bool enabled) override
			{
				m_Window.SetEnabled(enabled);
			}

			bool IsVisible() const override
			{
				return m_Window.IsVisible();
			}
			bool IsDisplayed() const override
			{
				return m_Window.IsDisplayed();
			}
			void SetVisible(bool visible) override
			{
				m_Window.SetVisible(visible);
			}

			// Widget styles functions
			FlagSet<WidgetStyle> GetWidgetStyle() const override
			{
				return m_Window.GetWidgetStyle();
			}
			void SetWidgetStyle(FlagSet<WidgetStyle> style) override
			{
				m_Window.SetWidgetStyle(style);
			}

			FlagSet<WidgetExStyle> GetWidgetExStyle() const override
			{
				return m_Window.GetWidgetExStyle();
			}
			void SetWidgetExStyle(FlagSet<WidgetExStyle> style) override
			{
				m_Window.SetWidgetExStyle(style);
			}

			WidgetBorder GetWidgetBorder() const override
			{
				return m_Window.GetWidgetBorder();
			}
			void SetWidgetBorder(WidgetBorder border) override
			{
				m_Window.SetWidgetBorder(border);
			}

			// Widget properties
			WidgetID GetWidgetID() const override
			{
				return m_Window.GetWidgetID();
			}
			void SetWidgetID(WidgetID id) override
			{
				m_Window.SetWidgetID(id);
			}

			String GetWidgetName() const override
			{
				return m_Window.GetWidgetName();
			}
			void SetWidgetName(const String& widgetName) override
			{
				m_Window.SetWidgetName(widgetName);
			}

			String GetWidgetText(FlagSet<WidgetTextFlag> flags = {}) const override
			{
				return m_Window.GetWidgetText(flags);
			}
			void SetWidgetText(const String& widgetText, FlagSet<WidgetTextFlag> flags = {}) override
			{
				m_Window.SetWidgetText(widgetText, flags);
			}

		public:
			// INativeWidget
			void* GetNativeHandle() const override
			{
				return m_Window.GetNativeHandle();
			}
			String GetIntrinsicText() const override
			{
				return m_Window.GetIntrinsicText();
			}

			intptr_t GetWindowProperty(NativeWidgetProperty index) const override
			{
				return m_Window.GetWindowProperty(index);
			}
			intptr_t SetWindowProperty(NativeWidgetProperty index, intptr_t value) override
			{
				return m_Window.SetWindowProperty(index, value);
			}

			bool PostMessage(uint32_t messageID, intptr_t wParam = 0, intptr_t lParam = 0) override
			{
				return m_Window.PostMessage(messageID, wParam, lParam);
			}
			bool NotifyMessage(uint32_t messageID, intptr_t wParam = 0, intptr_t lParam = 0) override
			{
				return m_Window.NotifyMessage(messageID, wParam, lParam);
			}
			bool SendMessageSignal(uint32_t messageID, intptr_t wParam = 0, intptr_t lParam = 0) override
			{
				return m_Window.SendMessageSignal(messageID, wParam, lParam);
			}
			intptr_t SendMessage(uint32_t messageID, intptr_t wParam = 0, intptr_t lParam = 0, TimeSpan timeout = {}) override
			{
				return m_Window.SendMessage(messageID, wParam, lParam, timeout);
			}

			HResult SetWindowTheme(const String& applicationName, const std::vector<String>& subIDs = {}) override
			{
				return m_Window.SetWindowTheme(applicationName, subIDs);
			}
			bool EnableNonClientDPIScaling() override
			{
				return m_Window.EnableNonClientDPIScaling();
			}

			bool IsForegroundWindow() const override
			{
				return m_Window.IsForegroundWindow();
			}
			bool SetForegroundWindow() override
			{
				return m_Window.SetForegroundWindow();
			}

		public:
			BasicWxWidget& operator=(const BasicWxWidget&) = delete;
	};
}
