#pragma once
#include "Common.h"
#include "../../IImageViewWidget.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
#include <wx/control.h>
#include <wx/systhemectrl.h>

namespace kxf::WXUI
{
	class KXF_API ImageView: public EvtHandlerWrapper<ImageView, UI::WindowRefreshScheduler<wxSystemThemedControl<wxControl>>>
	{
		private:
			IImageViewWidget& m_Widget;
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;

			std::unique_ptr<IImage2D> m_Image;
			IImageViewWidget::BackgroundMode m_BackgroundMode = IImageViewWidget::BackgroundMode::Solid;
			IImageViewWidget::ScaleMode m_ScaleMode = IImageViewWidget::ScaleMode::None;
			Direction m_GradientDirection = Direction::Down;
			float m_ScaleFactor = 1.0f;

		private:
			void OnDrawBackground(wxEraseEvent& event);
			void OnDrawForeground(wxPaintEvent& event);
			void OnSize(wxSizeEvent& event);

		public:
			ImageView(IImageViewWidget& widget)
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
			IImageViewWidget::BackgroundMode GetBackgroundMode()
			{
				return m_BackgroundMode;
			}
			void SetBackgroundMode(IImageViewWidget::BackgroundMode mode)
			{
				m_BackgroundMode = mode;
				ScheduleRefresh();
			}

			Direction GetGradientDirection() const
			{
				return m_GradientDirection;
			}
			void SetGradientDirection(Direction direction)
			{
				m_GradientDirection = direction;
				ScheduleRefresh();
			}

			IImageViewWidget::ScaleMode GetScaleMode() const
			{
				return m_ScaleMode;
			}
			void SetScaleMode(IImageViewWidget::ScaleMode mode)
			{
				m_ScaleMode = mode;
				ScheduleRefresh();
			}

			float GetScaleFactor() const
			{
				return m_ScaleFactor;
			}
			void SetScaleFactor(float factor)
			{
				if (std::abs(m_ScaleFactor - factor) > 0.01f)
				{
					m_ScaleFactor = factor;
					ScheduleRefresh();
				}
			}

			void SetImage(const IImage2D& image)
			{
				ScheduleRefresh();
				m_Image = image ? image.CloneImage2D() : nullptr;
			}
	};
}
