#pragma once
#include "kxf-gui/Widgets/Common.h"
#include "Panel.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
#include "kxf-gui/Widgets/Controls/StaticBitmap.h"
#include <wx/graphics.h>

namespace kxf::UI
{
	enum class DrawablePanelMode: uint32_t
	{
		None = 0,

		Soild = 1 << 0,
		Gradient = 1 << 1,
		TransparencyPattern = 1 << 2,
		BGImage = 1 << 3,
		FGImage = 1 << 4,
	};
}
namespace kxf
{
	class GDIContext;
	class GDIMemoryContext;
	class GDIWindowContext;

	kxf_FlagSet_Declare(UI::DrawablePanelMode);
}

namespace kxf::UI
{
	class KXF_API DrawablePanel: public WindowRefreshScheduler<Panel>
	{
		public:
			static Size DrawScaledBitmap(wxGraphicsContext* gc, const wxGraphicsBitmap& bitmap, const Size& bitmapSize, const Rect& rect, BitmapScaleMode scaleMode, double globalScale = 1.0);
			static Size DrawScaledBitmap(wxGraphicsContext* gc, const GDIBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale = 1.0);
			static Size DrawScaledBitmap(GDIWindowContext& dc, const GDIBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale = 1.0);
			static Size DrawScaledBitmap(GDIMemoryContext& dc, const GDIBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale = 1.0);
			static void DrawTransparencyPattern(GDIContext& dc);

		private:
			GDIBitmap m_Bitmap;
			BitmapScaleMode m_ImageScaleMode = BitmapScaleMode::None;
			FlagSet<DrawablePanelMode> m_BackgroundMode = DrawablePanelMode::Soild;
			Direction m_GradientDirection = Direction::Down;
			Size m_ScaledImageSize;
			double m_ScaleFactor = 1.0;

		private:
			void OnDrawBackground(wxEraseEvent& event);
			void OnDrawForeground(wxPaintEvent& event);

		public:
			DrawablePanel() = default;
			DrawablePanel(wxWindow* parent,
						  wxWindowID id,
						  FlagSet<WindowStyle> style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						FlagSet<WindowStyle> style = DefaultStyle
			);
			bool Create(wxWindow* parent,
						wxWindowID id,
						const Point& pos,
						const Size& size,
						FlagSet<WindowStyle> style = DefaultStyle,
						const String& name = {}
			)
			{
				return Create(parent, id, style);
			}

		public:
			bool HasBitmap() const
			{
				return !m_Bitmap.IsNull();
			}
			const GDIBitmap& GetBitmap() const
			{
				return m_Bitmap;
			}
			void SetBitmap(const GDIBitmap& image)
			{
				m_Bitmap = image;
				ScheduleRefresh();
			}

			BitmapScaleMode GetScaleMode() const
			{
				return m_ImageScaleMode;
			}
			void SetScaleMode(BitmapScaleMode mode)
			{
				m_ImageScaleMode = mode;
				ScheduleRefresh();
			}

			FlagSet<DrawablePanelMode> GetBGMode() const
			{
				return m_BackgroundMode;
			}
			void SetBGMode(FlagSet<DrawablePanelMode> mode)
			{
				m_BackgroundMode = mode;
				ScheduleRefresh();
			}

			Direction GetGradientDirection() const
			{
				return m_GradientDirection;
			}
			void SetGradientDirection(Direction mode)
			{
				m_GradientDirection = mode;
				ScheduleRefresh();
			}

			Size GetScaledImageSize() const
			{
				return m_ScaledImageSize;
			}
			double GetScaleFactor() const
			{
				return m_ScaleFactor;
			}
			void SetScaleFactor(double factor)
			{
				m_ScaleFactor = factor;
				ScheduleRefresh();
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(DrawablePanel);
	};

	using ScrolledDrawablePanel = wxScrolled<DrawablePanel>;
}
