#include "kxf-pch.h"
#include "TabBook.h"
#include "kxf/Drawing/GDIRenderer/GDIContext.h"
#include "kxf/Drawing/GDIRenderer/UxTheme.h"

namespace kxf::WXUI
{
	bool TabBook::DoTryBefore(wxEvent& event)
	{
		return TranslateBookEvent(m_Widget, event);
	}

	bool TabBook::Create(wxWindow* parent,
						 const String& label,
						 const Point& pos,
						 const Size& size
	)
	{
		if (wxAuiNotebook::Create(parent, wxID_NONE, pos, size, wxAUI_NB_TOP|wxAUI_NB_SCROLL_BUTTONS))
		{
			SetLabel(label);
			EnableSystemTheme();
			if (ShouldInheritColours())
			{
				SetBackgroundColour(parent->GetBackgroundColour());
			}

			if (m_Widget.QueryInterface(m_RendererAware))
			{
				SetArtProvider(new Private::TabBookTabRenderer(*this));
				m_mgr.SetArtProvider(new Private::TabBookDockRenderer(*this));

				return true;
			}
		}
		return false;
	}
}

namespace kxf::WXUI::Private
{
	void TabBookTabRenderer::DrawBackground(wxDC& dc, wxWindow* window, const wxRect& rect)
	{
		wxAuiDefaultTabArt::DrawBackground(dc, window, rect);
	}
	void TabBookTabRenderer::DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent)
	{
		wxAuiDefaultTabArt::DrawTab(dc, wnd, pane, inRect, closeButtonState, outTabRect, outButtonRect, xExtent);
	}
	void TabBookTabRenderer::DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, int bitmapId, int buttonState, int orientation, wxRect* outRect)
	{
		wxAuiDefaultTabArt::DrawButton(dc, wnd, inRect, bitmapId, buttonState, orientation, outRect);
	}
}

namespace kxf::WXUI::Private
{
	void TabBookDockRenderer::DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
	{
		wxAuiDefaultDockArt::DrawBackground(dc, window, orientation, rect);
	}
	void TabBookDockRenderer::DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane)
	{
		GDIContext context(dc);

		// Border
		wxAuiDefaultDockArt::DrawBorder(dc, window, rect, pane);

		// Top horizontal line
		Rect topLine = Rect(wxRect(rect.GetLeftTop(), rect.GetRightTop()));
		topLine.Width() += 1;
		topLine.Height() = 1;
		UxTheme::DrawParentBackground(*m_TabBook, context, topLine);

		// Right vertical line
		Rect rightLine = Rect(wxRect(topLine.GetRightTop(), topLine.GetRightBottom()));
		rightLine.X() += 1;
		rightLine.Y() -= 2;
		rightLine.Width() += 5;
		rightLine.Height() += 2;
		UxTheme::DrawParentBackground(*m_TabBook, context, rightLine);

		// Left vertical line
		Rect leftLine;
		leftLine.X() = 0;
		leftLine.Y() = 0;
		leftLine.Width() = m_TabBook->GetTabIndent();
		leftLine.Height() = m_TabBook->GetTabCtrlHeight() - 1;
		UxTheme::DrawParentBackground(*m_TabBook, context, leftLine);
	}
}
