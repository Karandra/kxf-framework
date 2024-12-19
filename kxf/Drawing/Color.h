#pragma once
#include "Common.h"
#include "Angle.h"
#include "ColorDefines.h"
#include "kxf/Serialization/BinarySerializer.h"
class wxColour;
class wxBrush;
class wxPen;

namespace kxf
{
	enum class C2SFormat
	{
		CSS,
		HTML
	};
	enum class C2SAlpha
	{
		Auto,
		Never,
		Always
	};
	enum class ColorSpace
	{
		None = 0,

		RGB,
		HSL,
		HSV
	};
}

namespace kxf
{
	class KXF_API Color final
	{
		friend struct BinarySerializer<Color>;

		public:
			constexpr static Color FromHSL(const PackedHSL& color) noexcept
			{
				return Color().SetHSL(color);
			}
			constexpr static Color FromHSV(const PackedHSV& color) noexcept
			{
				return Color().SetHSV(color);
			}
			constexpr static Color FromRGBA(uint32_t color) noexcept
			{
				return Color().SetRGBA(color);
			}
			constexpr static Color FromARGB(uint32_t color) noexcept
			{
				return Color().SetARGB(color);
			}
			constexpr static Color FromCOLORREF(uint32_t color) noexcept
			{
				return Color().SetCOLORREF(color);
			}
			constexpr static Color FromFixed8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = ColorTraits<uint8_t>::max()) noexcept
			{
				return Color().SetFixed8(r, g, b, a);
			}
			constexpr static Color FromNormalized(float r, float g, float b, float a = ColorTraits<float>::max()) noexcept
			{
				return Color().SetNormalized(r, g, b, a);
			}
			constexpr static Color FromNormalized(double r, double g, double b, double a = ColorTraits<float>::max()) noexcept
			{
				return FromNormalized(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a));
			}
			static Color FromString(const String& value, ColorSpace* colorSpace = nullptr);
			static Color FromColorName(const String& name);

			constexpr static float AlphaBlend(float foreground, float background, float alpha) noexcept
			{
				return ColorTraits<float>::clamp(background + (alpha * (foreground - background)));
			}
			constexpr static float ContrastRatio(const Color& left, const Color& right, const PackedRGB<float>& weight = ColorWeight::sRGB) noexcept
			{
				return (left.GetLuminance(weight) + 0.05f) / (right.GetLuminance(weight) + 0.05f);
			}
			constexpr static float GetDifference(const Color& left, const Color& right) noexcept
			{
				const auto l = left.GetNormalized();
				const auto r = right.GetNormalized();

				return (std::max(l.Red, r.Red) - std::min(l.Red, r.Red)) + (std::max(l.Green, r.Green) - std::min(l.Green, r.Green)) + (std::max(l.Blue, r.Blue) - std::min(l.Blue, r.Blue));
			}

			constexpr static Color SelectLighterColor(const Color& left, const Color& right, const PackedRGB<float>& weight = ColorWeight::sRGB) noexcept
			{
				const float leftLuminance = left.GetLuminance(weight);
				const float rightLuminance = right.GetLuminance(weight);

				return leftLuminance > rightLuminance ? left : right;
			}
			constexpr static Color SelectDarkerColor(const Color& left, const Color& right, const PackedRGB<float>& weight = ColorWeight::sRGB) noexcept
			{
				const float leftLuminance = left.GetLuminance(weight);
				const float rightLuminance = right.GetLuminance(weight);

				return leftLuminance < rightLuminance ? left : right;
			}
			constexpr static std::pair<Color, Color> SelectLighterAndDarkerColor(const Color& left, const Color& right, const PackedRGB<float>& weight = ColorWeight::sRGB) noexcept
			{
				Color lighter = SelectLighterColor(left, right, weight);
				Color darker = lighter == left ? right : left;

				return {lighter, darker};
			}

		private:
			PackedRGBA<float> m_Value = {-1.0f, -1.0f, -1.0f, -1.0f};

		public:
			constexpr Color() noexcept = default;
			constexpr Color(Color&&) noexcept = default;
			constexpr Color(const Color&) noexcept = default;

			constexpr Color(const PackedRGBA<uint8_t>& other) noexcept
				:m_Value(ToNormalizedBBP(other))
			{
			}
			constexpr Color(const PackedRGBA<float>& other) noexcept
				:m_Value(other)
			{
			}

			constexpr Color(const PackedRGB<uint8_t>& other) noexcept
				:Color(other.AddAlpha(255))
			{
			}
			constexpr Color(const PackedRGB<float>& other) noexcept
				:Color(other.AddAlpha(1.0f))
			{
			}

			Color(const wxColour& other) noexcept;

		public:
			// General
			constexpr bool IsValid() const noexcept
			{
				using Traits = ColorTraits<float>;
				return Traits::test_range(m_Value.Red) && Traits::test_range(m_Value.Green) && Traits::test_range(m_Value.Blue) && Traits::test_range(m_Value.Alpha);
			}
			constexpr bool IsOpaque() const noexcept
			{
				return m_Value.Alpha == 1.0f;
			}
			constexpr bool IsTransparent() const noexcept
			{
				return !IsOpaque();
			}
			constexpr Color Clone() const noexcept
			{
				return *this;
			}

			// Conversion to other types
			String ToString(C2SFormat format, C2SAlpha alpha = C2SAlpha::Auto, ColorSpace colorSpace = ColorSpace::RGB) const;
			String GetColorName() const;

			// Normalized
			constexpr PackedRGBA<float> GetNormalized() const noexcept
			{
				return m_Value;
			}
			constexpr Color& SetNormalized(const PackedRGBA<float>& other) noexcept
			{
				m_Value = other;
				return *this;
			}
			constexpr Color& SetNormalized(const PackedRGBA<double>& other) noexcept
			{
				SetNormalized(other.Red, other.Green, other.Blue, other.Alpha);
				return *this;
			}
			constexpr Color& SetNormalized(float r, float g, float b, float a = ColorTraits<float>::max()) noexcept
			{
				m_Value = PackedRGBA(r, g, b, a);
				return *this;
			}
			constexpr Color& SetNormalized(double r, double g, double b, double a = ColorTraits<float>::max()) noexcept
			{
				m_Value = PackedRGBA(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a));
				return *this;
			}

			constexpr Color& SetNormalizedRed(float r) noexcept
			{
				m_Value.Red = r;
				return *this;
			}
			constexpr Color& SetNormalizedGreen(float g) noexcept
			{
				m_Value.Green = g;
				return *this;
			}
			constexpr Color& SetNormalizedBlue(float b) noexcept
			{
				m_Value.Blue = b;
				return *this;
			}
			constexpr Color& SetNormalizedAlpha(float a) noexcept
			{
				m_Value.Alpha = a;
				return *this;
			}

			// Fixed
			constexpr PackedRGBA<uint8_t> GetFixed8() const noexcept
			{
				return ToFixedBPP<uint8_t>(m_Value);
			}
			constexpr Color& SetFixed8(const PackedRGBA<uint8_t>& other) noexcept
			{
				m_Value = ToNormalizedBBP(other);
				return *this;
			}
			constexpr Color& SetFixed8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = ColorTraits<uint8_t>::max()) noexcept
			{
				m_Value = ToNormalizedBBP(r, g, b, a);
				return *this;
			}

			constexpr Color& SetRed8(uint8_t r) noexcept
			{
				m_Value.Red = Private::ChannelToNormalizedBBP(r);
				return *this;
			}
			constexpr Color& SetGreen8(uint8_t g) noexcept
			{
				m_Value.Green = Private::ChannelToNormalizedBBP(g);
				return *this;
			}
			constexpr Color& SetBlue8(uint8_t b) noexcept
			{
				m_Value.Blue = Private::ChannelToNormalizedBBP(b);
				return *this;
			}
			constexpr Color& SetAlpha8(uint8_t a) noexcept
			{
				m_Value.Alpha = Private::ChannelToNormalizedBBP(a);
				return *this;
			}

			// Color-space and representation conversions
			constexpr uint32_t GetRGBA() const noexcept
			{
				auto temp = GetFixed8();
				return ((temp.Red & 0xff) << 24) + ((temp.Green & 0xff) << 16) + ((temp.Blue & 0xff) << 8) + (temp.Alpha & 0xff);
			}
			constexpr Color& SetRGBA(uint32_t color) noexcept
			{
				if (((color >> 24) & 0xFF) == 0)
				{
					SetFixed8((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
				}
				else
				{
					SetFixed8((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
				}
				return *this;
			}

			constexpr uint32_t GetARGB() const noexcept
			{
				auto temp = GetFixed8();
				return ((temp.Alpha & 0xff) << 24) + ((temp.Red & 0xff) << 16) + ((temp.Green & 0xff) << 8) + (temp.Blue & 0xff);
			}
			constexpr Color& SetARGB(uint32_t color) noexcept
			{
				uint8_t a = ((color >> 24) & 0xff);
				uint8_t r = ((color >> 16) & 0xff);
				uint8_t g = ((color >> 8) & 0xff);
				uint8_t b = (color & 0xff);

				SetFixed8(r, g, b, a);
				return *this;
			}

			constexpr uint32_t GetCOLORREF() const noexcept
			{
				auto temp = GetFixed8();
				return (temp.Red | (static_cast<uint16_t>(temp.Green) << 8)) | (static_cast<uint32_t>(temp.Blue) << 16);
			}
			constexpr Color& SetCOLORREF(uint32_t color) noexcept
			{
				uint8_t r = (color & 0xff);
				uint8_t g = ((color >> 8) & 0xff);
				uint8_t b = ((color >> 16) & 0xff);

				SetFixed8(r, g, b);
				return *this;
			}

			constexpr PackedHSL GetHSL() const noexcept
			{
				// https://en.wikipedia.org/wiki/HSL_and_HSV#From_RGB

				PackedHSL hsl;
				hsl.Alpha = m_Value.Alpha;

				const float r = m_Value.Red;
				const float g = m_Value.Green;
				const float b = m_Value.Blue;

				const float Xmax = std::max({r, g, b});
				const float Xmin = std::min({r, g, b});
				const float C = Xmax - Xmin;
				const float V = Xmax;

				// Hue
				if (C == 0.0f)
				{
					hsl.Hue = Angle::FromDegrees(0);
				}
				else if (V == r)
				{
					hsl.Hue = Angle::FromDegrees(60.0f * (0.0f + (g - b) / C));
				}
				else if (V == g)
				{
					hsl.Hue = Angle::FromDegrees(60.0f * (2.0f + (b - r) / C));
				}
				else if (V == b)
				{
					hsl.Hue = Angle::FromDegrees(60.0f * (4.0f + (r - g) / C));
				}

				// Lightness
				hsl.Lightness = (Xmax + Xmin) / 2.0f;

				// Saturation
				if (hsl.Lightness == 0.0f || hsl.Lightness == 1.0f)
				{
					hsl.Saturation = 0.0f;
				}
				else
				{
					hsl.Saturation = (V - hsl.Lightness) / std::min(hsl.Lightness, 1.0f - hsl.Lightness);
				}

				return hsl;
			}
			constexpr Color& SetHSL(const PackedHSL& hsl) noexcept
			{
				// https://en.wikipedia.org/wiki/HSL_and_HSV#HSL_to_RGB_alternative
				const float a = hsl.Saturation * std::min(hsl.Lightness, 1.0f - hsl.Lightness);
				auto f = [&](int n)
				{
					const float k = std::fmod(n + hsl.Hue.ToDegrees() / 30.0f, 12.0f);
					return hsl.Lightness - a * std::max(-1.0f, std::min({k - 3.0f, 9.0f - k, 1.0f}));
				};

				SetNormalized(f(0), f(8), f(4), hsl.Alpha);
				return *this;
			}

			constexpr PackedHSV GetHSV() const noexcept
			{
				return ToHSV(GetHSL());
			}
			constexpr Color& SetHSV(const PackedHSV& hsv) noexcept
			{
				return SetHSL(ToHSL(hsv));
			}

			// Operations
			constexpr Color MakeMono(bool isWhite = true) const noexcept
			{
				using Traits = ColorTraits<float>;

				const float value = isWhite ? Traits::max() : Traits::min();
				return FromNormalized(value, value, value, m_Value.Alpha);
			}
			constexpr Color MakeGray(const PackedRGB<float>& weight = ColorWeight::sRGB) const noexcept
			{
				// https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
				const float luma = m_Value.Red * weight.Red + m_Value.Green * weight.Green + m_Value.Blue * weight.Blue;
				return FromNormalized(luma, luma, luma, m_Value.Alpha);
			}
			constexpr Color MakeDisabled(float brightness = 1.0f) const noexcept
			{
				constexpr float alpha = 0.4f;

				auto temp = GetNormalized();
				temp.Red = AlphaBlend(temp.Red, brightness, alpha);
				temp.Green = AlphaBlend(temp.Green, brightness, alpha);
				temp.Blue = AlphaBlend(temp.Blue, brightness, alpha);

				return temp;
			}
			constexpr Color ChangeLightness(Angle alphaAngle) const noexcept
			{
				float alpha = alphaAngle.ToNormalized();
				if (alpha != 0.5f)
				{
					using Traits = ColorTraits<float>;

					float background = 0;
					if (alpha > 0.5f)
					{
						// Blend with white
						alpha = 1.0f - alpha;
						background = Traits::max();
					}
					else
					{
						// Blend with black
						alpha = 1.0f + alpha;
						background = 0;
					}

					auto temp = GetNormalized();
					temp.Red = AlphaBlend(temp.Red, background, alpha);
					temp.Green = AlphaBlend(temp.Green, background, alpha);
					temp.Blue = AlphaBlend(temp.Blue, background, alpha);
					return temp;
				}
				return *this;
			}
			constexpr Color RotateHue(Angle angle) const noexcept
			{
				PackedHSL hsl = GetHSL();
				hsl.Hue += angle;

				return Color::FromHSL(hsl);
			}
			constexpr Color Invert() const noexcept
			{
				PackedHSL hsl = GetHSL();
				hsl.Hue += Angle::FromNormalized(0.5f);

				return Color().SetHSL(hsl);
			}
			constexpr Color Negate() const noexcept
			{
				return FromRGBA(0xFFFFFF00u ^ GetRGBA());
			}
			constexpr Color GetContrastColor(const Color& light, const Color& dark, const PackedRGB<float>& weight = ColorWeight::sRGB) const noexcept
			{
				// https://stackoverflow.com/questions/7260989/how-to-pick-good-contrast-rgb-colors-programmatically/7261283#7261283
				return GetLuminance(weight) < 0.5f ? light : dark;
			}

			constexpr float GetDifference(const Color& other) const noexcept
			{
				const float r1 = m_Value.Red;
				const float g1 = m_Value.Green;
				const float b1 = m_Value.Blue;

				const float r2 = other.m_Value.Red;
				const float g2 = other.m_Value.Green;
				const float b2 = other.m_Value.Blue;

				return (std::max(r1, r2) - std::min(r1, r2)) + (std::max(g1, g2) - std::min(g1, g2)) + (std::max(b1, b2) - std::min(b1, b2));
			}
			constexpr float GetLuminance(const PackedRGB<float>& weight = ColorWeight::sRGB) const noexcept
			{
				// https://www.w3.org/TR/WCAG/#dfn-relative-luminance
				// https://stackoverflow.com/a/56678483/6512579
				auto SRGBToLinear = [](float channel) constexpr noexcept
				{
					// Send this function a decimal sRGB gamma encoded color value
					// between 0.0 and 1.0, and it returns a linearized value.

					if (channel <= 0.03928f)
					{
						return channel / 12.92f;
					}
					else
					{
						return std::pow((channel + 0.055f) / 1.055f, 2.4f);
					}
				};

				const float r = SRGBToLinear(m_Value.Red);
				const float g = SRGBToLinear(m_Value.Green);
				const float b = SRGBToLinear(m_Value.Blue);
				return r * weight.Red + g * weight.Green + b * weight.Blue;
			}
			constexpr float GetPercievedLighteness(const PackedRGB<float>& weight = ColorWeight::sRGB) const noexcept
			{
				auto YToLstar = [](float luminance) constexpr noexcept
				{
					// Send this function a luminance value between 0.0 and 1.0,
					// and it returns L* which is "perceptual lightness"

					// The CIE standard states 0.008856 but 216/24389 is the intent for 0.008856451679036
					if (luminance <= (216.0f / 24389.0f))
					{
						// The CIE standard states 903.3, but 24389/27 is the intent, making 903.296296296296296
						return luminance * (24389.0f / 27.0f);
					}
					else
					{
						return std::pow(luminance, (1.0f / 3.0f)) * 116.0f - 16.0f;
					}
				};
				return YToLstar(GetLuminance(weight));
			}

		public:
			constexpr explicit operator bool() const noexcept
			{
				return IsValid();
			}
			constexpr bool operator!() const noexcept
			{
				return !IsValid();
			}

			constexpr auto operator<=>(const Color&) const noexcept = default;
			constexpr bool operator==(const Color& other) const noexcept
			{
				return m_Value == other.m_Value;
			}

			std::strong_ordering operator<=>(const wxColour& other) const noexcept;
			bool operator==(const wxColour& other) const noexcept;

			constexpr Color& operator=(Color&&) noexcept = default;
			constexpr Color& operator=(const Color&) noexcept = default;

			operator wxColour() const noexcept;
			operator wxBrush() const noexcept;
			operator wxPen() const noexcept;
	};
}

namespace kxf::Drawing
{
	constexpr Color GetStockColor(StockColor color) noexcept
	{
		switch (color)
		{
			case StockColor::Transparent:
			{
				return Color::FromNormalized(0.0f, 0.0f, 0.0f, 0.0f);
			}
			case StockColor::Black:
			{
				return Color::FromNormalized(0.0f, 0.0f, 0.0f);
			}
			case StockColor::White:
			{
				return Color::FromNormalized(1.0f, 1.0f, 1.0f);
			}
			case StockColor::Cyan:
			{
				return Color::FromNormalized(0.0f, 1.0f, 1.0f);
			}
			case StockColor::Blue:
			{
				return Color::FromNormalized(0.0f, 0.0f, 1.0f);
			}
			case StockColor::Red:
			{
				return Color::FromNormalized(1.0f, 0.0f, 0.0f);
			}
			case StockColor::Green:
			{
				return Color::FromNormalized(0.0f, 1.0f, 0.0f);
			}
			case StockColor::Yellow:
			{
				return Color::FromNormalized(1.0f, 1.0f, 0.0f);
			}
			case StockColor::Gray:
			{
				return Color::FromNormalized(0.5f, 0.5f, 0.5f);
			}
			case StockColor::LightGray:
			{
				return Color::FromNormalized(0.83f, 0.83f, 0.83f);
			}
			case StockColor::MediumGray:
			{
				return Color::FromNormalized(0.66f, 0.66f, 0.66f);
			}
			case StockColor::Purple:
			{
				return Color::FromFixed8(196, 0, 171);
			}
			case StockColor::Magenta:
			{
				return Color::FromNormalized(1.0f, 0.0f, 1.0f);
			}
		};
		return {};
	}
}

namespace kxf
{
	template<>
	struct BinarySerializer<Color> final
	{
		uint64_t Serialize(IOutputStream& stream, const Color& value) const
		{
			return Serialization::WriteObject(stream, value.m_Value.Red) +
				Serialization::WriteObject(stream, value.m_Value.Green) +
				Serialization::WriteObject(stream, value.m_Value.Blue) +
				Serialization::WriteObject(stream, value.m_Value.Alpha);
		}
		uint64_t Deserialize(IInputStream& stream, Color& value) const
		{
			return Serialization::ReadObject(stream, value.m_Value.Red) +
				Serialization::ReadObject(stream, value.m_Value.Green) +
				Serialization::ReadObject(stream, value.m_Value.Blue) +
				Serialization::ReadObject(stream, value.m_Value.Alpha);
		}
	};
}
