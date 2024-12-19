#include "kxf-pch.h"
#include "DrawablePanel.h"
#include "kxf-gui/Drawing/GDIRenderer/UxTheme.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIWindowContext.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIMemoryContext.h"
#include <wx/bookctrl.h>

namespace
{
	const kxf::GDIBitmap g_EmptyBitmap({8, 8}, kxf::ColorDepthDB::BPP32);
}

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(DrawablePanel, Panel);

	Size DrawablePanel::DrawScaledBitmap(wxGraphicsContext* gc, const wxGraphicsBitmap& bitmap, const Size& bitmapSize, const Rect& rect, BitmapScaleMode scaleMode, double globalScale)
	{
		Size scaledImageSize;
		double x = 0;
		double y = 0;
		double width = 0;
		double height = 0;

		switch (scaleMode)
		{
			case BitmapScaleMode::None:
			{
				scaledImageSize = bitmapSize;
				gc->DrawBitmap(!bitmap.IsNull() ? bitmap : gc->CreateBitmap(g_EmptyBitmap.AsWXBitmap()), rect.GetX(), rect.GetY(), bitmapSize.GetWidth(), bitmapSize.GetWidth());
				return scaledImageSize;
			}
			case BitmapScaleMode::Fill:
			{
				width = rect.GetWidth();
				height = rect.GetHeight();
				break;
			}
			case BitmapScaleMode::AspectFit:
			case BitmapScaleMode::AspectFill:
			{
				double scaleFactor;
				double scaleX = (double)rect.GetWidth() / (double)bitmapSize.GetWidth();
				double scaleY = (double)rect.GetHeight() / (double)bitmapSize.GetHeight();
				if ((scaleMode == BitmapScaleMode::AspectFit && scaleY < scaleX) || (scaleMode == BitmapScaleMode::AspectFill && scaleY > scaleX))
				{
					scaleFactor = scaleY;
				}
				else
				{
					scaleFactor = scaleX;
				}
				scaleFactor *= globalScale;

				width = bitmapSize.GetWidth() * scaleFactor;
				height = bitmapSize.GetHeight() * scaleFactor;
				break;
			}
		};

		x = (rect.GetWidth() - width) / 2;
		y = (rect.GetHeight() - height) / 2;

		scaledImageSize.Width() = width;
		scaledImageSize.Height() = height;
		gc->DrawBitmap(!bitmap.IsNull() ? bitmap : gc->CreateBitmap(g_EmptyBitmap.AsWXBitmap()), rect.GetX() + x, rect.GetY() + y, width, height);
		return scaledImageSize;
	}
	Size DrawablePanel::DrawScaledBitmap(wxGraphicsContext* gc, const GDIBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale)
	{
		return DrawScaledBitmap(gc, gc->CreateBitmap(bitmap.AsWXBitmap()), bitmap.GetSize(), rect, scaleMode, globalScale);
	}
	Size DrawablePanel::DrawScaledBitmap(GDIWindowContext& dc, const GDIBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale)
	{
		wxGCDC gcdc(dc.AsWXDC());
		return DrawScaledBitmap(gcdc.GetGraphicsContext(), bitmap, rect, scaleMode, globalScale);
	}
	Size DrawablePanel::DrawScaledBitmap(GDIMemoryContext& dc, const GDIBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale)
	{
		wxGCDC gcdc(dc.AsWXDC());
		return DrawScaledBitmap(gcdc.GetGraphicsContext(), bitmap, rect, scaleMode, globalScale);
	}
	void DrawablePanel::DrawTransparencyPattern(GDIContext& dc)
	{
		const wxWindow* window = dc.GetWindow();

		const int sideSize = window->FromDIP(8);
		dc.SetBackgroundBrush(window->GetBackgroundColour());
		dc.SetBrush(window->GetForegroundColour());
		dc.SetPen(window->GetForegroundColour());
		dc.Clear();

		size_t indexY = 0;
		for (int y = 0; y <= dc.GetHeight(); y += sideSize)
		{
			for (int x = indexY % 2 == 0 ? 0 : sideSize; x <= dc.GetWidth(); x += 2 * sideSize)
			{
				dc.DrawRectangle({x, y, sideSize, sideSize});
			}
			indexY++;
		}
	}

	void DrawablePanel::OnDrawBackground(wxEraseEvent& event)
	{
		GDIContext dc(*event.GetDC());
		dc.SetBackgroundTransparent();
		dc.SetBackgroundBrush(Drawing::GetStockGDIBrush(StockBrush::Transparent));

		auto mode = GetBGMode();
		if (mode & DrawablePanelMode::Soild)
		{
			dc.SetBackgroundBrush(GetBackgroundColour());
			dc.Clear();
		}
		else if (mode & DrawablePanelMode::Gradient)
		{
			dc.DrawGradientLinear(dc.GetSize(), GetForegroundColour(), GetBackgroundColour(), m_GradientDirection);
		}
		else if (mode & DrawablePanelMode::TransparencyPattern)
		{
			DrawTransparencyPattern(dc);
		}
		else
		{
			wxWindow* window = this;
			if (GetParent()->IsKindOf(wxCLASSINFO(wxBookCtrlBase)))
			{
				window = GetParent();
			}
			UxTheme::DrawParentBackground(*window, dc, Rect({0, 0}, Size(GetSize())));
		}

		if (mode & DrawablePanelMode::BGImage)
		{
			m_ScaledImageSize = m_Bitmap.GetSize();
			dc.DrawBitmap(m_Bitmap, {0, 0});
		}
	}
	void DrawablePanel::OnDrawForeground(wxPaintEvent& event)
	{
		GDIPaintContext dc(*this);
		if (m_BackgroundMode & DrawablePanelMode::FGImage)
		{
			wxGCDC gcdc(dc.AsWXDC());
			m_ScaledImageSize = DrawScaledBitmap(gcdc.GetGraphicsContext(), m_Bitmap, Rect({0, 0}, Size(GetClientSize())), m_ImageScaleMode, m_ScaleFactor);
		}
		else
		{
			event.Skip();
		}
	}

	bool DrawablePanel::Create(wxWindow* parent,
							   wxWindowID id,
							   FlagSet<WindowStyle> style
	)
	{
		if (Panel::Create(parent, id, style|WindowStyle::FullRepaintOnResize))
		{
			SetBackgroundStyle(wxBG_STYLE_ERASE);
			Bind(wxEVT_ERASE_BACKGROUND, &DrawablePanel::OnDrawBackground, this);
			Bind(wxEVT_PAINT, &DrawablePanel::OnDrawForeground, this);

			return true;
		}
		return false;
	}
}
