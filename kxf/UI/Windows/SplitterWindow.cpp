#include "kxf-pch.h"
#include "SplitterWindow.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(SplitterWindow, wxSplitterWindow);

	void SplitterWindow::OnDoubleClickSash(int x, int y)
	{
		SetSashPosition(m_InitialPosition != 0 ? m_InitialPosition : GetMinimumPaneSize(), true);
	}
	void SplitterWindow::OnDrawSash(wxPaintEvent& event)
	{
		wxAutoBufferedPaintDC dc(this);

		int sashSize = GetSashSize();
		if (sashSize != 0)
		{
			dc.SetBrush(m_SashColor);
			dc.SetPen(*wxTRANSPARENT_PEN);

			if (GetSplitMode() == wxSPLIT_VERTICAL)
			{
				dc.DrawRectangle(GetSashPosition(), 0, sashSize, GetSize().GetHeight());
			}
			else
			{
				dc.DrawRectangle(0, GetSashPosition(), GetSize().GetWidth(), sashSize);
			}
		}
	}

	bool SplitterWindow::Create(wxWindow* parent,
								wxWindowID id,
								FlagSet<SplitterWindowStyle> style
	)
	{
		if (wxSplitterWindow::Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt()))
		{
			wxSplitterWindow::m_isHot = true;
			return true;
		}
		return false;
	}

	bool SplitterWindow::SplitHorizontally(wxWindow* window1, wxWindow* window2, int sashPosition)
	{
		m_InitialPosition = sashPosition;
		return wxSplitterWindow::SplitHorizontally(window1, window2, sashPosition);
	}
	bool SplitterWindow::SplitVertically(wxWindow* window1, wxWindow* window2, int sashPosition)
	{
		m_InitialPosition = sashPosition;
		return wxSplitterWindow::SplitVertically(window1, window2, sashPosition);
	}

	void SplitterWindow::SetInitialPosition(int pos)
	{
		m_InitialPosition = pos;
		SetSashPosition(pos, false);
		//wxSplitterWindow::m_sashPosition = pos;
		//wxSplitterWindow::m_sashPositionCurrent = pos;
		//wxSplitterWindow::m_needUpdating = true;
	}
	void SplitterWindow::SetSashColor(const Color& color)
	{
		m_SashColor = color;
		if (m_SashColor)
		{
			SetBackgroundStyle(wxBG_STYLE_PAINT);
			Bind(wxEVT_PAINT, &SplitterWindow::OnDrawSash, this);
		}
		else
		{
			SetBackgroundStyle(wxBG_STYLE_ERASE);
			Unbind(wxEVT_PAINT, &SplitterWindow::OnDrawSash, this);
		}
	}
}
