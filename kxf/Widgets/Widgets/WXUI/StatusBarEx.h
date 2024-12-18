#pragma once
#include "Common.h"
#include "../../IStatusBarWidget.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include "kxf/Drawing/IRendererNative.h"
#include "StatusBar.h"
#include <wx/toplevel.h>

namespace kxf::WXUI
{
	class KXF_API StatusBarEx: public StatusBar
	{
		private:
			std::vector<std::unique_ptr<IImage2D>> m_Icons;
			Color m_BorderColor;
			NativeWidgetFlag m_WidgetState = NativeWidgetFlag::None;
			bool m_IsSeparatorsVisible = false;

			int m_ProgressRange = 100;
			int m_ProgressStep = 10;
			int m_ProgressPos = 0;
			
		protected:
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnSize(wxSizeEvent& event);
			void OnEnter(wxMouseEvent& event);
			void OnLeave(wxMouseEvent& event);
			
			EllipsizeMode GetEllipsizeMode() const;
			wxTopLevelWindow* GetTLWParent() const;

			void UseDefaultColors();
			void UseBackgroundColors();

		public:
			StatusBarEx(IStatusBarWidget& widget)
				:StatusBar(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			// wxWindow
			bool SetForegroundColour(const wxColour& color) override;
			bool SetBackgroundColour(const wxColour& color) override;

			// StatusBar
			void SetPaneCount(size_t count)
			{
				m_Icons.resize(count);
				StatusBar::SetPaneCount(count);
			}

			// StatusBarEx
			Color GetBorderColor() const
			{
				return m_BorderColor;
			}
			void SetBorderColor(const Color& color)
			{
				m_BorderColor = color;
				ScheduleRefresh();
			}
		
			bool IsSeparatorsVisible() const
			{
				return m_IsSeparatorsVisible;
			}
			void SetSeparatorsVisible(bool visible = true)
			{
				m_IsSeparatorsVisible = visible;
				ScheduleRefresh();
			}
		
			void SetPaneIcon(size_t index, const IImage2D& icon)
			{
				if (index < m_Icons.size())
				{
					m_Icons[index] = icon ? icon.CloneImage2D() : nullptr;
					ScheduleRefresh();
				}
			}

		public:
			int GetRange() const
			{
				return m_ProgressRange;
			}
			void SetRange(int range)
			{
				m_ProgressRange = range;
				ScheduleRefresh();
			}

			int GetValue() const
			{
				return m_ProgressPos;
			}
			void SetValue(int value)
			{
				m_ProgressPos = value;
				ScheduleRefresh();
			}

			int GetStep() const
			{
				return m_ProgressStep;
			}
			void SetStep(int step)
			{
				m_ProgressStep = step <= m_ProgressRange ? step : m_ProgressRange;
			}
	};
}
