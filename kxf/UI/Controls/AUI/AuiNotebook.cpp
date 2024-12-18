#include "kxf-pch.h"
#include "AuiNotebook.h"
#include "kxf/Drawing/GDIRenderer/UxTheme.h"
#include "kxf/Drawing/GDIRenderer/GDIContext.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(AuiNotebook, wxAuiNotebook);

	bool AuiNotebook::Create(wxWindow* parent,
							 wxWindowID id,
							 FlagSet<AuiNotebookStyle> style
	)
	{
		if (wxAuiNotebook::Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt()))
		{
			auto font = parent->GetFont();
			SetNormalFont(font);
			SetMeasuringFont(font);
			SetSelectedFont(font);

			m_mgr.SetArtProvider(new AuiNotebookDockArtProvider(*this));
			SetRenderer(RendererType::Default);
			return true;
		}
		return false;
	}

	void AuiNotebook::SetRenderer(RendererType value)
	{
		m_CurrentRendererType = value;
		switch (value)
		{
			case RendererType::Generic:
			{
				SetArtProvider(new wxAuiGenericTabArt());
				break;
			}
			case RendererType::Simple:
			{
				SetArtProvider(new wxAuiSimpleTabArt());
				break;
			}
			default:
			{
				SetArtProvider(new AuiDefaultTabArt(*this));
			}
		};
	}
}

namespace kxf::UI
{
	void AuiDefaultTabArt::DrawBackground(wxDC& dc, wxWindow* window, const wxRect& rect)
	{
		AuiDefaultTabArtBase::DrawBackground(dc, window, rect);
	}
	void AuiDefaultTabArt::DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent)
	{
		AuiDefaultTabArtBase::DrawTab(dc, wnd, pane, inRect, closeButtonState, outTabRect, outButtonRect, xExtent);
	}
	void AuiDefaultTabArt::DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, int bitmapId, int buttonState, int orientation, wxRect* outRect)
	{
		AuiDefaultTabArtBase::DrawButton(dc, wnd, inRect, bitmapId, buttonState, orientation, outRect);
	}
}

namespace kxf::UI
{
	void AuiNotebookDockArtProvider::DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
	{
		AuiDefaultDockArtBase::DrawBackground(dc, window, orientation, rect);
	}
	void AuiNotebookDockArtProvider::DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane)
	{
		GDIContext context(dc);

		// Border
		AuiDefaultDockArtBase::DrawBorder(dc, window, rect, pane);

		// Top horizontal line
		Rect topLine = Rect(wxRect(rect.GetLeftTop(), rect.GetRightTop()));
		topLine.Width() += 1;
		topLine.Height() = 1;
		UxTheme::DrawParentBackground(*m_Notebook, context, topLine);

		// Right vertical line
		Rect rightLine = Rect(wxRect(topLine.GetRightTop(), topLine.GetRightBottom()));
		rightLine.X() += 1;
		rightLine.Y() -= 2;
		rightLine.Width() += 5;
		rightLine.Height() += 2;
		UxTheme::DrawParentBackground(*m_Notebook, context, rightLine);

		// Left vertical line
		Rect leftLine;
		leftLine.X() = 0;
		leftLine.Y() = 0;
		leftLine.Width() = m_Notebook->GetTabIndent();
		leftLine.Height() = m_Notebook->GetTabCtrlHeight() - 1;
		UxTheme::DrawParentBackground(*m_Notebook, context, leftLine);
	}
}
