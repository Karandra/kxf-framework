#pragma once
#include "Common.h"
#include "kxf/Core/WithOptions.h"

namespace kxf::DataView
{
	enum class CellStyle: uint32_t
	{
		None = 0,
		Enabled = FlagSetValue<CellStyle>(0),
		Editable = FlagSetValue<CellStyle>(1),
		Category = FlagSetValue<CellStyle>(2),
		HighlightItem = FlagSetValue<CellStyle>(3),
		ShowAccelerators = FlagSetValue<CellStyle>(4),

		Default = Enabled|Editable
	};
	enum class CellBGStyle: uint32_t
	{
		None = 0,
		Header = FlagSetValue<CellBGStyle>(0),
		Button = FlagSetValue<CellBGStyle>(1),
		ComboBox = FlagSetValue<CellBGStyle>(2),

		Default = None
	};
	enum class CellFontStyle: uint32_t
	{
		None = 0,
		Bold = FlagSetValue<CellFontStyle>(0),
		Italic = FlagSetValue<CellFontStyle>(1),
		Underlined = FlagSetValue<CellFontStyle>(2),
		Strikethrough = FlagSetValue<CellFontStyle>(3),

		Default = None
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(DataView::CellStyle);
	kxf_FlagSet_Declare(DataView::CellBGStyle);
	kxf_FlagSet_Declare(DataView::CellFontStyle);
}

namespace kxf::DataView
{
	class CellOptions final: public WithOptions<CellStyle, CellStyle::Default>
	{
		private:
			Color m_ForegroundColor;
			Color m_BackgroundColor;
			FlagSet<Alignment> m_Alignment = Alignment::Invalid;
			int m_RowHeight = -1;

		private:
			bool IsAltered() const noexcept
			{
				return HasColors() || m_RowHeight > 0 || HasAlignment() || GetOptionFlags() != CellStyle::Default;
			}
			void ChangeAlpha(Color& color, uint8_t alpha) const noexcept
			{
				color.SetAlpha8(alpha);
			}

		public:
			bool IsDefault() const noexcept
			{
				return !IsAltered();
			}

			int GetRowHeight() const noexcept
			{
				return m_RowHeight;
			}
			void SetRowHeight(int height) noexcept
			{
				m_RowHeight = std::clamp(height, -1, std::numeric_limits<int>::max());
			}

			// Colors
			bool HasColors() const noexcept
			{
				return HasBackgroundColor() || HasForegroundColor();
			}

			bool HasBackgroundColor() const noexcept
			{
				return m_BackgroundColor.IsValid();
			}
			Color GetBackgroundColor() const noexcept
			{
				return m_BackgroundColor;
			}
			void SetBackgroundColor(const Color& color) noexcept
			{
				m_BackgroundColor = color;
			}
			void SetBackgroundColor(const Color& color, uint8_t alpha) noexcept
			{
				m_BackgroundColor = color;
				ChangeAlpha(m_BackgroundColor, alpha);
			}

			bool HasForegroundColor() const noexcept
			{
				return m_ForegroundColor.IsValid();
			}
			Color GetForegroundColor() const noexcept
			{
				return m_ForegroundColor;
			}
			void SetForegroundColor(const Color& color) noexcept
			{
				m_ForegroundColor = color;
			}
			void SetForegroundColor(const Color& color, uint8_t alpha) noexcept
			{
				m_ForegroundColor = color;
				ChangeAlpha(m_ForegroundColor, alpha);
			}

			// Alignment
			bool HasAlignment() const noexcept
			{
				return m_Alignment != Alignment::Invalid;
			}
			FlagSet<Alignment> GetAlignment() const noexcept
			{
				return m_Alignment;
			}
			void SetAlignment(FlagSet<Alignment> alignment) noexcept
			{
				m_Alignment = alignment;
			}
	};
}

namespace kxf::DataView
{
	class CellFontOptions final: public WithOptions<CellFontStyle, CellFontStyle::Default>
	{
		private:
			String m_FontFace;
			int m_FontSize = -1;

		private:
			bool IsAltered() const noexcept
			{
				return HasFontFace() || HasFontSize() || GetOptionFlags() != CellFontStyle::Default;
			}

		public:
			bool IsDefault() const noexcept
			{
				return !IsAltered();
			}

			bool HasFontFace() const noexcept
			{
				return !m_FontFace.IsEmpty();
			}
			const String& GetFontFace() const
			{
				return m_FontFace;
			}
			void SetFontFace(String faceName) noexcept
			{
				m_FontFace = std::move(faceName);
			}

			bool HasFontSize() const noexcept
			{
				return m_FontSize > 0;
			}
			int GetFontSize() const noexcept
			{
				return m_FontSize;
			}
			void SetFontSize(int value) noexcept
			{
				m_FontSize = std::clamp(value, -1, std::numeric_limits<int>::max());
			}
	};
}

namespace kxf::DataView
{
	class CellBGOptions final: public WithOptions<CellBGStyle, CellBGStyle::Default>
	{
		public:
			bool IsDefault() const noexcept
			{
				return GetOptionFlags() == CellBGStyle::Default;
			}
	};
}
