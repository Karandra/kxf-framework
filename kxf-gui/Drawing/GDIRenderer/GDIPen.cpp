#include "kxf-pch.h"
#include "GDIPen.h"
#include "GDIBitmap.h"
#include "Private/GDI.h"
#include "kxf/Drawing/GDIRenderer/Private.h"
#include "kxf/wxWidgets/MapDrawing.h"

namespace
{
	class wxPenRefDataHack final: public wxGDIRefData
	{
		public:
			int m_width = 0;
			wxPenStyle m_style = wxPENSTYLE_INVALID;
			wxPenJoin m_join = wxJOIN_INVALID;
			wxPenCap m_cap = wxCAP_INVALID;
			wxBitmap m_stipple;
			int m_nbDash = 0;
			wxDash* m_dash = nullptr;
			wxColour m_colour;
			HPEN m_hPen = nullptr;

		public:
			void Clear()
			{
				m_width = 0;
				m_style = wxPENSTYLE_INVALID;
				m_join = wxJOIN_INVALID;
				m_cap = wxCAP_INVALID;
				m_stipple = wxNullBitmap;
				m_nbDash = 0;
				m_dash = nullptr;
				m_colour = wxNullColour;
				m_hPen = nullptr;
			}
	};
}

namespace kxf
{
	// GDIPen
	void GDIPen::Initialize()
	{
		m_Pen.SetCap(wxCAP_BUTT);
		m_Pen.SetJoin(wxJOIN_MITER);
	}

	GDIPen::GDIPen(const GDIBitmap& stippleBitmap, int width)
		:m_Pen(stippleBitmap.AsWXBitmap(), width)
	{
		Initialize();
	}

	// IGDIObject
	void* GDIPen::GetHandle() const
	{
		return m_Pen.GetResourceHandle();
	}
	void* GDIPen::DetachHandle()
	{
		if (wxPenRefDataHack* refData = static_cast<wxPenRefDataHack*>(m_Pen.GetRefData()))
		{
			// 'GetResourceHandle' creates the actual pen object if it doesn't already exist
			void* handle = m_Pen.GetResourceHandle();

			// Clear the internal structures
			refData->Clear();

			return handle;
		}
		return nullptr;
	}
	void GDIPen::AttachHandle(void* handle)
	{
		m_Pen = wxPen();

		if (handle)
		{
			m_Pen.SetColour({});
			if (wxPenRefDataHack* refData = static_cast<wxPenRefDataHack*>(m_Pen.GetRefData()))
			{
				refData->Clear();
				refData->m_hPen = static_cast<HPEN>(handle);

				// Test for transparent stock pen
				if (::GetStockObject(NULL_PEN) == handle)
				{
					refData->m_style = wxPENSTYLE_TRANSPARENT;
					return;
				}

				// Extended pen config
				if (EXTLOGPEN penInfoEx = {}; ::GetObjectW(handle, sizeof(penInfoEx), &penInfoEx) != 0)
				{
					// Convert width
					refData->m_width = penInfoEx.elpWidth;

					// Convert join type if the pen is geometric
					if ((penInfoEx.elpPenStyle & PS_TYPE_MASK) == PS_GEOMETRIC)
					{
						refData->m_join = wxWidgets::MapNativePenJoin(penInfoEx.elpPenStyle & PS_JOIN_MASK);
					}

					// Convert pen style
					refData->m_style = wxWidgets::MapNativePenStyle(penInfoEx.elpPenStyle & PS_STYLE_MASK);

					// Convert brush style
					switch (penInfoEx.elpBrushStyle)
					{
						case BS_DIBPATTERN:
						case BS_DIBPATTERN8X8:
						case BS_DIBPATTERNPT:
						{
							if (penInfoEx.elpBrushStyle == BS_DIBPATTERN || penInfoEx.elpBrushStyle == BS_DIBPATTERN8X8)
							{
								HGLOBAL memoryHandle = reinterpret_cast<HGLOBAL>(penInfoEx.elpHatch);
								if (const void* data = ::GlobalLock(memoryHandle))
								{
									refData->m_stipple = Drawing::BitmapFromMemoryLocation(data).AsWXBitmap();
									::GlobalUnlock(memoryHandle);
								}
							}
							else if (penInfoEx.elpBrushStyle == BS_DIBPATTERNPT)
							{
								const void* data = reinterpret_cast<const void*>(penInfoEx.elpHatch);
								refData->m_stipple = Drawing::BitmapFromMemoryLocation(data).AsWXBitmap();
							}
							break;
						}
						case BS_PATTERN:
						case BS_PATTERN8X8:
						{
							GDIBitmap bitmap;
							bitmap.AttachHandle(reinterpret_cast<HBITMAP>(penInfoEx.elpHatch));

							refData->m_stipple = bitmap.AsWXBitmap();
							refData->m_style = wxPENSTYLE_STIPPLE;

							break;
						}
						case BS_HATCHED:
						case BS_SOLID:
						{
							refData->m_colour = Color::FromCOLORREF(penInfoEx.elpColor);
							if (penInfoEx.elpBrushStyle == BS_HATCHED)
							{
								refData->m_style = static_cast<wxPenStyle>(wxWidgets::MapNativeHatchStyle(static_cast<int>(penInfoEx.elpHatch)));
							}
							else
							{
								refData->m_style = wxPENSTYLE_SOLID;
							}
							break;
						}
						case BS_NULL:
						{
							refData->m_style = wxPENSTYLE_TRANSPARENT;
							break;
						}
					};

					// We can't convert dashes because we don't store them ourselves
					// so ignore 'penInfoEx.elpStyleEntry' and 'penInfoEx.elpNumEntries'
					// fields for now.
				}

				// Regular pen config
				if (LOGPEN penInfo = {}; ::GetObjectW(handle, sizeof(penInfo), &penInfo) != 0)
				{
					refData->m_style = wxWidgets::MapNativePenStyle(penInfo.lopnStyle);
					refData->m_width = penInfo.lopnWidth.x;
					refData->m_colour = Color::FromCOLORREF(penInfo.lopnColor);
				}
			}

			// Delete the handle if we can't attach it
			::DeleteObject(handle);
		}
	}

	// GDIPen
	GDIBitmap GDIPen::GetStipple() const
	{
		if (m_Pen.GetStyle() == wxPENSTYLE_STIPPLE)
		{
			const wxBitmap* stipple = m_Pen.GetStipple();
			if (stipple && stipple->IsOk())
			{
				return *stipple;
			}
		}
		return {};
	}
	void GDIPen::SetStipple(const GDIBitmap& stipple)
	{
		m_Pen.SetStipple(stipple.AsWXBitmap());
		m_Pen.SetStyle(wxPENSTYLE_STIPPLE);
	}
}

namespace kxf::Drawing
{
	GDIPen GetStockGDIPen(StockPen pen)
	{
		switch (pen)
		{
			case StockPen::Black:
			{
				return *wxBLACK_PEN;
			}
			case StockPen::BlackDashed:
			{
				return *wxBLACK_DASHED_PEN;
			}
			case StockPen::Cyan:
			{
				return *wxCYAN_PEN;
			}
			case StockPen::Blue:
			{
				return *wxBLUE_PEN;
			}
			case StockPen::Red:
			{
				return *wxRED_PEN;
			}
			case StockPen::Green:
			{
				return *wxGREEN_PEN;
			}
			case StockPen::Yellow:
			{
				return *wxYELLOW_PEN;
			}
			case StockPen::Gray:
			{
				return *wxGREY_PEN;
			}
			case StockPen::LightGray:
			{
				return *wxLIGHT_GREY_PEN;
			}
			case StockPen::MediumGray:
			{
				return *wxMEDIUM_GREY_PEN;
			}
			case StockPen::White:
			{
				return *wxWHITE_PEN;
			}
			case StockPen::Transparent:
			{
				return *wxTRANSPARENT_PEN;
			}
		};
		return {};
	}
}
