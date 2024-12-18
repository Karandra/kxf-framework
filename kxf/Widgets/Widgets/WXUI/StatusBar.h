#pragma once
#include "Common.h"
#include "../../IStatusBarWidget.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include <wx/statusbr.h>
#include <wx/systhemectrl.h>

namespace kxf::WXUI
{
	class KXF_API StatusBar: public EvtHandlerWrapper<StatusBar, UI::WindowRefreshScheduler<wxSystemThemedControl<wxStatusBar>>>
	{
		protected:
			IStatusBarWidget& m_Widget;

		public:
			StatusBar(IStatusBarWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			// wxStatusBar
			void SetMinHeight(int height) override;

			// StatusBar
			size_t GetPaneCount() const
			{
				return wxStatusBar::GetFieldsCount();
			}
			void SetPaneCount(size_t count)
			{
				wxStatusBar::SetFieldsCount(static_cast<int>(count));
			}

			Rect GetPaneRect(size_t index) const
			{
				wxRect rect;
				if (wxStatusBar::GetFieldRect(static_cast<int>(index), rect))
				{
					return Rect(rect);
				}
				return Rect::UnspecifiedRect();
			}
			int GetPaneWidth(size_t index) const
			{
				return wxStatusBar::GetStatusWidth(static_cast<int>(index));
			}
			void SetPaneWidth(size_t index, int width);

			String GetPaneLabel(size_t index) const
			{
				return StatusBar::GetStatusText(static_cast<int>(index));
			}
			void SetPaneLabel(size_t index, const String& label)
			{
				StatusBar::SetStatusText(label, index);
				ScheduleRefresh();
			}
	};
}
