#include "kxf-pch.h"
#include "GDIBrush.h"
#include "Private/GDI.h"
#include "../Private/Common.h"

namespace
{
	class wxBrushRefDataHack final: public wxGDIRefData
	{
		public:
			wxBrushStyle m_style = wxBRUSHSTYLE_INVALID;
			wxBitmap m_stipple;
			wxColour m_colour;
			HBRUSH m_hBrush = nullptr;

		public:
			void Clear()
			{
				m_hBrush = nullptr;
				m_stipple = wxNullBitmap;
				m_colour = wxNullColour;
				m_style = wxBRUSHSTYLE_INVALID;
			}
	};
}

namespace kxf
{
	// IGDIObject
	void* GDIBrush::GetHandle() const
	{
		return m_Brush.GetResourceHandle();
	}
	void* GDIBrush::DetachHandle()
	{
		if (wxBrushRefDataHack* refData = static_cast<wxBrushRefDataHack*>(m_Brush.GetRefData()))
		{
			// 'GetResourceHandle' creates the actual brush object if it doesn't already exist
			void* handle = m_Brush.GetResourceHandle();

			// Clear the internal structures
			refData->Clear();

			return handle;
		}
		return nullptr;
	}
	void GDIBrush::AttachHandle(void* handle)
	{
		m_Brush = wxBrush();

		if (handle)
		{
			m_Brush.SetColour({});
			if (wxBrushRefDataHack* refData = static_cast<wxBrushRefDataHack*>(m_Brush.GetRefData()))
			{
				refData->Clear();
				refData->m_hBrush = static_cast<HBRUSH>(handle);

				LOGBRUSH brushInfo = {};
				if (::GetObjectW(handle, sizeof(brushInfo), &brushInfo) != 0)
				{
					if (::GetStockObject(NULL_BRUSH) == handle)
					{
						refData->m_style = wxBRUSHSTYLE_TRANSPARENT;
						return;
					}

					switch (brushInfo.lbStyle)
					{
						case BS_DIBPATTERN:
						case BS_DIBPATTERN8X8:
						case BS_DIBPATTERNPT:
						{
							// DIB_PAL_COLORS
							// DIB_RGB_COLORS
							const int colorType = LOWORD(brushInfo.lbColor);
							if (brushInfo.lbStyle == BS_DIBPATTERN || brushInfo.lbStyle == BS_DIBPATTERN8X8)
							{
								HGLOBAL memoryHandle = reinterpret_cast<HGLOBAL>(brushInfo.lbHatch);
								if (const void* data = ::GlobalLock(memoryHandle))
								{
									refData->m_stipple = Drawing::Private::BitmapFromMemoryLocation(data).ToWxBitmap();
									::GlobalUnlock(memoryHandle);
								}
							}
							else if (brushInfo.lbStyle == BS_DIBPATTERNPT)
							{
								const void* data = reinterpret_cast<const void*>(brushInfo.lbHatch);
								refData->m_stipple = Drawing::Private::BitmapFromMemoryLocation(data).ToWxBitmap();
							}
							break;
						}
						case BS_PATTERN:
						case BS_PATTERN8X8:
						{
							GDIBitmap bitmap;
							bitmap.AttachHandle(reinterpret_cast<HBITMAP>(brushInfo.lbHatch));

							refData->m_stipple = bitmap.ToWxBitmap();
							refData->m_style = wxBRUSHSTYLE_STIPPLE;

							break;
						}
						case BS_HATCHED:
						case BS_SOLID:
						{
							refData->m_colour = Color::FromCOLORREF(brushInfo.lbColor);
							if (brushInfo.lbStyle == BS_HATCHED)
							{
								refData->m_style = static_cast<wxBrushStyle>(Drawing::Private::MapNativeHatchStyle(static_cast<int>(brushInfo.lbHatch)));
							}
							else
							{
								refData->m_style = wxBRUSHSTYLE_SOLID;
							}
							break;
						}
						case BS_NULL:
						{
							refData->m_style = wxBRUSHSTYLE_TRANSPARENT;
							break;
						}
					};
					return;
				}
			}

			// Delete the handle if we can't attach it
			::DeleteObject(handle);
		}
	}
}

namespace kxf::Drawing
{
	GDIBrush GetStockGDIBrush(StockBrush brush)
	{
		switch (brush)
		{
			case StockBrush::Black:
			{
				return *wxBLACK_BRUSH;
			}
			case StockBrush::Cyan:
			{
				return *wxCYAN_BRUSH;
			}
			case StockBrush::Blue:
			{
				return *wxBLUE_BRUSH;
			}
			case StockBrush::Red:
			{
				return *wxRED_BRUSH;
			}
			case StockBrush::Green:
			{
				return *wxGREEN_BRUSH;
			}
			case StockBrush::Yellow:
			{
				return *wxYELLOW_BRUSH;
			}
			case StockBrush::Gray:
			{
				return *wxGREY_BRUSH;
			}
			case StockBrush::LightGray:
			{
				return *wxLIGHT_GREY_BRUSH;
			}
			case StockBrush::MediumGray:
			{
				return *wxMEDIUM_GREY_BRUSH;
			}
			case StockBrush::White:
			{
				return *wxWHITE_BRUSH;
			}
			case StockBrush::Transparent:
			{
				return *wxTRANSPARENT_BRUSH;
			}
		};
		return {};
	}
}
