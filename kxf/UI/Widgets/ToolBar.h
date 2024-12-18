#pragma once
#include "Common.h"
#include "../IToolBarWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::WXUI
{
	class ToolBar;
}
namespace kxf::Widgets
{
	class ToolBarItem;
}

namespace kxf::Widgets
{
	class KXF_API ToolBar: public RTTI::DynamicImplementation<ToolBar, Private::BasicWxWidget<ToolBar, WXUI::ToolBar, IToolBarWidget>, IGraphicsRendererAwareWidget>
	{
		kxf_RTTI_DeclareIID(ToolBar, {0x7caacd52, 0xf264, 0x4877, {0xa1, 0x8e, 0x9e, 0xfa, 0xa0, 0xd2, 0x3b, 0xa}});

		friend class ToolBarItem;

		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			ToolBar();
			~ToolBar();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			Color GetColor(WidgetColorFlag colorType) const override;
			void SetColor(const Color& color, FlagSet<WidgetColorFlag> flags) override;

			// IToolBarWidget
			std::shared_ptr<IToolBarWidgetItem> InsertItem(const String& label, ItemType type = ItemType::Regular, WidgetID id = {}, size_t index = npos) override;
			std::shared_ptr<IToolBarWidgetItem> InsertWidgetItem(IWidget& widget, size_t index = npos) override;
			void RemoveItem(size_t index) override;

			size_t GetItemCount() const override;
			Enumerator<std::shared_ptr<IToolBarWidgetItem>> EnumItems() override;

			std::shared_ptr<IToolBarWidgetItem> GetItemByID(const WidgetID& id) override;
			std::shared_ptr<IToolBarWidgetItem> GetItemByIndex(size_t index) override;
			std::shared_ptr<IToolBarWidgetItem> GetItemByPosition(const Point& point) override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
