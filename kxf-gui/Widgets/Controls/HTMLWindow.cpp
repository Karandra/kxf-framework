#include "kxf-pch.h"
#include "HTMLWindow.h"
#include "kxf/Localization/Common.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIWindowContext.h"
#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(HTMLWindow, wxHtmlWindow);

	String HTMLWindow::ProcessPlainText(const String& text)
	{
		if (text.StartsWith("<html>") && text.EndsWith("</html>"))
		{
			return text;
		}
		else
		{
			String temp = text;
			temp.Replace("\r\n", "<br/>");
			temp.Replace("\r", "<br/>");
			temp.Replace("\n", "<br/>");
			return temp;
		}
	}
	bool HTMLWindow::SetupFontsUsing(const GDIFont& normalFont, String& normalFace, String& fixedFace, int& pointSize)
	{
		if (normalFont)
		{
			GDIFont fixedFont(normalFont);
			auto UsingFixedFont = [&]()
			{
				pointSize = normalFont.GetPointSize();
				normalFace = normalFont.GetFaceName();
				fixedFace = fixedFont.GetFaceName();
			};

			if (fixedFont.SetFaceName("Consolas") || fixedFont.SetFaceName("Courier New"))
			{
				UsingFixedFont();
			}
			else if (fixedFont.SetFamily(FontFamily::FixedWidth); fixedFont)
			{
				UsingFixedFont();
			}
			else
			{
				pointSize = normalFont.GetPointSize();
				normalFace = normalFont.GetFaceName();
				fixedFace = normalFace;
			}
			return true;
		}
		return false;
	}

	void HTMLWindow::CopyTextToClipboard(const String& value) const
	{
		if (wxTheClipboard->Open())
		{
			wxTheClipboard->SetData(new wxTextDataObject(value));
			wxTheClipboard->Close();
		}
	}
	void HTMLWindow::CreateContextMenu(Menu& menu, const wxHtmlLinkInfo* link)
	{
		/*
		auto MakeItem = [&menu](int id)
		{
			return menu.AddItem(id, Localization::GetStandardString(id));
		};

		{
			MenuItem* item = MakeItem(wxID_UNDO);
			item->Enable(CanUndo());
		}
		{
			MenuItem* item = MakeItem(wxID_REDO);
			item->Enable(CanRedo());
		}
		menu.AddSeparator();

		{
			MenuItem* item = MakeItem(wxID_CUT);
			item->Enable(CanCut());
		}
		{
			MenuItem* item = MakeItem(wxID_COPY);
			item->Enable(CanCopy());
		}
		if (link != nullptr)
		{
			MenuItem* item = MakeItem(static_cast<wxWindowID>(StdID::CopyLink));
		}
		{
			MenuItem* item = MakeItem(wxID_PASTE);
			item->Enable(CanPaste());
		}
		{
			MenuItem* item = MakeItem(wxID_DELETE);
			item->Enable(IsEditable());
		}
		menu.AddSeparator();
		{
			MenuItem* item = MakeItem(wxID_SELECTALL);
			item->Enable(!IsEmpty());
		}
		*/
	}
	void HTMLWindow::ExecuteContextMenu(Menu& menu, const wxHtmlLinkInfo* link)
	{
		/*
		switch (menu.Show(this))
		{
			case wxID_COPY:
			{
				Copy();
				break;
			}
			case static_cast<wxWindowID>(StdID::CopyLink) :
			{
				CopyTextToClipboard(link->GetHref());
				break;
			}
			case wxID_SELECTALL:
			{
				SelectAll();
				break;
			}
		};
		*/
	}

	void HTMLWindow::OnContextMenu(wxContextMenuEvent& event)
	{
		/*
		Menu menu;
		CreateContextMenu(menu);
		ExecuteContextMenu(menu);
		*/
		event.Skip();
	}
	void HTMLWindow::OnKey(wxKeyEvent& event)
	{
		if (event.ControlDown())
		{
			switch (event.GetKeyCode())
			{
				case 'A':
				{
					SelectAll();
					break;
				}
				case 'C':
				{
					Copy();
					break;
				}
			};
		}
		event.Skip();
	}

	String HTMLWindow::OnProcessPlainText(const String& text) const
	{
		return ProcessPlainText(text);
	}
	void HTMLWindow::OnHTMLLinkClicked(const wxHtmlLinkInfo& link)
	{
		const wxMouseEvent* event = link.GetEvent();
		if (event && event->RightUp())
		{
			/*
			Menu menu;
			CreateContextMenu(menu, &link);
			ExecuteContextMenu(menu, &link);
			*/
		}
		else
		{
			wxHtmlWindow::OnHTMLLinkClicked(link);
		}
	}
	wxHtmlOpeningStatus HTMLWindow::OnHTMLOpeningURL(wxHtmlURLType type, const wxString& url, wxString* redirect) const
	{
		return wxHtmlWindow::OnHTMLOpeningURL(type, url, redirect);
	}

	bool HTMLWindow::DoSetFont(const GDIFont& normalFont)
	{
		int pointSize = normalFont.GetPointSize();
		String normalFace;
		String fixedFace;

		if (SetupFontsUsing(normalFont, normalFace, fixedFace, pointSize))
		{
			wxHtmlWindow::SetStandardFonts(pointSize, normalFace, fixedFace);
			return true;
		}
		return false;
	}
	bool HTMLWindow::DoSetValue(const String& value)
	{
		return wxHtmlWindow::SetPage(OnProcessPlainText(value));
	}
	bool HTMLWindow::DoAppendValue(const String& value)
	{
		return wxHtmlWindow::AppendToPage(OnProcessPlainText(value));
	}

	void HTMLWindow::OnEraseBackground(wxEraseEvent& event)
	{
		// Taken from 'wxHtmlWindow::DoEraseBackground'
		GDIContext dc(*event.GetDC());

		dc.SetBackgroundBrush(m_BackgroundColor);
		dc.Clear();

		if (m_BackgroundBitmap)
		{
			// Draw the background bitmap tiling it over the entire window area
			const Size virtualSize = Size(GetVirtualSize());
			const Size bitmapSize = m_BackgroundBitmap.GetSize();
			for (int x = 0; x < virtualSize.GetWidth(); x += bitmapSize.GetWidth())
			{
				for (int y = 0; y < virtualSize.GetWidth(); y += bitmapSize.GetWidth())
				{
					dc.DrawBitmap(m_BackgroundBitmap, {x, y});
				}
			}
		}
	}
	void HTMLWindow::OnPaint(wxPaintEvent& event)
	{
		// Taken from 'wxHtmlWindow::OnPaint'
		wxAutoBufferedPaintDC paintDC(this);
		if (IsFrozen() || !m_Cell)
		{
			return;
		}

		int x, y;
		GetViewStart(&x, &y);
		const Rect rect = Rect(GetUpdateRegion().GetBox());

		// Don't bother drawing the empty window.
		const Size clientSize = Size(GetClientSize());
		if (clientSize.GetWidth() == 0 || clientSize.GetHeight() == 0)
		{
			return;
		}

		auto Draw = [this, &rect, x, y](wxDC& dc)
		{
			wxEraseEvent eraseEvent(GetId(), &dc);
			OnEraseBackground(eraseEvent);

			// Draw the HTML window contents
			dc.SetMapMode(wxMM_TEXT);
			dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
			dc.SetLayoutDirection(GetLayoutDirection());

			wxHtmlRenderingInfo renderInfo;
			wxDefaultHtmlRenderingStyle renderStyle;
			renderInfo.SetSelection(m_selection);
			renderInfo.SetStyle(&renderStyle);
			m_Cell->Draw(dc, 0, 0, y * wxHTML_SCROLL_STEP + rect.GetTop(), y * wxHTML_SCROLL_STEP + rect.GetBottom(), renderInfo);
		};

		PrepareDC(paintDC);
		if (m_Renderer)
		{
			wxGCDC dc(m_Renderer->CreateContext(paintDC));
			Draw(dc);
		}
		else
		{
			Draw(paintDC);
		}
	}

	bool HTMLWindow::Create(wxWindow* parent,
							wxWindowID id,
							const String& text,
							FlagSet<HTMLWindowStyle> style
	)
	{
		if (wxHtmlWindow::Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt(), "HTMLWindow"))
		{
			m_BackgroundColor = wxHtmlWindow::GetBackgroundColour();
			SetBorders(2);
			DoSetFont(parent->GetFont());
			DoSetValue(text);

			SetBackgroundStyle(wxBG_STYLE_PAINT);
			Bind(wxEVT_PAINT, &HTMLWindow::OnPaint, this);
			Bind(wxEVT_ERASE_BACKGROUND, &HTMLWindow::OnEraseBackground, this);

			Bind(wxEVT_CONTEXT_MENU, &HTMLWindow::OnContextMenu, this);
			Bind(wxEVT_KEY_DOWN, &HTMLWindow::OnKey, this);
			return true;
		}
		return false;
	}
}
