#include "kxf-pch.h"
#include "StyledTextBox.h"
#include <Scintilla\Scintilla.h>
#include <Scintilla\Sci_Position.h>

namespace
{
	constexpr int g_GlobalStyle = 0;
	constexpr int g_DefaultStyle = wxSTC_STYLE_DEFAULT;
}

namespace kxf::WXUI
{
	bool StyledTextBox::DoTryBefore(wxEvent& event)
	{
		return false;
	}

	bool StyledTextBox::Create(wxWindow* parent,
							   const String& text,
							   const Point& pos,
							   const Size& size
	)
	{
		if (wxStyledTextCtrl::Create(parent, wxID_NONE, pos, size, 0))
		{
			GDIFont font = parent->GetFont();
			font.SetFamily(FontFamily::FixedWidth);
			if (font.SetFaceName("Consolas") || font.SetFaceName("Courier New"))
			{
				StyleSetFont(g_GlobalStyle, font.ToWxFont());
				StyleSetFont(wxSTC_STYLE_LINENUMBER, font.ToWxFont());
			}

			SetDoubleBuffered(true);
			SetBufferedDraw(true);
			SetTabWidth(4);
			SetIndentationGuides(wxSTC_IV_REAL);
			SetEOLMode(wxSTC_EOL_CRLF);
			SetTechnology(wxSTC_TECHNOLOGY_DEFAULT);
			SetMarginType(wxSTC_MARGIN_NUMBER, wxSTC_MARGIN_NUMBER);
			SetMarginWidth(wxSTC_MARGIN_NUMBER, TextWidth(g_GlobalStyle, "99999"));
			SetWrapMode(wxSTC_WRAP_WORD);

			m_IsCreated = true;
			return true;
		}
		return false;
	}

	// wxWindow
	bool StyledTextBox::SetBackgroundColour(const wxColour& color)
	{
		if (m_IsCreated)
		{
			StyleSetBackground(g_GlobalStyle, color);
			StyleSetBackground(g_DefaultStyle, color);
			SetWhitespaceBackground(true, color);
		}
		return wxStyledTextCtrl::SetBackgroundColour(color);
	}
	bool StyledTextBox::SetForegroundColour(const wxColour& color)
	{
		if (m_IsCreated)
		{
			StyleSetForeground(g_GlobalStyle, color);
			StyleSetForeground(g_DefaultStyle, color);
			SetWhitespaceForeground(true, color);
		}
		return wxStyledTextCtrl::SetForegroundColour(color);
	}
}
