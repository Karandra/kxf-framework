#include "kxf-pch.h"
#include "GDIRegion.h"
#include "GDIBitmap.h"
#include "Private/GDI.h"

namespace
{
	class wxRegionRefDataHack final: public wxGDIRefData
	{
		public:
			HRGN m_region = nullptr;

		public:
			void Clear()
			{
				m_region = nullptr;
			}
	};
}

namespace kxf
{
	// GDIRegion
	GDIRegion::GDIRegion(const GDIBitmap& bitmap)
		:m_Region(bitmap.AsWXBitmap())
	{
	}
	GDIRegion::GDIRegion(const GDIBitmap& bitmap, const Color& transparentColor, int tolerance)
		:m_Region(bitmap.AsWXBitmap(), transparentColor, tolerance)
	{
	}

	// IGDIObject
	void* GDIRegion::GetHandle() const
	{
		return m_Region.GetHRGN();
	}
	void* GDIRegion::DetachHandle()
	{
		if (wxRegionRefDataHack* refData = static_cast<wxRegionRefDataHack*>(m_Region.GetRefData()))
		{
			// 'GetResourceHandle' creates the actual pen object if it doesn't already exist
			void* handle = m_Region.GetHRGN();

			// Clear the internal structures
			refData->Clear();

			return handle;
		}
		return nullptr;
	}
	void GDIRegion::AttachHandle(void* handle)
	{
		m_Region = wxRegion(reinterpret_cast<WXHRGN>(handle));
	}

	GDIBitmap GDIRegion::ToBitmap() const
	{
		return m_Region.ConvertToBitmap();
	}
	bool GDIRegion::Union(const GDIBitmap& bitmap)
	{
		return m_Region.Union(bitmap.AsWXBitmap());
	}
	bool GDIRegion::Union(const GDIBitmap& bitmap, const Color& transparentColor, int tolerance)
	{
		return m_Region.Union(bitmap.AsWXBitmap(), transparentColor, tolerance);
	}
}
