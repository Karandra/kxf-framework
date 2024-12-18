#include "kxf-pch.h"
#include "ImageListRenderer.h"
#include "../View.h"
#include "../Node.h"
#include "../Column.h"
#include "kxf/System/SystemInformation.h"

namespace
{
	int CalcSpacing(wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(value, 0)).GetWidth();
	}
	kxf::Size GetSmallIconSize(const kxf::UI::DataView::ImageValueBase& bitmapValue)
	{
		using namespace kxf;

		auto size = System::GetMetric(SystemSizeMetric::IconSmall);
		if (int width = bitmapValue.GetDefaultImagesSize().GetWidth(); width != Geometry::DefaultCoord)
		{
			size.SetWidth(width);
		}
		return size;
	}
}

namespace kxf::UI::DataView
{
	bool ImageListValue::FromAny(Any& value)
	{
		return TextValue::FromAny(value) || std::move(value).GetAs(m_Images) || std::move(value).GetAs(*this);
	}
}

namespace kxf::UI::DataView
{
	void ImageListRendererBase::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		RenderEngine renderEngine = GetRenderEngine();

		int offsetX = 0;
		if (size_t bitmapCount = GetImageCount(); bitmapCount != 0)
		{
			const Size smallIcon = GetSmallIconSize(m_BitmapValueBase);
			const int spacing = CalcSpacing(GetView(), m_BitmapValueBase.GetImageSpacing());

			for (size_t i = 0; i < bitmapCount; i++)
			{
				BitmapImage bitmap = GetImage(i);
				if (bitmap || !m_BitmapValueBase.ShouldDrawInvalidImages())
				{
					const Size bitmapSize = bitmap ? Size(bitmap.GetSize()) : smallIcon;

					Rect bitmapRect = cellRect;
					bitmapRect.SetX(cellRect.GetX() + offsetX);
					bitmapRect.SetWidth(bitmapSize.GetWidth());
					bitmapRect.SetHeight(std::clamp(bitmapSize.GetHeight(), 0, cellRect.GetHeight()));

					// Don't draw images with invalid indexes, but count them as drawn to allow spaces.
					if (bitmap)
					{
						renderEngine.DrawBitmap(bitmapRect, cellState, bitmap);
					}

					// Move right
					offsetX += bitmapSize.GetWidth() + spacing;
				}
			}
		}
		if (m_TextValue.HasText())
		{
			offsetX += renderEngine.GetInterTextSpacing();
			renderEngine.DrawText(cellRect, cellState, m_TextValue.GetText(), offsetX);
		}
	}
	Size ImageListRendererBase::GetCellSize() const
	{
		Size totalSize;
		if (m_TextValue.HasText())
		{
			RenderEngine renderEngine = GetRenderEngine();

			totalSize = renderEngine.GetTextExtent(m_TextValue.GetText());
			totalSize.Width() += renderEngine.GetInterTextSpacing();
		}
		if (size_t bitmapCount = GetImageCount(); bitmapCount != 0)
		{
			const Size smallIcon = GetSmallIconSize(m_BitmapValueBase);
			const int spacing = CalcSpacing(GetView(), m_BitmapValueBase.GetImageSpacing());

			for (size_t i = 0; i < bitmapCount; i++)
			{
				BitmapImage bitmap = GetImage(i);
				if (bitmap || !m_BitmapValueBase.ShouldDrawInvalidImages())
				{
					const Size bitmapSize = bitmap ? bitmap.GetSize() : smallIcon;

					totalSize.Width() += bitmapSize.GetWidth() + spacing;
					totalSize.Height() = std::max(totalSize.GetHeight(), bitmapSize.GetHeight());
				}
			}
		}
		return totalSize;
	}
}

namespace kxf::UI::DataView
{
	bool ImageListRenderer::SetDisplayValue(Any value)
	{
		m_Value = {};
		return m_Value.FromAny(value);
	}
}
