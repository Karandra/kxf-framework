#pragma once
#include "Common.h"
#include "Geometry.h"
#include "kxf/Core/String.h"
#include "kxf/Serialization/BinarySerializer.h"
class wxFont;
class wxNativeFontInfo;

namespace kxf
{
	class KXF_API Font
	{
		friend struct BinarySerializer<Font>;

		public:
			static FontEncoding GetDefaultEncoding() noexcept;
			static void SetDefaultEncoding(FontEncoding encoding) noexcept;

			static int GetNumericWeightOf(FontWeight weight) noexcept;
			static FontWeight GetSymbolicWeightOf(int weight) noexcept;

		private:
			String m_FaceName;
			Size m_PixelSize = Size::UnspecifiedSize();
			float m_PointSize = 0;
			int m_Weight = 0;
			FontFamily m_Family = FontFamily::None;
			FontEncoding m_Encoding = FontEncoding::None;
			FlagSet<FontStyle> m_Style;

		private:
			void CreateFrom(const wxFont& other);
			void CreateFrom(const wxNativeFontInfo& other);
			void InitCommon()
			{
				if (m_FaceName.IsEmptyOrWhitespace())
				{
					m_FaceName.clear();
				}
			}

		public:
			Font() = default;
			Font(const Font&) = default;
			Font(Font&&) noexcept = default;

			Font(const wxFont& other)
			{
				CreateFrom(other);
				InitCommon();
			}
			Font(const wxNativeFontInfo& other)
			{
				CreateFrom(other);
				InitCommon();
			}

			Font(float pointSize, FontFamily family, FlagSet<FontStyle> style, FontWeight weight, String faceName = {}, FontEncoding encoding = FontEncoding::Default)
				:m_Family(family), m_Style(style), m_Weight(GetNumericWeightOf(weight)), m_FaceName(std::move(faceName)), m_Encoding(encoding)
			{
				InitCommon();
				SetPointSize(pointSize);
			}
			Font(const Size& pixelSize, FontFamily family, FlagSet<FontStyle> style, FontWeight weight, String faceName = {}, FontEncoding encoding = FontEncoding::Default)
				:m_PixelSize(pixelSize), m_Family(family), m_Style(style), m_Weight(GetNumericWeightOf(weight)), m_FaceName(std::move(faceName)), m_Encoding(encoding)
			{
				InitCommon();
				SetPixelSize(pixelSize);
			}

			virtual ~Font() = default;

		public:
			bool IsNull() const noexcept;
			bool IsInstalled() const;
			Font GetBaseFont() const;

			String GetFaceName() const
			{
				return m_FaceName;
			}
			void SetFaceName(String faceName)
			{
				m_FaceName = std::move(faceName);
			}

			float GetPointSize() const;
			void SetPointSize(float pointSize);
			void ScaleSzie(float scale);

			Size GetPixelSize() const;
			void SetPixelSize(const Size& pixelSize);

			FontEncoding GetEncoding() const
			{
				return m_Encoding;
			}
			void SetEncoding(FontEncoding encoding)
			{
				m_Encoding = encoding;
			}

			FontFamily GetFamily() const
			{
				return m_Family;
			}
			void SetFamily(FontFamily family)
			{
				m_Family = family;
			}

			FlagSet<FontStyle> GetStyle() const
			{
				return m_Style;
			}
			void SetStyle(FlagSet<FontStyle> style)
			{
				m_Style = style;
			}
			void AddStyle(FlagSet<FontStyle> style)
			{
				m_Style.Add(style);
			}
			void RemoveStyle(FlagSet<FontStyle> style)
			{
				m_Style.Remove(style);
			}

			FontWeight GetWeight() const
			{
				return GetSymbolicWeightOf(m_Weight);
			}
			void SetWeight(FontWeight weight)
			{
				m_Weight = GetNumericWeightOf(weight);
			}
			int GetNumericWeight() const
			{
				return m_Weight;
			}
			void SetNumericWeight(int weight)
			{
				m_Weight = weight;
			}

			wxFont ToWXFont() const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			bool operator==(const Font&) const noexcept = default;

			Font& operator=(const Font&) = default;
			Font& operator=(Font&&) noexcept = default;
	};
}

namespace kxf
{
	template<>
	struct KXF_API BinarySerializer<Font> final
	{
		uint64_t Serialize(IOutputStream& stream, const Font& value) const;
		uint64_t Deserialize(IInputStream& stream, Font& value) const;
	};
}
