#pragma once
#include "Common.h"
#include "../../IClusterMapWidget.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
#include <wx/control.h>
#include <wx/systhemectrl.h>

namespace kxf::WXUI
{
	class KXF_API ClusterMap: public EvtHandlerWrapper<ClusterMap, UI::WindowRefreshScheduler<wxSystemThemedControl<wxControl>>>
	{
		private:
			struct DrawInfo final
			{
				Size ClientSize;
				int Increment = 0;
				int ItemsX = 0;
				int ItemsY = 0;
			};

		private:
			IClusterMapWidget& m_Widget;
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;
			wxEvtHandler m_EvtHandler;

			size_t m_ItemCount = 0;
			size_t m_UnderMouseIndex = IClusterMapWidget::npos;

			int m_ItemSize = 0;
			int m_Spacing = 0;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnMouse(wxMouseEvent& event);
			void OnMouseLeave(wxMouseEvent& event);
			void OnLeftUp(wxMouseEvent& event);

		protected:
			// wxWidnow
			wxSize DoGetBestSize() const override
			{
				return FromDIP(wxSize(320, 240));
			}
			wxSize GetMinSize() const override
			{
				return FromDIP(wxSize(240, 120));
			}

			// ClusterMap
			DrawInfo GetDrawInfo() const;

			Point CoordToXY(const DrawInfo& drawInfo, const Point& pos) const;
			size_t CoordToIndex(const DrawInfo& drawInfo, const Point& pos) const;

			Point IndexToXY(const DrawInfo& drawInfo, size_t index) const;
			size_t XYToIndex(const DrawInfo& drawInfo, const Point& xy) const;

			Rect XYToCoordRect(const DrawInfo& drawInfo, const Point& xy) const;
			Rect IndexToCoordRect(const DrawInfo& drawInfo, size_t index) const;

		public:
			ClusterMap(IClusterMapWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}
			~ClusterMap()
			{
				if (m_EvtHandler.GetClientData() == this)
				{
					PopEventHandler();
				}
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			size_t HitTest(const Point& pos) const
			{
				return CoordToIndex(GetDrawInfo(), pos);
			}
			Rect GetItemRect(const Point& pos) const
			{
				return XYToCoordRect(GetDrawInfo(), pos);
			}
			Rect GetItemRect(size_t index) const
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

				ScheduleRefresh();
			}

			int GetItemSize() const
			{
				return m_ItemSize;
			}
			void SetItemSize(int size, int spacing = 0)
			{
				m_ItemSize = size;
				m_Spacing = std::clamp<int>(spacing > 0 ? spacing : 0.1 * size, 0, m_ItemSize);

				ScheduleRefresh();
			}
	};
}
