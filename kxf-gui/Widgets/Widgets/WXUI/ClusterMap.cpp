#include "kxf-pch.h"
#include "ClusterMap.h"
#include "../../Events/WidgetItemEvent.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"

namespace kxf::WXUI
{
	void ClusterMap::OnPaint(wxPaintEvent& event)
	{
		auto renderer = m_RendererAware->GetActiveGraphicsRenderer();
		auto gc = renderer->CreateLegacyWindowPaintContext(*this);
		gc->Clear(renderer->GetTransparentBrush());

		const DrawInfo drawInfo = GetDrawInfo();
		if (m_ItemCount != 0 && m_ItemSize > 0)
		{
			constexpr float contrast = 0.7f;
			const Color color = GetForegroundColour();
			auto brush = renderer->CreateSolidBrush(color);
			auto pen = renderer->CreatePen(color.GetContrastColor(color.ChangeLightness(Angle::FromNormalized(contrast)), color.ChangeLightness(Angle::FromNormalized(-contrast))));

			Point pos;
			int blocksDrawn = 0;
			for (size_t i = 0; i < m_ItemCount; i++)
			{
				// Draw the block
				Rect blockRect(pos.GetX(), pos.GetY(), m_ItemSize, m_ItemSize);
				if (m_UnderMouseIndex != IClusterMapWidget::npos && i == m_UnderMouseIndex)
				{
					blockRect.Deflate(m_Widget.FromDIP<Size>(1, 1));
				}
				gc->DrawRectangle(blockRect, *brush, *pen);
				blocksDrawn++;

				// Move right
				pos.X() += drawInfo.Increment;

				// See if we need to move one row down
				if (blocksDrawn == drawInfo.ItemsY)
				{
					// Rewind to left side and move down
					pos.X() = 0;
					pos.X() += drawInfo.Increment;

					// Reset counter
					blocksDrawn = 0;
				}
			}
		}
	}
	void ClusterMap::OnMouse(wxMouseEvent& event)
	{
		size_t index = HitTest(Point(event.GetPosition()));
		if (index != m_UnderMouseIndex)
		{
			if (index != IClusterMapWidget::npos)
			{
				m_Widget.ProcessEvent(WidgetItemEvent::EvtEnter, m_Widget, index);
			}

			m_UnderMouseIndex = index;
			ScheduleRefresh();
		}
	}
	void ClusterMap::OnMouseLeave(wxMouseEvent& event)
	{
		if (m_UnderMouseIndex != IClusterMapWidget::npos)
		{
			m_Widget.ProcessEvent(WidgetItemEvent::EvtLeave, m_Widget, m_UnderMouseIndex);
		}

		m_UnderMouseIndex = IClusterMapWidget::npos;
		ScheduleRefresh();
	}
	void ClusterMap::OnLeftUp(wxMouseEvent& event)
	{
		if (m_UnderMouseIndex != IClusterMapWidget::npos)
		{
			m_Widget.ProcessEvent(WidgetItemEvent::EvtSelected, m_Widget, m_UnderMouseIndex);
		}
	}

	auto ClusterMap::GetDrawInfo() const -> DrawInfo
	{
		DrawInfo info;
		info.ClientSize = {};
		info.Increment = m_ItemSize + m_Spacing;

		if (info.Increment != 0)
		{
			info.ItemsY = info.ClientSize.GetWidth() / info.Increment;
			if (info.ItemsY != 0)
			{
				info.ItemsX = std::ceil(static_cast<double>(m_ItemCount) / info.ItemsY);

				return info;
			}
		}
		return {};
	}

	Point ClusterMap::CoordToXY(const DrawInfo& drawInfo, const Point& pos) const
	{
		// See if we are inside actual space used by blocks
		const Rect effectiveRect(0, 0, drawInfo.ItemsY * drawInfo.Increment, drawInfo.ItemsX * drawInfo.Increment);
		if (effectiveRect.Contains(pos))
		{
			return {pos.GetX() / drawInfo.Increment, pos.GetY() / drawInfo.Increment};
		}
		return Point::UnspecifiedPosition();
	}
	size_t ClusterMap::CoordToIndex(const DrawInfo& drawInfo, const Point& pos) const
	{
		Point xy = CoordToXY(drawInfo, pos);
		if (xy.IsFullySpecified())
		{
			int index = XYToIndex(drawInfo, xy);
			if (index != -1 && XYToCoordRect(drawInfo, xy).Contains(pos))
			{
				return index;
			}
		}
		return IClusterMapWidget::npos;
	}

	Point ClusterMap::IndexToXY(const DrawInfo& drawInfo, size_t index) const
	{
		if (index < m_ItemCount && drawInfo.ItemsY > 0)
		{
			const int y = index / drawInfo.ItemsY;
			const int x = index - (y * static_cast<size_t>(drawInfo.ItemsY)) - 1;

			return {x, y};
		}
		return Point::UnspecifiedPosition();
	}
	size_t ClusterMap::XYToIndex(const DrawInfo& drawInfo, const Point& xy) const
	{
		if (xy.IsFullySpecified())
		{
			size_t index = IClusterMapWidget::npos;
			if (xy.GetY() == 0)
			{
				index = xy.GetX();
			}
			else
			{
				index = (static_cast<size_t>(xy.GetY()) * drawInfo.ItemsY) + xy.GetX();
			}

			if (index >= 0 && index < m_ItemCount)
			{
				return index;
			}
		}
		return IClusterMapWidget::npos;
	}

	Rect ClusterMap::XYToCoordRect(const DrawInfo& drawInfo, const Point& xy) const
	{
		return {xy.GetX() * drawInfo.Increment, xy.GetY() * drawInfo.Increment, m_ItemSize, m_ItemSize};
	}
	Rect ClusterMap::IndexToCoordRect(const DrawInfo& drawInfo, size_t index) const
	{
		Point xy = IndexToXY(drawInfo, index);
		if (xy.IsFullySpecified())
		{
			return Rect(xy.GetX() * drawInfo.Increment, xy.GetY() * drawInfo.Increment, m_ItemSize, m_ItemSize);
		}
		return {};
	}

	bool ClusterMap::Create(wxWindow* parent,
							const String& label,
							const Point& pos,
							const Size& size
	)
	{
		if (wxSystemThemedControl::Create(parent, wxID_NONE, pos, size, wxFULL_REPAINT_ON_RESIZE))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);
			SetBackgroundStyle(wxBG_STYLE_PAINT);

			m_EvtHandler.Bind(wxEVT_PAINT, &ClusterMap::OnPaint, this);
			m_EvtHandler.Bind(wxEVT_MOTION, &ClusterMap::OnMouse, this);
			m_EvtHandler.Bind(wxEVT_ENTER_WINDOW, &ClusterMap::OnMouse, this);
			m_EvtHandler.Bind(wxEVT_LEAVE_WINDOW, &ClusterMap::OnMouseLeave, this);
			m_EvtHandler.Bind(wxEVT_LEFT_UP, &ClusterMap::OnLeftUp, this);

			PushEventHandler(&m_EvtHandler);
			m_EvtHandler.SetClientData(this);

			return m_Widget.QueryInterface(m_RendererAware);
		}
		return false;
	}
}
