#include "kxf-pch.h"
#include "Color.h"
#include "kxf/Core/RegEx.h"
#include "kxf/Core/String.h"
#include "kxf/Utility/Common.h"
#include <wx/colour.h>
#include <wx/brush.h>
#include <wx/pen.h>

namespace
{
	kxf::String CSS2RGB(const kxf::Color& color, kxf::C2SAlpha alpha)
	{
		using namespace kxf;

		switch (alpha)
		{
			case C2SAlpha::Never:
			{
				const auto fixed = color.GetFixed8();
				return Format("rgb({}, {}, {})", fixed.Red, fixed.Green, fixed.Blue);
			}
			case C2SAlpha::Always:
			{
				const auto fixed = color.GetFixed8();
				const auto normalized = color.GetNormalized();
				return Format("rgba({}, {}, {}, {})", fixed.Red, fixed.Green, fixed.Blue, normalized.Alpha);
			}
			case C2SAlpha::Auto:
			{
				return color.IsOpaque() ? CSS2RGB(color, C2SAlpha::Never) : CSS2RGB(color, C2SAlpha::Always);
			}
		};
		return {};
	}
	kxf::String CSS2HSL(const kxf::Color& color, kxf::C2SAlpha alpha)
	{
		using namespace kxf;

		switch (alpha)
		{
			case C2SAlpha::Never:
			{
				const auto hsl = color.GetHSL();
				return Format("hsl({}, {}, {})", hsl.Hue.ToDegrees(), hsl.Saturation, hsl.Lightness);
			}
			case C2SAlpha::Always:
			{
				const auto hsl = color.GetHSL();
				const auto normalized = color.GetNormalized();
				return Format("hsla({}, {}, {}, {})", hsl.Hue.ToDegrees(), hsl.Saturation, hsl.Lightness, normalized.Alpha);
			}
			case C2SAlpha::Auto:
			{
				return color.IsOpaque() ? CSS2HSL(color, C2SAlpha::Never) : CSS2HSL(color, C2SAlpha::Always);
			}
		};
		return {};
	}

	kxf::String HTML2RGB(const kxf::Color& color, kxf::C2SAlpha alpha)
	{
		using namespace kxf;

		switch (alpha)
		{
			case C2SAlpha::Never:
			{
				const auto fixed = color.GetFixed8();
				return Format("#{:0>2x}{:0>2x}{:0>2x}", fixed.Red, fixed.Green, fixed.Blue);
			}
			case C2SAlpha::Always:
			{
				const auto fixed = color.GetFixed8();
				return Format("#{:0>2x}{:0>2x}{:0>2x}{:0>2x}", fixed.Red, fixed.Green, fixed.Blue, fixed.Alpha);
			}
			case C2SAlpha::Auto:
			{
				return color.IsOpaque() ? HTML2RGB(color, C2SAlpha::Never) : HTML2RGB(color, C2SAlpha::Always);
			}
		};
		return {};
	}
	kxf::String HTML2HSL(const kxf::Color& color, kxf::C2SAlpha alpha)
	{
		using namespace kxf;

		switch (alpha)
		{
			case C2SAlpha::Never:
			{
				const auto hsl = color.GetHSL();
				return Format("hsl({}, {}%, {}%)", static_cast<int>(hsl.Hue.ToDegrees()), static_cast<int>(hsl.Saturation * 100), static_cast<int>(hsl.Lightness * 100));
			}
			case C2SAlpha::Always:
			{
				const auto hsl = color.GetHSL();
				return Format("hsl({}, {}%, {}%, {})", static_cast<int>(hsl.Hue.ToDegrees()), static_cast<int>(hsl.Saturation * 100), static_cast<int>(hsl.Lightness * 100), hsl.Alpha);
			}
			case C2SAlpha::Auto:
			{
				return color.IsOpaque() ? HTML2HSL(color, C2SAlpha::Never) : HTML2HSL(color, C2SAlpha::Always);
			}
		};
		return {};
	}
}

namespace kxf
{
	Color Color::FromString(const String& value, ColorSpace* colorSpace)
	{
		if (!value.IsEmpty())
		{
			if (value.front() == '#')
			{
				auto r = value.SubMid(1, 2).ToInteger<uint8_t>(16);
				auto g = value.SubMid(3, 2).ToInteger<uint8_t>(16);
				auto b = value.SubMid(5, 2).ToInteger<uint8_t>(16);
				if (r && g && b)
				{
					Utility::SetIfNotNull(colorSpace, ColorSpace::RGB);

					auto a = value.SubMid(7, 2).ToInteger<uint8_t>(16);
					return FromFixed8(*r, *g, *b, a.value_or(255));
				}
			}
			else if (RegEx regEx(wxS(R"(rgba?\((\d+),\s+(\d+),\s+(\d+),?\s*([\d\.]*)\))")); regEx.Matches(value))
			{
				auto r = regEx.GetMatch(value, 1).ToInteger<uint8_t>();
				auto g = regEx.GetMatch(value, 2).ToInteger<uint8_t>();
				auto b = regEx.GetMatch(value, 3).ToInteger<uint8_t>();

				if (r && g && b)
				{
					Utility::SetIfNotNull(colorSpace, ColorSpace::RGB);

					auto a = regEx.GetMatch(value, 4).ToFloatingPoint<float>();
					return FromFixed8(*r, *g, *b, a.value_or(1) * 255);
				}
			}
			else if (RegEx regEx(wxS(R"(hsla?\(([\d\.]+),\s+([\d\.]+),\s+([\d\.]+),?\s*([\d\.]*)\))")); regEx.Matches(value))
			{
				auto h = regEx.GetMatch(value, 1).ToFloatingPoint<float>();
				auto s = regEx.GetMatch(value, 2).ToFloatingPoint<float>();
				auto l = regEx.GetMatch(value, 3).ToFloatingPoint<float>();

				if (h && s && l)
				{
					Utility::SetIfNotNull(colorSpace, ColorSpace::HSL);

					auto a = regEx.GetMatch(value, 4).ToFloatingPoint<float>();
					return FromHSL(PackedHSL{Angle::FromDegrees(*h), *s, *l, a.value_or(1)});
				}
			}
		}

		Utility::SetIfNotNull(colorSpace, ColorSpace::None);
		return {};
	}
	Color Color::FromColorName(const String& name)
	{
		return wxTheColourDatabase->Find(name);
	}

	Color::Color(const wxColour& other) noexcept
	{
		if (other.IsOk())
		{
			m_Value = ToNormalizedBBP(other.Red(), other.Green(), other.Blue(), other.Alpha());
		}
	}

	String Color::ToString(C2SFormat format, C2SAlpha alpha, ColorSpace colorSpace) const
	{
		switch (format)
		{
			case C2SFormat::CSS:
			{
				switch (colorSpace)
				{
					case ColorSpace::RGB:
					{
						return CSS2RGB(*this, alpha);
					}
					case ColorSpace::HSL:
					{
						return CSS2HSL(*this, alpha);
					}
				};
				break;
			}
			case C2SFormat::HTML:
			{
				switch (colorSpace)
				{
					case ColorSpace::RGB:
					{
						return HTML2RGB(*this, alpha);
					}
					case ColorSpace::HSL:
					{
						return HTML2HSL(*this, alpha);
					}
				};
				break;
			}
		};
		return {};
	}
	String Color::GetColorName() const
	{
		const auto fixed8 = GetFixed8();
		return wxTheColourDatabase->FindName(wxColour(fixed8.Red, fixed8.Green, fixed8.Blue, wxALPHA_OPAQUE));
	}

	std::strong_ordering Color::operator<=>(const wxColour& other) const noexcept
	{
		return GetFixed8() <=> PackedRGBA<uint8_t>(other.Red(), other.Green(), other.Blue(), other.Alpha());
	}
	bool Color::operator==(const wxColour& other) const noexcept
	{
		if (IsValid() && other.IsOk())
		{
			return GetFixed8() == PackedRGBA<uint8_t>(other.Red(), other.Green(), other.Blue(), other.Alpha());
		}
		return false;
	}

	Color::operator wxColour() const noexcept
	{
		if (IsValid())
		{
			auto temp = GetFixed8();
			return wxColour(temp.Red, temp.Green, temp.Blue, temp.Alpha);
		}
		return {};
	}
	Color::operator wxBrush() const noexcept
	{
		return static_cast<wxColour>(*this);
	}
	Color::operator wxPen() const noexcept
	{
		return static_cast<wxColour>(*this);
	}
}
