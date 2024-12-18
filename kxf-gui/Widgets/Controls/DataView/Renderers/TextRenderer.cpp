#include "kxf-pch.h"
#include "TextRenderer.h"

namespace kxf::UI::DataView
{
	bool TextValue::FromAny(Any& value)
	{
		return std::move(value).GetAs(m_Text) || std::move(value).GetAs(*this);
	}
}

namespace kxf::UI::DataView
{
	bool TextRenderer::SetDisplayValue(Any value)
	{
		m_Value = {};
		return m_Value.FromAny(value);
	}

	void TextRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		if (m_Value.HasText())
		{
			GetRenderEngine().DrawText(cellRect, cellState, m_Value.GetText());
		}
	}
	Size TextRenderer::GetCellSize() const
	{
		if (m_Value.HasText())
		{
			return GetRenderEngine().GetTextExtent(m_Value.GetText());
		}
		return {};
	}
}
