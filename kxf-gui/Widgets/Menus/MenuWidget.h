#pragma once
#include "Common.h"
#include "../IWidget.h"
#include "../IGraphicsRendererAwareWidget.h"
#include "../Private/WidgetFreezeHandler.h"
#include "../Private/AnonymousNativeWindow.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/EventSystem/BasicEvtHandler.h"
#include "kxf/EventSystem/EventHandlerStack.h"
class wxMenu;
class wxMenuItem;

namespace kxf::Private
{
	class MenuWidgetGuard;
}
namespace kxf::WXUI
{
	class Menu;
	class MenuItem;
}
namespace kxf::Widgets
{
	class MenuWidgetItem;
}

namespace kxf::Widgets
{
	class MenuWidget: public RTTI::DynamicImplementation<MenuWidget, BasicEvtHandler<MenuWidget, IMenuWidget>, IGraphicsRendererAwareWidget>
	{
		kxf_RTTI_DeclareIID(MenuWidget, {0x3ed24a86, 0xd924, 0x4c39, {0xae, 0x70, 0x40, 0x7b, 0x45, 0x47, 0xea, 0x16}});

		friend class MenuWidgetItem;
		friend class BasicEvtHandler<MenuWidget, IMenuWidget>;
		friend class Private::MenuWidgetGuard;
		friend class WXUI::Menu;
		friend class WXUI::MenuItem;

		protected:
			static void AssociateWXMenuItem(wxMenuItem& wx, IMenuWidgetItem& item) noexcept;
			static void DissociateWXMenuItem(wxMenuItem& wx) noexcept;
			static std::shared_ptr<IMenuWidgetItem> FindByWXMenuItem(const wxMenuItem& wx) noexcept;

		protected:
			EvtHandler m_EvtHandler;
			EvtHandlerStack m_EventHandlerStack;

			std::unique_ptr<WXUI::Menu> m_Menu;
			std::shared_ptr<IWidget> m_ParentWidget;
			std::shared_ptr<IGraphicsRenderer> m_Renderer;
			bool m_IsAttached = false;

			// Invoking environment data
			Private::AnonymousNativeWindow m_NativeWindow;
			std::shared_ptr<IWidget> m_InvokingWidget;
			uint32_t m_InvokingThread = 0;
			Point m_InvokingPosition = Point::UnspecifiedPosition();

			// Options
			String m_Name;
			String m_Label;
			String m_Description;
			LayoutDirection m_LayoutDirection = LayoutDirection::Default;
			WidgetBorder m_WidgetBorder = WidgetBorder::Default;
			Private::WidgetFreezeHandler m_FreezeHandler;
			Point m_PopupPosition = Point::UnspecifiedPosition();
			Size m_MaxSize = Size::UnspecifiedSize();
			Font m_Font;
			Color m_ColorBackground;
			Color m_ColorBorder;
			Color m_ColorText;
			float m_Transparency = 0.0f;
			float m_VisibleFocus = false;
			bool m_IsEnabled = true;

		protected:
			EvtHandler& GetThisEvtHandler() noexcept
			{
				return m_EvtHandler;
			}
			const EvtHandler& GetThisEvtHandler() const noexcept
			{
				return m_EvtHandler;
			}

			IEvtHandler& GetTopEvtHandler() noexcept
			{
				return *m_EventHandlerStack.GetTop();
			}
			const IEvtHandler& GetTopEvtHandler() const noexcept
			{
				return *m_EventHandlerStack.GetTop();
			}

			std::shared_ptr<IWidget> GetAnyWidget() const
			{
				if (m_ParentWidget)
				{
					return m_ParentWidget;
				}
				return GetInvokingWidget();
			}
			bool HandleMessage(intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam);
			bool DoShow(Point screenPos, FlagSet<Alignment> alignment, std::shared_ptr<IWidget> invokingWidget);

			virtual std::shared_ptr<MenuWidgetItem> DoCreateItem();
			bool DoDestroyWidget(bool releaseWX = false);
			void OnWXMenuDestroyed();

		public:
			MenuWidget();
			MenuWidget(const MenuWidget&) = delete;
			~MenuWidget();

			// --- IWidget ---
		public:
			// Native interface
			void* GetHandle() const override;
			wxWindow* GetWxWindow() const override
			{
				return nullptr;
			}

			// Lifetime management
			bool IsWidgetAlive() const override;
			bool CreateWidget(std::shared_ptr<IWidget> parent = {}, const String& text = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;
			bool CloseWidget(bool force = false) override;
			bool DestroyWidget() override;

			// Event handling
			IEvtHandler& GetEventHandler() override
			{
				return m_EvtHandler;
			}

			void PushEventHandler(IEvtHandler& evtHandler) override
			{
				m_EventHandlerStack.Push(evtHandler);
			}
			bool RemoveEventHandler(IEvtHandler& evtHandler) override
			{
				if (&evtHandler != &m_EvtHandler)
				{
					return m_EventHandlerStack.Remove(evtHandler);
				}
				return false;
			}
			IEvtHandler* PopEventHandler() override
			{
				return m_EventHandlerStack.Pop();
			}

			// HiDPI support
			float GetContentScaleFactor() const override
			{
				if (auto widget = GetAnyWidget())
				{
					return widget->GetContentScaleFactor();
				}
				return 1.0f;
			}

			void FromDIP(int& x, int& y) const override
			{
				if (auto widget = GetAnyWidget())
				{
					return widget->FromDIP(x, y);
				}
			}
			using IWidget::FromDIP;

			void ToDIP(int& x, int& y) const override
			{
				if (auto widget = GetAnyWidget())
				{
					return widget->ToDIP(x, y);
				}
			}
			using IWidget::ToDIP;

			// Positioning functions
			Point GetPosition() const override
			{
				return m_InvokingPosition;
			}
			void SetPosition(const Point& pos) override
			{
				m_PopupPosition = pos;
			}

			void Center(FlagSet<Orientation> orientation = Orientation::Both) override
			{
			}
			void CenterOnParent(FlagSet<Orientation> orientation = Orientation::Both) override
			{
			}

			// Size functions
			Rect GetRect(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const override
			{
				return Rect::UnspecifiedRect();
			}
			Size GetSize(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const override
			{
				return Size::UnspecifiedSize();
			}
			void SetSize(const Size& size, FlagSet<WidgetSizeFlag> flags = WidgetSizeFlag::Widget) override
			{
				if (flags.Contains(WidgetSizeFlag::Widget) || flags.Contains(WidgetSizeFlag::WidgetMax) || flags.Contains(WidgetSizeFlag::Client) || flags.Contains(WidgetSizeFlag::ClientMax))
				{
					m_MaxSize = size;
				}
			}

			// Coordinate conversion functions
			void ScreenToClient(int& x, int& y) const override
			{
				if (auto widget = GetAnyWidget())
				{
					return widget->ClientToScreen(x, y);
				}
			}
			using IWidget::ScreenToClient;

			void ClientToScreen(int& x, int& y) const override
			{
				if (auto widget = GetAnyWidget())
				{
					return widget->ClientToScreen(x, y);
				}
			}
			using IWidget::ClientToScreen;

			void DialogUnitsToPixels(int& x, int& y) const override
			{
				if (auto widget = GetAnyWidget())
				{
					return widget->DialogUnitsToPixels(x, y);
				}
			}
			using IWidget::DialogUnitsToPixels;

			void PixelsToDialogUnits(int& x, int& y) const override
			{
				if (auto widget = GetAnyWidget())
				{
					return widget->PixelsToDialogUnits(x, y);
				}
			}
			using IWidget::PixelsToDialogUnits;

			// Focus
			bool IsFocusable() const override
			{
				return true;
			}
			bool HasFocus() const override
			{
				return m_InvokingThread != 0;
			}
			void SetFocus() override
			{
			}

			bool IsFocusVisible() const override
			{
				return m_VisibleFocus;
			}
			void SetFocusVisible(bool visible = true) override
			{
				m_VisibleFocus = visible;
			}

			// Layout
			LayoutDirection GetLayoutDirection() const override
			{
				if (m_LayoutDirection == LayoutDirection::Default)
				{
					if (auto widget = GetAnyWidget())
					{
						return widget->GetLayoutDirection();
					}
				}
				return m_LayoutDirection;
			}
			void SetLayoutDirection(LayoutDirection direction) override
			{
				m_LayoutDirection = direction;
			}

			void Fit() override
			{
			}
			void FitInterior() override
			{
			}
			bool Layout() override
			{
				return false;
			}

			// Child management functions
			void AddChildWidget(IWidget& widget) override;
			void RemoveChildWidget(const IWidget& widget) override;
			void DestroyChildWidgets() override;

			std::shared_ptr<IWidget> FindChildWidgetByID(WidgetID id) const override;
			std::shared_ptr<IWidget> FindChildWidgetByName(const String& widgetName) const override;
			Enumerator<std::shared_ptr<IWidget>> EnumChildWidgets() const override;

			// Sibling and parent management functions
			std::shared_ptr<IWidget> GetParentWidget() const override
			{
				return m_ParentWidget;
			}
			void SetParentWidget(IWidget& widget) override
			{
				m_ParentWidget = widget.QueryInterface<IWidget>();
			}

			std::shared_ptr<IWidget> GetPrevSiblingWidget() const override
			{
				return nullptr;
			}
			std::shared_ptr<IWidget> GetNextSiblingWidget() const override
			{
				return nullptr;
			}

			// Drawing-related functions
			bool IsFrozen() const override
			{
				return m_FreezeHandler.IsFrozen();
			}
			void Freeze() override
			{
				m_FreezeHandler.Freeze();
			}
			void Thaw() override
			{
				m_FreezeHandler.Thaw();
			}

			Rect GetRefreshRect() const override
			{
				return Rect::UnspecifiedRect();
			}
			void Refresh(const Rect& rect = Rect::UnspecifiedRect()) override
			{
			}

			Font GetFont() const override
			{
				return m_Font;
			}
			void SetFont(const Font& font) override
			{
				m_Font = font;
			}

			Color GetColor(WidgetColorFlag colorType) const override
			{
				switch (colorType)
				{
					case WidgetColorFlag::Background:
					{
						return m_ColorBackground;
					}
					case  WidgetColorFlag::Border:
					{
						return m_ColorBorder;
					}
					case WidgetColorFlag::Foreground:
					case WidgetColorFlag::Text:
					{
						return m_ColorText;
					}
				}
				return {};
			}
			void SetColor(const Color& color, FlagSet<WidgetColorFlag> flags) override
			{
				if (flags.Contains(WidgetColorFlag::Background))
				{
					m_ColorBackground = color;
				}
				if (flags.Contains(WidgetColorFlag::Border))
				{
					m_ColorBorder = color;
				}
				if (flags.Contains(WidgetColorFlag::Foreground) || flags.Contains(WidgetColorFlag::Text))
				{
					m_ColorText = color;
				}
			}

			float GetTransparency() const override
			{
				return m_Transparency;
			}
			bool SetTransparency(float value) override
			{
				return std::clamp(m_Transparency, 0.0f, 1.0f);
			}

			// Widget state and visibility functions
			bool IsEnabled() const override
			{
				if (m_IsEnabled)
				{
					if (auto widget = GetAnyWidget())
					{
						return widget->IsEnabled();
					}
				}
				return m_IsEnabled;
			}
			bool IsIntrinsicallyEnabled() const override
			{
				return m_IsEnabled;
			}
			void SetEnabled(bool enabled) override
			{
				m_IsEnabled = enabled;
			}

			bool IsVisible() const override
			{
				return m_InvokingThread != 0;
			}
			bool IsDisplayed() const override
			{
				return m_InvokingThread != 0;
			}
			void SetVisible(bool visible) override
			{
				if (!visible)
				{
					MenuWidget::CloseWidget();
				}
			}

			// Widget styles functions
			FlagSet<WidgetStyle> GetWidgetStyle() const override
			{
				return {};
			}
			void SetWidgetStyle(FlagSet<WidgetStyle> style) override
			{
			}

			FlagSet<WidgetExStyle> GetWidgetExStyle() const override
			{
				return {};
			}
			void SetWidgetExStyle(FlagSet<WidgetExStyle> style) override
			{
			}

			WidgetBorder GetWidgetBorder() const override
			{
				return m_WidgetBorder;
			}
			void SetWidgetBorder(WidgetBorder border) override
			{
				m_WidgetBorder = border;
			}

			// Widget properties
			WidgetID GetWidgetID() const override
			{
				return {};
			}
			void SetWidgetID(WidgetID id) override
			{
			}

			String GetWidgetName() const override
			{
				return m_Name;
			}
			void SetWidgetName(const String& widgetName) override
			{
				m_Name = widgetName;
			}

			String GetWidgetText(FlagSet<WidgetTextFlag> flags = {}) const override;
			void SetWidgetText(const String& widgetText, FlagSet<WidgetTextFlag> flags = {}) override;

		public:
			// IMenuWidget
			String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const override
			{
				return MenuWidget::GetWidgetText(flags);
			}
			void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) override
			{
				MenuWidget::SetWidgetText(label, flags);
			}

			String GetDescription() const override
			{
				return m_Description;
			}
			void SetDescription(const String& description) override
			{
				m_Description = description;
			}

			using IMenuWidget::InsertItem;
			std::shared_ptr<IMenuWidgetItem> InsertItem(IMenuWidgetItem& item, size_t index = npos) override;
			std::shared_ptr<IMenuWidgetItem> InsertMenu(IMenuWidget& subMenu, const String& label = {}, WidgetID id = {}, size_t index = npos) override;

			std::shared_ptr<IMenuWidgetItem> CreateItem(const String& label, ItemType type = ItemType::Regular, WidgetID id = {}) override;
			std::shared_ptr<IMenuWidgetItem> GetDefaultItem() const override;

			size_t GetMenuItemCount() const override;
			Enumerator<std::shared_ptr<IMenuWidgetItem>> EnumMenuItems() const override;

			void Show(Point pos = Point::UnspecifiedPosition(), FlagSet<Alignment> alignment = {}) override;
			void ShowAt(const IWidget& widget, Point pos = Point::UnspecifiedPosition(), FlagSet<Alignment> alignment = {}) override;
			void ShowWithOffset(const IWidget& widget, int offset = 1, FlagSet<Alignment> alignment = {}) override;

			Point GetInvokingPosition() const override;
			std::shared_ptr<IWidget> GetInvokingWidget() const override;

		public:
			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}

		public:
			MenuWidget& operator=(const MenuWidget&) = delete;
	};
}
