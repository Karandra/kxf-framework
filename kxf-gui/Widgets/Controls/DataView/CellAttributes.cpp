#include "kxf-pch.h"
#include "CellAttributes.h"

namespace kxf::UI::DataView
{
	Font CellAttribute::GetEffectiveFont(const Font& baseFont) const
	{
		if (!m_FontOptions.IsDefault())
		{
			Font font = baseFont;
			if (m_FontOptions.ContainsOption(CellFontOption::Bold))
			{
				font.SetWeight(FontWeight::Bold);
			}
			if (m_FontOptions.ContainsOption(CellFontOption::Italic))
			{
				font.AddStyle(FontStyle::Italic);
			}
			if (m_FontOptions.ContainsOption(CellFontOption::Underlined))
			{
				font.AddStyle(FontStyle::Underline);
			}
			if (m_FontOptions.ContainsOption(CellFontOption::Strikethrough))
			{
				font.AddStyle(FontStyle::Strikethrough);
			}
			if (m_FontOptions.HasFontFace())
			{
				font.SetFaceName(m_FontOptions.GetFontFace());
			}
			if (m_FontOptions.HasFontSize())
			{
				font.SetPointSize(m_FontOptions.GetFontSize());
			}
			return font;
		}
		return baseFont;
	}
}
