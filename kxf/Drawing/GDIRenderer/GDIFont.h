#pragma once
#include "Common.h"
#include <wx/font.h>
#include "kxf/wxWidgets/MapDrawing.h"

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
			static FontEncoding GetDefaultEncoding() noexcept;
			static void SetDefaultEncoding(FontEncoding encoding) noexcept;

			static int GetNumericWeightOf(FontWeight weight) noexcept;

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
				:m_Font(static_cast<int>(pointSize), wxWidgets::MapFontFamily(family), wxWidgets::MapFontStyle(style), wxWidgets::MapFontWeight(weight), style.Contains(FontStyle::Underline), faceName, wxWidgets::MapFontEncoding(encoding))
			{
				m_Font.SetFractionalPointSize(static_cast<double>(pointSize));
				m_Font.SetStrikethrough(style.Contains(FontStyle::Strikethrough));
			}
			GDIFont(const Size& pixelSize, FontFamily family, FlagSet<FontStyle> style, FontWeight weight, const String& faceName = {}, FontEncoding encoding = FontEncoding::Default)
				:m_Font(pixelSize, wxWidgets::MapFontFamily(family), wxWidgets::MapFontStyle(style), wxWidgets::MapFontWeight(weight), style.Contains(FontStyle::Underline), faceName, wxWidgets::MapFontEncoding(encoding))
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
			std::shared_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_shared<GDIFont>(m_Font);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// GDIFont
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
				m_Font.SetSymbolicSize(wxWidgets::MapFontSymbolicSize(size));
			}
			void SetSymbolicSizeRelativeTo(FontSymbolicSize size, int base)
			{
				m_Font.SetSymbolicSizeRelativeTo(wxWidgets::MapFontSymbolicSize(size), base);
			}
			void ScaleSzie(double scale)
			{
				m_Font.SetFractionalPointSize(m_Font.GetFractionalPointSize() * scale);
			}

			FontEncoding GetEncoding() const
			{
				return wxWidgets::MapFontEncoding(m_Font.GetEncoding());
			}
			void SetEncoding(FontEncoding encoding)
			{
				m_Font.SetEncoding(wxWidgets::MapFontEncoding(encoding));
			}

			FontFamily GetFamily() const
			{
				return wxWidgets::MapFontFamily(m_Font.GetFamily());
			}
			void SetFamily(FontFamily family)
			{
				m_Font.SetFamily(wxWidgets::MapFontFamily(family));
			}

			FlagSet<FontStyle> GetStyle() const
			{
				auto style = wxWidgets::MapFontStyle(m_Font.GetStyle());
				style.Add(FontStyle::Underline, m_Font.GetUnderlined());
				style.Add(FontStyle::Strikethrough, m_Font.GetStrikethrough());

				return style;
			}
			void SetStyle(FlagSet<FontStyle> style)
			{
				m_Font.SetStyle(wxWidgets::MapFontStyle(style));
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
				return wxWidgets::MapFontWeight(m_Font.GetWeight());
			}
			void SetWeight(FontWeight weight)
			{
				m_Font.SetWeight(wxWidgets::MapFontWeight(weight));
			}
			int GetNumericWeight() const
			{
				return m_Font.GetNumericWeight();
			}
			void SetNumericWeight(int weight)
			{
				m_Font.SetNumericWeight(weight);
			}

			Font ToFont() const;
			wxFont& AsWXFont() noexcept
			{
				return m_Font;
			}
			const wxFont& AsWXFont() const noexcept
			{
				return m_Font;
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
