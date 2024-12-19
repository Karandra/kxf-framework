#include "kxf-pch.h"
#include "GDIFont.h"
#include "../Font.h"
#include "kxf/wxWidgets/MapDrawing.h"
#include <wx/fontutil.h>

namespace
{
	class wxFontRefDataHack final: public wxGDIRefData
	{
		public:
			bool m_sizeUsingPixels = false;
			HFONT m_hFont = nullptr;
			wxNativeFontInfo m_nativeFontInfo;
	};
}

namespace kxf
{
	FontEncoding GDIFont::GetDefaultEncoding() noexcept
	{
		return wxWidgets::MapFontEncoding(wxFont::GetDefaultEncoding());
	}
	void GDIFont::SetDefaultEncoding(FontEncoding encoding) noexcept
	{
		wxFont::SetDefaultEncoding(wxWidgets::MapFontEncoding(encoding));
	}
	int GDIFont::GetNumericWeightOf(FontWeight weight) noexcept
	{
		return wxFont::GetNumericWeightOf(wxWidgets::MapFontWeight(weight));
	}

	GDIFont::GDIFont(const Font& other)
		:m_Font(other.GetPointSize(),
				wxWidgets::MapFontFamily(other.GetFamily()),
				wxWidgets::MapFontStyle(other.GetStyle()),
				wxWidgets::MapFontWeight(other.GetWeight()),
				other.GetStyle().Contains(FontStyle::Underline),
				other.GetFaceName(),
				wxWidgets::MapFontEncoding(other.GetEncoding())
		)
	{
		m_Font.SetFractionalPointSize(other.GetPointSize());
		m_Font.SetStrikethrough(other.GetStyle().Contains(FontStyle::Strikethrough));
	}

	// IGDIObject
	void* GDIFont::GetHandle() const
	{
		return m_Font.GetResourceHandle();
	}
	void* GDIFont::DetachHandle()
	{
		if (wxFontRefDataHack* refData = static_cast<wxFontRefDataHack*>(m_Font.GetRefData()))
		{
			// 'GetResourceHandle' creates the actual font object if it doesn't already exist
			void* handle = m_Font.GetResourceHandle();

			// Clear the internal structures
			refData->m_hFont = nullptr;
			refData->m_nativeFontInfo.lf = {};
			refData->m_sizeUsingPixels = false;

			return handle;
		}
		return nullptr;
	}
	void GDIFont::AttachHandle(void* handle)
	{
		m_Font = wxFont();

		if (handle)
		{
			m_Font.SetFamily(wxFONTFAMILY_UNKNOWN);
			if (wxFontRefDataHack* refData = static_cast<wxFontRefDataHack*>(m_Font.GetRefData()))
			{
				LOGFONTW fontInfo = {};
				if (::GetObjectW(handle, sizeof(fontInfo), &fontInfo) != 0)
				{
					refData->m_hFont = static_cast<HFONT>(handle);
					refData->m_nativeFontInfo.lf = fontInfo;
					refData->m_sizeUsingPixels = true;
					return;
				}
			}

			// Delete the handle if we can't attach it
			::DeleteObject(handle);
		}
	}
}
