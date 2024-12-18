#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include "../Private/Common.h"
#include <wx/font.h>

namespace kxf
{
	class Font;
}

namespace kxf
{
	class KXF_API GDIFont: public RTTI::ExtendInterface<GDIFont, IGDIObject>
	{
		kxf_RTTI_DeclareIID(GDIFont, {0xbeb3a65c, 0xf639, 0x4e44, {0x80, 0x3a, 0x1b, 0x53, 0xf6, 0x9c, 0x61, 0xd8}});

		public:
			static FontEncoding GetDefaultEncoding() noexcept
			{
				return Drawing::Private::MapFontEncoding(wxFont::GetDefaultEncoding());
			}
			static void SetDefaultEncoding(FontEncoding encoding) noexcept
			{
				wxFont::SetDefaultEncoding(Drawing::Private::MapFontEncoding(encoding));
			}

			static int GetNumericWeightOf(FontWeight weight) noexcept
			{
				return wxFont::GetNumericWeightOf(Drawing::Private::MapFontWeight(weight));
			}

		private:
			wxFont m_Font;

		public:
			GDIFont() = default;
			GDIFont(const Font& other);
			GDIFont(const wxFont& other)
				:m_Font(other)
			{
			}
			GDIFont(const wxNativeFontInfo& other)
				:m_Font(other)
			{
			}

			GDIFont(float pointSize, FontFamily family, FlagSet<FontStyle> style, FontWeight weight, const String& faceName = {}, FontEncoding encoding = FontEncoding::Default)
				:m_Font(static_cast<int>(pointSize), Drawing::Private::MapFontFamily(family), Drawing::Private::MapFontStyle(style), Drawing::Private::MapFontWeight(weight), style.Contains(FontStyle::Underline), faceName, Drawing::Private::MapFontEncoding(encoding))
			{
				m_Font.SetFractionalPointSize(static_cast<double>(pointSize));
				m_Font.SetStrikethrough(style.Contains(FontStyle::Strikethrough));
			}
			GDIFont(const Size& pixelSize, FontFamily family, FlagSet<FontStyle> style, FontWeight weight, const String& faceName = {}, FontEncoding encoding = FontEncoding::Default)
				:m_Font(pixelSize, Drawing::Private::MapFontFamily(family), Drawing::Private::MapFontStyle(style), Drawing::Private::MapFontWeight(weight), style.Contains(FontStyle::Underline), faceName, Drawing::Private::MapFontEncoding(encoding))
			{
				m_Font.SetStrikethrough(style.Contains(FontStyle::Strikethrough));
			}

			virtual ~GDIFont() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Font.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto font = other.QueryInterface<GDIFont>())
				{
					return m_Font == font->m_Font;
				}
				return false;
			}
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIFont>(m_Font);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// GDIFont
			const wxFont& ToWxFont() const noexcept
			{
				return m_Font;
			}
			wxFont& ToWxFont() noexcept
			{
				return m_Font;
			}

			String GetDescription() const
			{
				return m_Font.GetNativeFontInfoUserDesc();
			}
			GDIFont GetBaseFont() const
			{
				return m_Font.GetBaseFont();
			}

			String Serialize() const
			{
				return m_Font.GetNativeFontInfoDesc();
			}
			bool Deserialize(const String& serializedData)
			{
				return m_Font.SetNativeFontInfo(serializedData);
			}

			String GetFaceName() const
			{
				return m_Font.GetFaceName();
			}
			bool SetFaceName(const String& faceName)
			{
				return m_Font.SetFaceName(faceName);
			}

			float GetPointSize() const
			{
				return static_cast<float>(m_Font.GetFractionalPointSize());
			}
			void SetPointSize(float pointSize)
			{
				m_Font.SetFractionalPointSize(static_cast<double>(pointSize));
			}

			Size GetPixelSize() const
			{
				return Size(m_Font.GetPixelSize());
			}
			void SetPixelSize(const Size& pixelSize)
			{
				m_Font.SetPixelSize(pixelSize);
			}

			void SetSymbolicSize(FontSymbolicSize size)
			{
				m_Font.SetSymbolicSize(Drawing::Private::MapFontSymbolicSize(size));
			}
			void SetSymbolicSizeRelativeTo(FontSymbolicSize size, int base)
			{
				m_Font.SetSymbolicSizeRelativeTo(Drawing::Private::MapFontSymbolicSize(size), base);
			}
			void ScaleSzie(double scale)
			{
				m_Font.SetFractionalPointSize(m_Font.GetFractionalPointSize() * scale);
			}

			FontEncoding GetEncoding() const
			{
				return Drawing::Private::MapFontEncoding(m_Font.GetEncoding());
			}
			void SetEncoding(FontEncoding encoding)
			{
				m_Font.SetEncoding(Drawing::Private::MapFontEncoding(encoding));
			}

			FontFamily GetFamily() const
			{
				return Drawing::Private::MapFontFamily(m_Font.GetFamily());
			}
			void SetFamily(FontFamily family)
			{
				m_Font.SetFamily(Drawing::Private::MapFontFamily(family));
			}

			FlagSet<FontStyle> GetStyle() const
			{
				auto style = Drawing::Private::MapFontStyle(m_Font.GetStyle());
				style.Add(FontStyle::Underline, m_Font.GetUnderlined());
				style.Add(FontStyle::Strikethrough, m_Font.GetStrikethrough());

				return style;
			}
			void SetStyle(FlagSet<FontStyle> style)
			{
				m_Font.SetStyle(Drawing::Private::MapFontStyle(style));
				m_Font.SetUnderlined(style.Contains(FontStyle::Underline));
				m_Font.SetStrikethrough(style.Contains(FontStyle::Strikethrough));
			}
			void AddStyle(FlagSet<FontStyle> style)
			{
				SetStyle(GetStyle().Add(style));
			}
			void RemoveStyle(FlagSet<FontStyle> style)
			{
				SetStyle(GetStyle().Remove(style));
			}

			FontWeight GetWeight() const
			{
				return Drawing::Private::MapFontWeight(m_Font.GetWeight());
			}
			void SetWeight(FontWeight weight)
			{
				m_Font.SetWeight(Drawing::Private::MapFontWeight(weight));
			}
			int GetNumericWeight() const
			{
				return m_Font.GetNumericWeight();
			}
			void SetNumericWeight(int weight)
			{
				m_Font.SetNumericWeight(weight);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			GDIFont& operator=(const GDIFont& other)
			{
				m_Font = other.m_Font;

				return *this;
			}
	};
}
