#pragma once
#include "Common.h"
#include "../IToolBarWidget.h"
#include "../IToolBarWidgetItem.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/EventSystem/BasicEvtHandler.h"
#include "kxf/EventSystem/EventHandlerStack.h"
class wxAuiToolBarItem;

namespace kxf::WXUI
{
	class ToolBar;
}
namespace kxf::Widgets
{
	class ToolBar;
}

namespace kxf::Widgets
{
	class ToolBarItem: public RTTI::Implementation<ToolBarItem, BasicEvtHandler<ToolBarItem, IToolBarWidgetItem>>
	{
		friend class BasicEvtHandler;
		friend class WXUI::ToolBar;

		protected:
			EvtHandler m_EvtHandler;
			wxAuiToolBarItem* m_Item = nullptr;
			std::weak_ptr<ToolBar> m_OwningToolBar;

			WidgetID m_ItemID;

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

			void OnWXItemDestroyed();
			void ScheduleRefreshItem();

		public:
			ToolBarItem();
			ToolBarItem(ToolBar& toolBar, wxAuiToolBarItem& item);
			ToolBarItem(const ToolBarItem&) = delete;
			~ToolBarItem();

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

			// IToolBarWidgetItem
			String GetTooltip() const override;
			void SetTooltip(const String& tooltip) override;

			bool IsChecked() const override;
			void SetChecked(bool checked = true) override;

			int GetProportion() const override;
			void SetProportion(int proportion) override;

			int GetSpacerSize() const override;
			void SetSpacerSize(int size) override;

			FlagSet<Alignment> GetAlignment() const override;
			void SetAlignment(FlagSet<Alignment> alignment) override;

			std::shared_ptr<IWidget> GetHostedWidget() const override;
			size_t GetIndex() const override;
			ItemType GetItemType() const override;

		public:
			// ToolBarWidgetItem
			wxAuiToolBarItem* GetWxItem() const noexcept
			{
				return m_Item;
			}

		public:
			ToolBarItem& operator=(const ToolBarItem&) = delete;
	};
}
