#pragma once
#include "Common.h"
#include "../../IBookWidget.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include <wx/aui/auibook.h>
#include <wx/aui/dockart.h>
#include <wx/systhemectrl.h>

namespace kxf::WXUI::Private
{
	class TabBookTabRenderer;
	class TabBookDockRenderer;
}

namespace kxf::WXUI
{
	class KXF_API TabBook: public EvtHandlerWrapper<TabBook, UI::WindowRefreshScheduler<wxSystemThemedControl<wxAuiNotebook>>>
	{
		friend class EvtHandlerWrapper;
		friend class Private::TabBookTabRenderer;
		friend class Private::TabBookDockRenderer;

		private:
			IBookWidget& m_Widget;
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;

			int m_TabIndent = 3;

		private:
			bool DoTryBefore(wxEvent& event);

		public:
			TabBook(IBookWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			// wxWindow
			bool ShouldInheritColours() const override
			{
				return true;
			}

			int GetTabIndent() const
			{
				return m_TabIndent;
			}
			void SetTabIndent(int value)
			{
				m_TabIndent = value;
			}
	};
}

namespace kxf::WXUI::Private
{
	class KXF_API TabBookTabRenderer: public wxAuiDefaultTabArt
	{
		private:
			TabBook* m_TabBook = nullptr;

		public:
			TabBookTabRenderer(TabBook& tabBook)
				:m_TabBook(&tabBook)
			{
			}

		public:
			TabBookTabRenderer* Clone() override
			{
				return new TabBookTabRenderer(*this);
			}

			int GetIndentSize() override
			{
				if (m_TabBook)
				{
					return m_TabBook->GetTabIndent();
				}
				return wxAuiDefaultTabArt::GetIndentSize();
			}

			void DrawBackground(wxDC& dc, wxWindow* window, const wxRect& rect) override;
			void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent) override;
			void DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, int bitmapId, int buttonState, int orientation, wxRect* outRect) override;
	};
}

namespace kxf::WXUI::Private
{
	class KXF_API TabBookDockRenderer: public wxAuiDefaultDockArt
	{
		private:
			TabBook* m_TabBook = nullptr;

		public:
			TabBookDockRenderer(TabBook& tabBook)
				:m_TabBook(&tabBook)
			{
			}

		public:
			void DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect) override;
			void DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane) override;
	};
}
