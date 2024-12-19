#include "kxf-pch.h"
#include "GDIContext.h"
#include "kxf/wxWidgets/MapCore.h"
#include "kxf/wxWidgets/MapDrawing.h"
#include <wx/msw/dc.h>

namespace kxf
{
	// IGDIObject
	void* GDIContext::GetHandle() const
	{
		return m_DC ? m_DC->GetHDC() : nullptr;
	}
	void* GDIContext::DetachHandle()
	{
		if (m_DC)
		{
			if (auto impl = dynamic_cast<wxMSWDCImpl*>(m_DC->GetImpl()))
			{
				void* handle = impl->GetHDC();
				impl->SetHDC(nullptr, false);

				return handle;
			}
		}
		return nullptr;
	}
	void GDIContext::AttachHandle(void* handle)
	{
		if (m_DC)
		{
			if (auto impl = dynamic_cast<wxMSWDCImpl*>(m_DC->GetImpl()))
			{
				impl->SetHDC(static_cast<HDC>(handle), true);
				return;
			}
		}

		// Delete the handle if we can't attach it
		::DeleteDC(static_cast<HDC>(handle));
	}

	// Drawing functions
	Rect GDIContext::DrawLabel(const String& text, const Rect& rect, const GDIBitmap& bitmap, FlagSet<Alignment> alignment, size_t acceleratorIndex)
	{
		wxRect boundingBox;
		m_DC->DrawLabel(text, bitmap.ToWxBitmap(), rect, *wxWidgets::MapAlignment(alignment), acceleratorIndex != String::npos ? static_cast<int>(acceleratorIndex) : -1, &boundingBox);

		return Rect(boundingBox);
	}
	Rect GDIContext::DrawLabel(const String& text, const Rect& rect, FlagSet<Alignment> alignment, size_t acceleratorIndex)
	{
		wxRect boundingBox;
		m_DC->DrawLabel(text, wxNullBitmap, rect, *wxWidgets::MapAlignment(alignment), acceleratorIndex != String::npos ? static_cast<int>(acceleratorIndex) : -1, &boundingBox);

		return Rect(boundingBox);
	}

	void GDIContext::FloodFill(const Point& pos, const Color& color, FloodFillMode fillMode)
	{
		if (auto modeWx = wxWidgets::MapFloodFillMode(fillMode))
		{
			m_DC->FloodFill(pos, color, *modeWx);
		}
	}

	// Clipping region functions
	void GDIContext::ClipRegion(const GDIRegion& region)
	{
		m_DC->SetDeviceClippingRegion(region.ToWxRegion());
	}

	// Text/character extent functions
	kxf::FontMetrics GDIContext::GetFontMetrics() const
	{
		return wxWidgets::MapFontMetrics(m_DC->GetFontMetrics());
	}

	// Bounding box functions
	Rect GDIContext::GetBoundingBox() const
	{
		const int left = m_DC->MinX();
		const int top = m_DC->MinY();
		const int right = m_DC->MaxX();
		const int bottom = m_DC->MaxY();

		if (left == 0 && top == 0 && right == 0 && bottom == 0)
		{
			return {};
		}
		else
		{
			Rect rect;
			rect.SetLeftTop({left, top});
			rect.SetRightBottom({right, bottom});

			return rect;
		}
	}

	// Transformation matrix
	AffineMatrix GDIContext::GetTransformMatrix() const
	{
		return wxWidgets::MapAffineMatrix(m_DC->GetTransformMatrix());
	}
	bool GDIContext::SetTransformMatrix(const AffineMatrix& transform)
	{
		return m_DC->SetTransformMatrix(wxWidgets::MapAffineMatrix(transform));
	}
}
