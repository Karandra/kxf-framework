#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/control.h>
#include <wx/systhemectrl.h>

namespace kxf::UI
{
	class KXF_API ClusterMap: public WindowRefreshScheduler<wxSystemThemedControl<wxControl>>
	{
		private:
			struct DrawInfo
			{
				Size ClientSize;
				int Increment = 0;
				int ItemsX = 0;
				int ItemsY = 0;
			};

		private:
			wxEvtHandler m_EvtHandler;

			size_t m_ItemCount = 0;
			int m_UnderMouseIndex = -1;

			int m_ItemSize = 0;
			int m_Spacing = 0;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnMouse(wxMouseEvent& event);
			void OnMouseLeave(wxMouseEvent& event);
			void OnLeftUp(wxMouseEvent& event);

		protected:
			DrawInfo GetDrawInfo() const;
			wxSize DoGetBestSize() const override
			{
				return {320, 240};
			}
			wxSize GetMinSize() const override
			{
				return {240, 120};
			}

			Point CoordToXY(const DrawInfo& drawInfo, const Point& pos) const;
			int CoordToIndex(const DrawInfo& drawInfo, const Point& pos) const;

			Point IndexToXY(const DrawInfo& drawInfo, int index) const;
			int XYToIndex(const DrawInfo& drawInfo, const Point& xy) const;

			Rect XYToCoordRect(const DrawInfo& drawInfo, const Point& xy) const;
			Rect IndexToCoordRect(const DrawInfo& drawInfo, int index) const;

		public:
			ClusterMap() = default;
			ClusterMap(wxWindow* parent,
					   wxWindowID id,
					   const Point& pos = Point::UnspecifiedPosition(),
					   const Size& size = Size::UnspecifiedSize(),
					   long style = 0
			)
			{
				Create(parent, id, pos, size, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						long style = 0
			);
			~ClusterMap();

		public:
			int HitTest(const Point& pos) const
			{
				return CoordToIndex(GetDrawInfo(), pos);
			}
			Rect GetItemRect(int x, int y) const
			{
				return XYToCoordRect(GetDrawInfo(), {x, y});
			}
			Rect GetItemRect(int index) const
			{
				return IndexToCoordRect(GetDrawInfo(), index);
			}

			size_t GetItemCount() const
			{
				return m_ItemCount;
			}
			void SetItemCount(size_t count)
			{
				m_ItemCount = count;
				Refresh();
			}

			int GetItemSize() const
			{
				return m_ItemSize;
			}
			void SetItemSize(int size, int spacing = -1)
			{
				m_ItemSize = size;
				m_Spacing = std::clamp<int>(spacing > 0 ? spacing : 0.1 * size, 0, m_ItemSize);
				Refresh();
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(ClusterMap);
	};
}
