#include "kxf-pch.h"
#include "ImageTextRenderer.h"
#include "TextRenderer.h"
#include "ImageRenderer.h"

namespace kxf::UI::DataView
{
	bool ImageTextValue::FromAny(Any& value)
	{
		return TextValue::FromAny(value) || ImageValue::FromAny(value) || std::move(value).GetAs(*this);
	}
}

namespace kxf::UI::DataView
{
	bool ImageTextRenderer::SetDisplayValue(Any value)
	{
		m_Value = {};
		return m_Value.FromAny(value);
	}

	void ImageTextRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		const bool centerTextV = m_Value.ContainsOption(ImageTextValueOption::VCenterText);
		const int reservedWidth = m_Value.GetDefaultImagesSize().GetWidth();

		GetRenderEngine().DrawBitmapWithText(cellRect, cellState, 0, m_Value.GetText(), m_Value.GetImage(), centerTextV, reservedWidth);
	}
	Size ImageTextRenderer::GetCellSize() const
	{
		RenderEngine renderEngine = GetRenderEngine();

		Size size;
		if (m_Value.HasText())
		{
			size += renderEngine.GetTextExtent(m_Value.GetText());
		}

		if (const BitmapImage& image = m_Value.GetImage())
		{
			size.Width() += image.GetWidth() + GetRenderEngine().FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.GetHeight() < image.GetHeight())
			{
				size.Height() = image.GetHeight();
			}
		}
		return size;
	}
}
