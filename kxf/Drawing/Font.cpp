#include "kxf-pch.h"
#include "Font.h"
#include "GDIRenderer/GDIScreenContext.h"
#include <wx/font.h>
#include <wx/fontutil.h>
#include "Private/Common.h"

namespace
{
	using namespace kxf;

	constexpr auto g_FontPPI = 72.0f;

	float PointSizeFromPixelSize(const Size& pixelSize, int dpi = Geometry::DefaultCoord)
	{
		if (dpi == Geometry::DefaultCoord)
		{
			dpi = GDIScreenContext().GetDPI().GetHeight();
		}
		return (pixelSize.GetHeight() * g_FontPPI) / dpi;
	}
	int PixelSizeFromPointSize(float pointSize, int dpi = Geometry::DefaultCoord)
	{
		if (dpi == Geometry::DefaultCoord)
		{
			dpi = GDIScreenContext().GetDPI().GetHeight();
		}
		return (pointSize * dpi) / g_FontPPI;
	}
}

namespace kxf
{
	FontEncoding Font::GetDefaultEncoding() noexcept
	{
		return GDIFont::GetDefaultEncoding();
	}
	void Font::SetDefaultEncoding(FontEncoding encoding) noexcept
	{
		GDIFont::SetDefaultEncoding(encoding);
	}

	int Font::GetNumericWeightOf(FontWeight weight) noexcept
	{
		switch (weight)
		{
			case FontWeight::Thin:
			{
				return 100;
			}
			case FontWeight::ExtraLight:
			{
				return 200;
			}
			case FontWeight::Light:
			{
				return 300;
			}
			case FontWeight::Normal:
			{
				return 400;
			}
			case FontWeight::Medium:
			{
				return 500;
			}
			case FontWeight::SemiBold:
			{
				return 600;
			}
			case FontWeight::Bold:
			{
				return 700;
			}
			case FontWeight::ExtraBold:
			{
				return 800;
			}
			case FontWeight::Heavy:
			{
				return 900;
			}
			case FontWeight::ExtraHeavy:
			{
				return 1000;
			}
		};
		return -1;
	}
	FontWeight Font::GetSymbolicWeightOf(int weight) noexcept
	{
		switch (weight)
		{
			case 100:
			{
				return FontWeight::Thin;
			}
			case 200:
			{
				return FontWeight::ExtraLight;
			}
			case 300:
			{
				return FontWeight::Light;
			}
			case 400:
			{
				return FontWeight::Normal;
			}
			case 500:
			{
				return FontWeight::Medium;
			}
			case 600:
			{
				return FontWeight::SemiBold;
			}
			case 700:
			{
				return FontWeight::Bold;
			}
			case 800:
			{
				return FontWeight::ExtraBold;
			}
			case 900:
			{
				return FontWeight::Heavy;
			}
			case 1000:
			{
				return FontWeight::ExtraHeavy;
			}
		};
		return FontWeight::None;
	}

	void Font::CreateFrom(const GDIFont& other)
	{
		CreateFrom(other.ToWxFont());
	}
	void Font::CreateFrom(const wxFont& other)
	{
		if (other.IsOk())
		{
			m_FaceName = other.GetFaceName();
			m_PixelSize = Size(other.GetPixelSize());
			m_PointSize = static_cast<float>(other.GetFractionalPointSize());
			m_Weight = other.GetNumericWeight();
			m_Family = Drawing::Private::MapFontFamily(other.GetFamily());
			m_Encoding = Drawing::Private::MapFontEncoding(other.GetEncoding());

			m_Style = Drawing::Private::MapFontStyle(other.GetStyle());
			m_Style.Add(FontStyle::Underline, other.GetUnderlined());
			m_Style.Add(FontStyle::Strikethrough, other.GetStrikethrough());
		}
	}
	void Font::CreateFrom(const wxNativeFontInfo& other)
	{
		m_FaceName = other.GetFaceName();
		m_PixelSize = Size(other.GetPixelSize());
		m_PointSize = static_cast<float>(other.GetFractionalPointSize());
		m_Weight = other.GetNumericWeight();
		m_Family = Drawing::Private::MapFontFamily(other.GetFamily());
		m_Encoding = Drawing::Private::MapFontEncoding(other.GetEncoding());

		m_Style = Drawing::Private::MapFontStyle(other.GetStyle());
		m_Style.Add(FontStyle::Underline, other.GetUnderlined());
		m_Style.Add(FontStyle::Strikethrough, other.GetStrikethrough());
	}

	bool Font::IsNull() const noexcept
	{
		return m_FaceName.IsEmpty() || (m_PointSize <= 0 && !m_PixelSize.IsFullySpecified());
	}
	bool Font::IsInstalled() const
	{
		return !ToGDIFont().IsNull();
	}
	Font Font::GetBaseFont() const
	{
		Font baseFont = *this;
		baseFont.m_Style = {};
		baseFont.m_Weight = GetNumericWeightOf(FontWeight::Normal);

		return baseFont;
	}

	float Font::GetPointSize() const
	{
		return m_PointSize;
	}
	void Font::SetPointSize(float pointSize)
	{
		m_PointSize = std::abs(pointSize);
		m_PixelSize = {Geometry::DefaultCoord, PixelSizeFromPointSize(pointSize)};
	}
	void Font::ScaleSzie(float scale)
	{
		if (m_PointSize > 0)
		{
			SetPointSize(m_PointSize * scale);
		}
	}

	Size Font::GetPixelSize() const
	{
		return m_PixelSize;
	}
	void Font::SetPixelSize(const Size& pixelSize)
	{
		m_PixelSize = pixelSize;
		m_PointSize = PointSizeFromPixelSize(pixelSize);
	}

	GDIFont Font::ToGDIFont() const
	{
		return *this;
	}
	wxFont Font::ToWxFont() const
	{
		return GDIFont(*this).ToWxFont();
	}
}

namespace kxf
{
	uint64_t BinarySerializer<Font>::Serialize(IOutputStream& stream, const Font& value) const
	{
		return Serialization::WriteObject(stream, value.m_FaceName) +
			Serialization::WriteObject(stream, value.m_PixelSize) +
			Serialization::WriteObject(stream, value.m_PointSize) +
			Serialization::WriteObject(stream, value.m_Weight) +
			Serialization::WriteObject(stream, value.m_Family) +
			Serialization::WriteObject(stream, value.m_Encoding) +
			Serialization::WriteObject(stream, value.m_Style);
	}
	uint64_t BinarySerializer<Font>::Deserialize(IInputStream& stream, Font& value) const
	{
		return Serialization::ReadObject(stream, value.m_FaceName) +
			Serialization::ReadObject(stream, value.m_PixelSize) +
			Serialization::ReadObject(stream, value.m_PointSize) +
			Serialization::ReadObject(stream, value.m_Weight) +
			Serialization::ReadObject(stream, value.m_Family) +
			Serialization::ReadObject(stream, value.m_Encoding) +
			Serialization::ReadObject(stream, value.m_Style);
	}
}
