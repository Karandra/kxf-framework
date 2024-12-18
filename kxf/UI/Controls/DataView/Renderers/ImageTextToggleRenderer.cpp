#include "kxf-pch.h"
#include "ImageTextToggleRenderer.h"
#include "ImageRenderer.h"
#include "TextRenderer.h"
#include "ToggleRenderer.h"
#include "../View.h"

namespace kxf::UI::DataView
{
	bool ImageTextToggleValue::FromAny(Any& value)
	{
		return ImageTextValue::FromAny(value) || ToggleValue::FromAny(value) || std::move(value).GetAs(*this);
	}
}

namespace kxf::UI::DataView
{
	Any ImageTextToggleRenderer::OnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent)
	{
		ToggleState state = m_Value.GetToggleState();
		if (DoOnActivateCell(GetRenderEngine().GetToggleSize(), state, mouseEvent))
		{
			return state;
		}
		return {};
	}

	bool ImageTextToggleRenderer::SetDisplayValue(Any value)
	{
		m_Value = {};
		return m_Value.FromAny(value);
	}
	void ImageTextToggleRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		int offsetX = 0;
		int offsetFromToggle = 0;

		if (m_Value.HasToggleType())
		{
			IGraphicsContext& gc = GetGraphicsContext();
			Rect toggleRect(cellRect.GetPosition(), GetRenderEngine().GetToggleSize());
			toggleRect.SetHeight(cellRect.GetHeight());

			offsetX += GetRenderEngine().DrawToggle(gc, toggleRect, cellState, m_Value.GetToggleState(), m_Value.GetToggleType()).GetWidth();
			offsetFromToggle = GetRenderEngine().FromDIPX(2);
		}
		if (m_Value.HasImage() || m_Value.HasText() || m_Value.GetDefaultImagesSize().GetWidth() != Geometry::DefaultCoord)
		{
			Rect rect = cellRect;
			rect.X() += offsetFromToggle;
			rect.Width() -= offsetFromToggle;

			const int reservedWidth = m_Value.GetDefaultImagesSize().GetWidth();
			const bool centerTextV = m_Value.ContainsOption(ImageTextValueOption::VCenterText);
			GetRenderEngine().DrawBitmapWithText(rect, cellState, offsetX, m_Value.GetText(), m_Value.GetImage(), centerTextV, reservedWidth);
		}
	}
	Size ImageTextToggleRenderer::GetCellSize() const
	{
		RenderEngine renderEngine = GetRenderEngine();

		Size size;
		if (m_Value.HasToggleType())
		{
			Size toggleSize = renderEngine.GetToggleSize();
			size.Width() += toggleSize.GetWidth() + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.GetHeight() < toggleSize.GetHeight())
			{
				size.Height() = toggleSize.GetHeight();
			}
		}
		if (m_Value.HasText())
		{
			Size textExtent = renderEngine.GetTextExtent(m_Value.GetText());
			size.Width() += textExtent.GetWidth();
			if (size.GetHeight() < textExtent.GetHeight())
			{
				size.Height() = textExtent.GetHeight();
			}
		}
		if (const BitmapImage& image = m_Value.GetImage())
		{
			size.Width() += image.GetWidth() + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.GetHeight() < image.GetHeight())
			{
				size.Height() = image.GetHeight();
			}
		}
		else if (int width = m_Value.GetDefaultImagesSize().GetWidth(); width != Geometry::DefaultCoord)
		{
			size.Width() += width + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
		}
		return size;
	}
}
