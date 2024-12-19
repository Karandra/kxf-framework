#include "kxf-pch.h"
#include "ClusterMap.h"
#include "kxf-gui/Drawing/GDIRenderer/UxTheme.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIWindowContext.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ClusterMap, wxControl);

	void ClusterMap::OnPaint(wxPaintEvent& event)
	{
		using namespace kxf;

		GDIPaintContext dc(*this);

		const DrawInfo drawInfo = GetDrawInfo();
		UxTheme::DrawParentBackground(*this, dc, Rect({0, 0}, drawInfo.ClientSize));

		if (m_ItemCount != 0 && m_ItemSize > 0)
		{
			constexpr float contrast = 0.7f;
			const Color color = GetForegroundColour();
			dc.SetBrush(color);
			dc.SetPen(color.GetContrastColor(color.ChangeLightness(Angle::FromNormalized(contrast)), color.ChangeLightness(Angle::FromNormalized(-contrast))));

			Point pos;
			int blocksDrawn = 0;
			for (size_t i = 0; i < m_ItemCount; i++)
			{
				// Draw the block
				Rect blockRect(pos.GetX(), pos.GetY(), m_ItemSize, m_ItemSize);
				if (m_UnderMouseIndex != -1 && i == m_UnderMouseIndex)
				{
					blockRect.Deflate(Size(FromDIP(wxSize(1, 1))));
				}
				dc.DrawRectangle(blockRect);
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
		int index = HitTest(Point(event.GetPosition()));
		if (index != m_UnderMouseIndex)
		{
			m_UnderMouseIndex = index;
			Refresh();

			if (index != -1)
			{
				wxCommandEvent evt(wxEVT_TOOL_ENTER, GetId());
				evt.SetEventObject(this);
				evt.SetInt(m_UnderMouseIndex);

				ProcessWindowEvent(evt);
			}
		}
	}
	void ClusterMap::OnMouseLeave(wxMouseEvent& event)
	{
		m_UnderMouseIndex = -1;
		Refresh();
	}
	void ClusterMap::OnLeftUp(wxMouseEvent& event)
	{
		if (m_UnderMouseIndex != -1 && event.LeftUp())
		{
			wxCommandEvent evt(wxEVT_BUTTON, GetId());
			evt.SetEventObject(this);
			evt.SetInt(m_UnderMouseIndex);

			ProcessWindowEvent(evt);
		}
	}

	auto ClusterMap::GetDrawInfo() const -> DrawInfo
	{
		DrawInfo info;
		info.ClientSize = Size();
		info.Increment = m_ItemSize + m_Spacing;
		if (info.Increment != 0)
		{
			info.ItemsY = info.ClientSize.GetWidth() / info.Increment;
			if (info.ItemsY != 0)
			{
				info.ItemsX = std::ceil(static_cast<float>(m_ItemCount) / info.ItemsY);

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
	int ClusterMap::CoordToIndex(const DrawInfo& drawInfo, const Point& pos) const
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
		return -1;
	}

	Point ClusterMap::IndexToXY(const DrawInfo& drawInfo, int index) const
	{
		if (index >= 0 && static_cast<size_t>(index) < m_ItemCount && drawInfo.ItemsY > 0)
		{
			const int y = index / drawInfo.ItemsY;
			const int x = index - (y * drawInfo.ItemsY) - 1;

			return {x, y};
		}
		return Point::UnspecifiedPosition();
	}
	int ClusterMap::XYToIndex(const DrawInfo& drawInfo, const Point& xy) const
	{
		if (xy.IsFullySpecified())
		{
			int index = -1;
			if (xy.GetY() == 0)
			{
				index = xy.GetX();
			}
			else
			{
				index = (xy.GetY() * drawInfo.ItemsY) + xy.GetX();
			}

			if (index >= 0 && static_cast<size_t>(index) < m_ItemCount)
			{
				return index;
			}
		}
		return -1;
	}

	Rect ClusterMap::XYToCoordRect(const DrawInfo& drawInfo, const Point& xy) const
	{
		return {xy.GetX() * drawInfo.Increment, xy.GetY() * drawInfo.Increment, m_ItemSize, m_ItemSize};
	}
	Rect ClusterMap::IndexToCoordRect(const DrawInfo& drawInfo, int index) const
	{
		Point xy = IndexToXY(drawInfo, index);
		if (xy.IsFullySpecified())
		{
			return Rect(xy.GetX() * drawInfo.Increment, xy.GetY() * drawInfo.Increment, m_ItemSize, m_ItemSize);
		}
		return {};
	}

	bool ClusterMap::Create(wxWindow* parent,
							wxWindowID id,
							const Point& pos,
							const Size& size,
							long style
	)
	{
		PushEventHandler(&m_EvtHandler);
		if (wxSystemThemedControl::Create(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);
			SetBackgroundStyle(wxBG_STYLE_PAINT);

			m_EvtHandler.Bind(wxEVT_PAINT, &ClusterMap::OnPaint, this);
			m_EvtHandler.Bind(wxEVT_MOTION, &ClusterMap::OnMouse, this);
			m_EvtHandler.Bind(wxEVT_ENTER_WINDOW, &ClusterMap::OnMouse, this);
			m_EvtHandler.Bind(wxEVT_LEAVE_WINDOW, &ClusterMap::OnMouseLeave, this);
			m_EvtHandler.Bind(wxEVT_LEFT_UP, &ClusterMap::OnLeftUp, this);
			return true;
		}
		return false;
	}
	ClusterMap::~ClusterMap()
	{
		PopEventHandler();
	}
}
