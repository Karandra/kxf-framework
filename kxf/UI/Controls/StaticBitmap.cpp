#include "stdafx.h"
#include "StaticBitmap.h"
#include "kxf/Drawing/UxTheme.h"
#include "kxf/Drawing/Bitmap.h"
#include "kxf/Drawing/Image.h"
#include "kxf/Drawing/GDIWindowContext.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(StaticBitmap, wxGenericStaticBitmap);

	void StaticBitmap::OnPaint(wxPaintEvent& event)
	{
		using namespace kxf;

		GDIPaintContext dc(*this);
		UxTheme::DrawParentBackground(*this, dc, Rect(Point(0, 0), GetSize()));
		event.Skip();
	}

	bool StaticBitmap::Create(wxWindow* parent,
							  wxWindowID id,
							  const Bitmap& bitmap,
							  FlagSet<WindowStyle> style
	)
	{
		SetBackgroundStyle(wxBG_STYLE_PAINT);
		if (wxGenericStaticBitmap::Create(parent, id, bitmap.ToWxBitmap(), Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt()))
		{
			m_InitialSize = bitmap ? bitmap.GetSize() : Size(this->GetSize());

			SetScaleMode(wxGenericStaticBitmap::Scale_None);
			Bind(wxEVT_PAINT, &StaticBitmap::OnPaint, this);

			return true;
		}
		return false;
	}

	Image StaticBitmap::GetImage() const
	{
		return GetBitmap().ConvertToImage();
	}
	void StaticBitmap::SetImage(const Image& image)
	{
		SetBitmap(Image(image).ToBitmap().ToWxBitmap());
	}
}
