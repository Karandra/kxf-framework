#include "kxf-pch.h"
#include "ProgressRenderer.h"
#include "../View.h"
#include "../Node.h"
#include "../Column.h"

namespace kxf::UI::DataView
{
	bool ProgressMeterValueBase::FromAny(Any& value)
	{
		return value.GetAs(m_Position) || std::move(value).GetAs(*this);
	}

	bool ProgressMeterValue::FromAny(Any& value)
	{
		return TextValue::FromAny(value) || ProgressMeterValueBase::FromAny(value) || std::move(value).GetAs(*this);
	}
}

namespace kxf::UI::DataView
{
	bool ProgressMeterRenderer::SetDisplayValue(Any value)
	{
		m_Value = {};
		return m_Value.FromAny(value);
	}

	void ProgressMeterRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		RenderEngine renderEngine = GetRenderEngine();

		if (m_Value.HasPosition())
		{
			renderEngine.DrawProgressBar(GetBarRect(), cellState, m_Value.GetPosition(), m_Value.GetRange(), m_Value.GetState());
		}
		if (m_Value.HasText())
		{
			renderEngine.DrawText(cellRect, cellState, m_Value.GetText());
		}
	}
	Size ProgressMeterRenderer::GetCellSize() const
	{
		if (m_Value.HasText())
		{
			RenderEngine renderEngine = GetRenderEngine();
			Size size = renderEngine.GetTextExtent(m_Value.GetText());

			// If we need to draw a progress bar, then add a small margin to prevent clipping
			if (m_Value.HasPosition())
			{
				size += renderEngine.FromDIP(8, 0);
			}
			return size;
		}
		else
		{
			return Size(wxDefaultCoord, GetBarRect().GetHeight());
		}
	}
	Rect ProgressMeterRenderer::GetBarRect() const
	{
		RenderEngine renderEngine = GetRenderEngine();
		const Rect paintRect = GetPaintRect();

		Rect barRect = paintRect;
		auto SetHeight = [&](int desiredHeight, int margin = 0)
		{
			if (desiredHeight < barRect.GetHeight())
			{
				barRect.Inflate(0, barRect.GetHeight() - desiredHeight);
			}
			barRect.Deflate(0, margin);
		};

		const int margin = renderEngine.FromDIPY(4);
		switch (m_Value.GetHeight())
		{
			case ProgressMeterValue::Height::Auto:
			{
				SetHeight(GetView()->GetCharHeight() + renderEngine.FromDIPY(2), margin);
				break;
			}
			case ProgressMeterValue::Height::Fit:
			{
				barRect.Deflate(0, margin);
				break;
			}
			default:
			{
				SetHeight(m_Value.GetHeight<int>(), margin);
				break;
			}
		};

		if (barRect.GetHeight() >= paintRect.GetHeight())
		{
			barRect.Deflate(0, margin);
		}
		return barRect;
	}
}
