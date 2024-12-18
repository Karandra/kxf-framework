#include "kxf-pch.h"
#include "TextRenderer.h"
#include "kxf/Drawing/GraphicsRenderer.h"

namespace kxf::DataView
{
	void TextRenderer::DrawContent(const RenderInfo& renderInfo)
	{
		if (!m_Value.IsEmpty())
		{
			CellRendererHelper renderHelper(renderInfo);
			renderHelper.DrawText(renderInfo.CellRect, m_Value);
		}
	}
	Size TextRenderer::GetCellSize(const RenderInfo& renderInfo) const
	{
		if (!m_Value.IsEmpty())
		{
			CellRendererHelper renderHelper(renderInfo);
			return renderHelper.GetTextExtent(m_Value);
		}
		return {};
	}

	String TextRenderer::GetDisplayText(const Any& value) const
	{
		return value.GetAs<String>();
	}
	bool TextRenderer::SetDisplayValue(Any value)
	{
		return std::move(value).GetAs<String>(m_Value);
	}
}
