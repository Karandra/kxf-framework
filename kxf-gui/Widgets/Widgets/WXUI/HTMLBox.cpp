#include "kxf-pch.h"
#include "HTMLBox.h"
#include "../../Menus/MenuWidget.h"
#include "../../Events/MenuWidgetEvent.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Drawing/GDIRenderer/GDIContext.h"
#include "kxf/Localization/Common.h"
#include <wx/clipbrd.h>

namespace kxf::WXUI
{
	String HTMLBox::ProcessPlainText(const String& text)
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
	bool HTMLBox::SetupFontsUsing(const GDIFont& normalFont, String& normalFace, String& fixedFace, int& pointSize)
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

	void HTMLBox::CopyTextToClipboard(const String& value) const
	{
		if (wxTheClipboard->Open())
		{
			wxTheClipboard->SetData(new wxTextDataObject(value));
			wxTheClipboard->Close();
		}
	}
	void HTMLBox::CreateContextMenu(IMenuWidget& menu, const wxHtmlLinkInfo* link)
	{
		auto MakeItem = [&menu](StdID id)
		{
			return menu.InsertItem(Localization::GetStandardString(id), id);
		};

		{
			auto item = MakeItem(StdID::Undo);
			item->SetEnabled(CanUndo());
		}
		{
			auto item = MakeItem(StdID::Redo);
			item->SetEnabled(CanRedo());
		}
		menu.InsertSeparator();

		{
			auto item = MakeItem(StdID::Cut);
			item->SetEnabled(CanCut());
		}
		{
			auto item = MakeItem(StdID::Copy);
			item->SetEnabled(CanCopy());
			item->Bind(MenuWidgetEvent::EvtSelected, [&](IEvent& event)
			{
				Copy();
			});
		}
		if (link)
		{
			auto item = MakeItem(StdID::CopyLink);
			item->Bind(MenuWidgetEvent::EvtSelected, [&, link](IEvent& event)
			{
				CopyTextToClipboard(link->GetHref());
			});
		}
		{
			auto item = MakeItem(StdID::Paste);
			item->SetEnabled(CanPaste());
		}
		{
			auto item = MakeItem(StdID::Delete);
			item->SetEnabled(IsEditable());
		}
		menu.InsertSeparator();
		{
			auto item = MakeItem(StdID::SelectAll);
			item->SetEnabled(!IsEmpty());
			item->Bind(MenuWidgetEvent::EvtSelected, [&](IEvent& event)
			{
				SelectAll();
			});
		}
	}

	void HTMLBox::OnPaint(wxPaintEvent& event)
	{
		// Taken from 'wxHtmlWindow::OnPaint'
		auto renderer = m_RendererAware->GetActiveGraphicsRenderer();
		auto gc = renderer->CreateLegacyWindowPaintContext(*this);

		if (IsFrozen() || !m_Cell)
		{
			return;
		}
		auto brush = renderer->CreateSolidBrush(m_BackgroundColor);
		gc->Clear(*brush);

		const Point pos = Point(GetViewStart());
		const Rect rect = Rect(GetUpdateRegion().GetBox());

		// Don't bother drawing the empty window.
		const Size clientSize = Size(GetClientSize());
		if (clientSize.GetWidth() == 0 || clientSize.GetHeight() == 0)
		{
			return;
		}

		gc->DrawGDI(rect, [&](GDIContext& dc)
		{
			dc.SetMapMode(GDIMappingMode::Text);
			dc.SetLayoutDirection(m_Widget.GetLayoutDirection());

			wxHtmlRenderingInfo renderInfo;
			wxDefaultHtmlRenderingStyle renderStyle;
			renderInfo.SetSelection(m_selection);
			renderInfo.SetStyle(&renderStyle);

			m_Cell->Draw(dc.ToWxDC(), 0, 0, pos.GetY() * wxHTML_SCROLL_STEP + rect.GetTop(), pos.GetY() * wxHTML_SCROLL_STEP + rect.GetBottom(), renderInfo);
		});
	}
	void HTMLBox::OnEraseBackground(wxEraseEvent& event)
	{
		// Taken from 'wxHtmlWindow::DoEraseBackground'
		auto renderer = m_RendererAware->GetActiveGraphicsRenderer();
		auto virtualRect = m_Widget.GetRect(WidgetSizeFlag::Virtual);
		auto gc = renderer->CreateLegacyContext(*event.GetDC(), virtualRect.GetSize());

		auto brush = renderer->CreateSolidBrush(m_BackgroundColor);
		gc->Clear(*brush);

		if (m_BackgroundImage)
		{
			gc->DrawTexture(*m_BackgroundImage, virtualRect);
		}
	}
	void HTMLBox::OnContextMenu(wxContextMenuEvent& event)
	{
		if (auto menu = NewWidget<Widgets::MenuWidget>(m_Widget.QueryInterface<IWidget>()))
		{
			CreateContextMenu(*menu);
			menu->Show(Point(event.GetPosition()));
		}
		event.Skip();
	}
	void HTMLBox::OnKey(wxKeyEvent& event)
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

	String HTMLBox::OnProcessPlainText(const String& text) const
	{
		return ProcessPlainText(text);
	}
	void HTMLBox::OnHTMLLinkClicked(const wxHtmlLinkInfo& link)
	{
		const wxMouseEvent* event = link.GetEvent();
		if (event && event->RightUp())
		{
			/*
			if (auto menu = NewWidget<Widgets::MenuWidget>(m_Widget.QueryInterface<IWidget>()))
			{
				CreateContextMenu(*menu);
				menu->Show(Point(event.GetPosition()));
			}
			*/
		}
		else
		{
			wxHtmlWindow::OnHTMLLinkClicked(link);
		}
	}
	wxHtmlOpeningStatus HTMLBox::OnHTMLOpeningURL(wxHtmlURLType type, const wxString& url, wxString* redirect) const
	{
		return wxHtmlWindow::OnHTMLOpeningURL(type, url, redirect);
	}

	bool HTMLBox::DoSetFont(const GDIFont& normalFont)
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
	bool HTMLBox::DoSetValue(const String& value)
	{
		return wxHtmlWindow::SetPage(OnProcessPlainText(value));
	}
	bool HTMLBox::DoAppendValue(const String& value)
	{
		return wxHtmlWindow::AppendToPage(OnProcessPlainText(value));
	}

	bool HTMLBox::Create(wxWindow* parent,
						 const String& text,
						 const Point& pos,
						 const Size& size
	)
	{
		if (wxHtmlWindow::Create(parent, wxID_NONE, pos, size, wxBORDER_THEME|wxHW_SCROLLBAR_AUTO, "HTMLBox"))
		{
			m_BackgroundColor = wxHtmlWindow::GetBackgroundColour();
			SetBorders(2);
			DoSetFont(parent->GetFont());
			DoSetValue(text);

			SetBackgroundStyle(wxBG_STYLE_PAINT);
			m_EvtHandler.Bind(wxEVT_PAINT, &HTMLBox::OnPaint, this);
			m_EvtHandler.Bind(wxEVT_ERASE_BACKGROUND, &HTMLBox::OnEraseBackground, this);
			m_EvtHandler.Bind(wxEVT_CONTEXT_MENU, &HTMLBox::OnContextMenu, this);
			m_EvtHandler.Bind(wxEVT_KEY_DOWN, &HTMLBox::OnKey, this);

			PushEventHandler(&m_EvtHandler);
			m_EvtHandler.SetClientData(this);

			return m_Widget.QueryInterface(m_RendererAware);
		}
		return false;
	}
}
