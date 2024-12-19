#pragma once
#include "Common.h"
#include "../IMenuWidget.h"
#include "../IMenuWidgetItem.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/EventSystem/BasicEvtHandler.h"
#include "kxf/EventSystem/EventHandlerStack.h"
#include "kxf-gui/Drawing/IRendererNative.h"
class wxMenu;
class wxMenuItem;

namespace kxf
{
	class IGraphicsContext;
}
namespace kxf::WXUI
{
	class MenuItem;
}
namespace kxf::Widgets
{
	class MenuWidget;
}

namespace kxf::Widgets
{
	class MenuWidgetItem: public RTTI::DynamicImplementation<MenuWidgetItem, BasicEvtHandler<MenuWidgetItem, IMenuWidgetItem>>
	{
		kxf_RTTI_DeclareIID(MenuWidgetItem, {0x56983711, 0xc1a4, 0x48ec, {0x97, 0xfd, 0x61, 0x4b, 0x53, 0x2f, 0xc0, 0x44}});

		friend class MenuWidget;
		friend class WXUI::MenuItem;
		friend class BasicEvtHandler<MenuWidgetItem, IMenuWidgetItem>;

		protected:
			EvtHandler m_EvtHandler;
			std::unique_ptr<WXUI::MenuItem> m_MenuItem;
			std::weak_ptr<MenuWidget> m_OwningMenu;

			// Options
			WidgetID m_ItemID;
			BitmapImage m_Icon;
			Size m_MinSize = Size::UnspecifiedSize();
			Size m_MaxSize = Size::UnspecifiedSize();

		protected:
			EvtHandler& GetThisEvtHandler() noexcept
			{
				return m_EvtHandler;
			}
			const EvtHandler& GetThisEvtHandler() const noexcept
			{
				return m_EvtHandler;
			}

			EvtHandler& GetTopEvtHandler() noexcept
			{
				return m_EvtHandler;
			}
			const EvtHandler& GetTopEvtHandler() const noexcept
			{
				return m_EvtHandler;
			}

			void DoCreateWidget();
			bool DoDestroyWidget(bool releaseWX = false);
			void OnWXMenuDestroyed();

			bool IsFirstItem() const;
			bool IsLastItem() const;

		protected:
			SizeF OnMeasureItem(SizeF size) const;
			void OnDrawItem(std::shared_ptr<IGraphicsContext> gc, RectF rect, FlagSet<NativeWidgetFlag> flags);

		public:
			MenuWidgetItem();
			MenuWidgetItem(const MenuWidgetItem&) = delete;
			~MenuWidgetItem();

		public:
			// IWidgetItem
			std::shared_ptr<IWidget> GetOwningWidget() const override;

			String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const override;
			void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) override;

			String GetDescription() const override;
			void SetDescription(const String& description) override;

			WidgetID GetID() const override;
			void SetID(WidgetID id) override;

			BitmapImage GetIcon() const override;
			void SetIcon(const BitmapImage& icon) override;

			bool IsEnabled() const override;
			void SetEnabled(bool enabled = true) override;

			bool IsVisible() const override;
			void SetVisible(bool visible) override;

			Point GetPosition() const override;
			void SetPosition(const Point& pos) override;

			Rect GetRect(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const override;
			Size GetSize(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const override;
			void SetSize(const Size& size, FlagSet<WidgetSizeFlag> sizeType = WidgetSizeFlag::Widget) override;

			// --- IMenuWidgetItem ---
			// General
			std::shared_ptr<IMenuWidget> GetSubMenu() const override;
			void SetSubMenu(IMenuWidget& subMenu) override;

			bool IsDefaultItem() const override;
			void SetDefaultItem() override;

			// Properties
			ItemType GetItemType() const override;

			bool IsChecked() const override;
			void SetChecked(bool checked = true) override;

		public:
			// MenuWidgetItem
			wxMenuItem* GetWxItem() const;

		public:
			MenuWidgetItem& operator=(const MenuWidgetItem&) = delete;
	};
}
