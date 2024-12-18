#pragma once
#include "Common.h"
#include "../../IToolBarWidget.h"
#include "../../IToolBarWidgetItem.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include <wx/aui/auibar.h>
#include <wx/systhemectrl.h>

namespace kxf::Widgets
{
	class ToolBar;
	class ToolBarItem;
}
namespace kxf::WXUI::Private
{
	class ToolBarRenderer;
}

namespace kxf::WXUI
{
	class KXF_API ToolBar: public EvtHandlerWrapper<ToolBar, UI::WindowRefreshScheduler<wxSystemThemedControl<wxAuiToolBar>>>
	{
		friend class EvtHandlerWrapper;
		friend class Private::ToolBarRenderer;
		friend class Widgets::ToolBarItem;
		friend class Widgets::ToolBar;

		private:
			Widgets::ToolBar& m_Widget;
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;

			std::unordered_map<const wxAuiToolBarItem*, std::shared_ptr<Widgets::ToolBarItem>> m_Items;
			Color m_ColorBorder;
			bool m_ItemsChanged = false;

		private:
			bool DoTryBefore(wxEvent& event) noexcept;

			std::shared_ptr<IToolBarWidgetItem> DoCreateItem(wxAuiToolBarItem* item, size_t index, WidgetID id);
			std::shared_ptr<IToolBarWidgetItem> DoGetItem(const wxAuiToolBarItem& item);
			bool DoRemoveItem(wxAuiToolBarItem& item);

			size_t DoGetItemIndex(const wxAuiToolBarItem& item) const;
			bool DoSetItemIndex(wxAuiToolBarItem& item, size_t newIndex);

		protected:
			// wxWindow
			void OnInternalIdle() override;

		public:
			ToolBar(Widgets::ToolBar& widget);
			~ToolBar();

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			Color GetBorderColor() const
			{
				return m_ColorBorder;
			}
			void SetBorderColor(const Color& color)
			{
				m_ColorBorder = color;
				ScheduleRefresh();
			}

			bool IsOverflowVisible() const
			{
				return wxAuiToolBar::GetOverflowVisible();
			}
			bool IsGripperVisible() const
			{
				return wxAuiToolBar::GetGripperVisible();
			}
			bool IsVertical() const
			{
				return GetWindowStyle() & wxAUI_TB_VERTICAL;
			}

			std::shared_ptr<IToolBarWidgetItem> AddItem(size_t index, const String& label, wxItemKind kind, WidgetID id);
			std::shared_ptr<IToolBarWidgetItem> AddWidget(size_t index, wxControl* control, WidgetID id);
			std::shared_ptr<IToolBarWidgetItem> AddSeparator(size_t index, WidgetID id);
			std::shared_ptr<IToolBarWidgetItem> AddSpacer(size_t index, int size, WidgetID id);
			std::shared_ptr<IToolBarWidgetItem> AddStretchSpacer(size_t index, int proportion, WidgetID id);
			bool RemoveItem(size_t index);
			void Clear();

			std::shared_ptr<IToolBarWidgetItem> FindToolByPosition(const Point& pos);
			std::shared_ptr<IToolBarWidgetItem> FindToolByIndex(size_t index);
			std::shared_ptr<IToolBarWidgetItem> FindToolByID(const WidgetID& id);

			void UpdateUI();
	};
}

namespace kxf::WXUI::Private
{
	class KXF_API ToolBarRenderer: public wxAuiDefaultToolBarArt
	{
		private:
			ToolBar* m_ToolBar = nullptr;

		public:
			ToolBarRenderer(ToolBar& toolbar)
				:m_ToolBar(&toolbar)
			{
			}

		public:
			void DrawPlainBackground(wxDC& dc, wxWindow* window, const wxRect& rect) override;
	};
}
