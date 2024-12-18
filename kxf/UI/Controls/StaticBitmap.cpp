#include "kxf-pch.h"
#include "StaticBitmap.h"
#include "kxf/Drawing/BitmapImage.h"
#include "kxf/Drawing/GDIRenderer/UxTheme.h"
#include "kxf/Drawing/GDIRenderer/GDIWindowContext.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(StaticBitmap, wxGenericStaticBitmap);

	void StaticBitmap::OnPaint(wxPaintEvent& event)
	{
		using namespace kxf;

		GDIPaintContext dc(*this);
		UxTheme::DrawParentBackground(*this, dc, Rect(wxRect({0, 0}, GetSize())));
		event.Skip();
	}

	bool StaticBitmap::Create(wxWindow* parent,
							  wxWindowID id,
							  const GDIBitmap& bitmap,
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

	BitmapImage StaticBitmap::GetImage() const
	{
		return GetBitmap().ConvertToImage();
	}
	void StaticBitmap::SetImage(const BitmapImage& image)
	{
		SetBitmap(image.ToGDIBitmap().ToWxBitmap());
	}
}
