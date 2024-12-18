#pragma once
#include "Common.h"
#include "kxf/Core/WithOptions.h"

namespace kxf::UI::DataView
{
	class Renderer;
	class RenderEngine;
}

namespace kxf::UI::DataView
{
	enum class CellOption: uint32_t
	{
		None = 0,
		Enabled = 1 << 0,
		Editable = 1 << 1,
		Category = 1 << 2,
		HighlightItem = 1 << 3,
		ShowAccelerators = 1 << 4,

		Default = Enabled|Editable
	};
	enum class CellBGOption: uint32_t
	{
		None = 0,
		Header = 1 << 0,
		Button = 1 << 1,
		ComboBox = 1 << 2,

		Default = None
	};
	enum class CellFontOption: uint32_t
	{
		None = 0,
		Bold = 1 << 0,
		Italic = 1 << 1,
		Underlined = 1 << 2,
		Strikethrough = 1 << 3,

		Default = None
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::DataView::CellOption);
	kxf_FlagSet_Declare(UI::DataView::CellBGOption);
	kxf_FlagSet_Declare(UI::DataView::CellFontOption);
}

namespace kxf::UI::DataView::CellAttributes
{
	class Options final: public WithOptions<CellOption, CellOption::Default>
	{
		friend class Renderer;
		friend class RenderEngine;

		private:
			Color m_ForegroundColor;
			Color m_BackgroundColor;
			FlagSet<Alignment> m_Alignment = Alignment::Invalid;

		private:
			bool RequiresNeedAlteration() const
			{
				return HasColors();
			}
			void ChangeAlpha(Color& color, uint8_t alpha) const
			{
				color.SetAlpha8(alpha);
			}

		public:
			bool IsDefault() const
			{
				return !HasColors() && !HasAlignment() && GetOptionFlags() == CellOption::Default;
			}

			// Colors
			bool HasColors() const
			{
				return HasBackgroundColor() || HasForegroundColor();
			}

			bool HasBackgroundColor() const
			{
				return m_BackgroundColor.IsValid();
			}
			Color GetBackgroundColor() const
			{
				return m_BackgroundColor;
			}
			void SetBackgroundColor(const Color& color)
			{
				m_BackgroundColor = color;
			}
			void SetBackgroundColor(const Color& color, uint8_t alpha)
			{
				m_BackgroundColor = color;
				ChangeAlpha(m_BackgroundColor, alpha);
			}

			bool HasForegroundColor() const
			{
				return m_ForegroundColor.IsValid();
			}
			Color GetForegroundColor() const
			{
				return m_ForegroundColor;
			}
			void SetForegroundColor(const Color& color)
			{
				m_ForegroundColor = color;
			}
			void SetForegroundColor(const Color& color, uint8_t alpha)
			{
				m_ForegroundColor = color;
				ChangeAlpha(m_ForegroundColor, alpha);
			}

			// Alignment
			bool HasAlignment() const
			{
				return m_Alignment != Alignment::Invalid;
			}
			FlagSet<Alignment> GetAlignment() const
			{
				return m_Alignment;
			}
			void SetAlignment(FlagSet<Alignment> alignment)
			{
				m_Alignment = alignment;
			}
	};
}

namespace kxf::UI::DataView::CellAttributes
{
	class FontOptions final: public WithOptions<CellFontOption, CellFontOption::Default>
	{
		friend class Renderer;
		friend class RenderEngine;

		private:
			wxString m_FontFace;
			int32_t m_FontSize = 0;

		private:
			bool RequiresNeedAlteration() const
			{
				return HasFontFace() || HasFontSize() || GetOptionFlags() != CellFontOption::Default;
			}

		public:
			bool IsDefault() const
			{
				return !RequiresNeedAlteration();
			}

			bool HasFontFace() const
			{
				return !m_FontFace.IsEmpty();
			}
			const wxString& GetFontFace() const
			{
				return m_FontFace;
			}
			void SetFontFace(const wxString& faceName)
			{
				m_FontFace = faceName;
			}

			bool HasFontSize() const
			{
				return m_FontSize > 0;
			}
			int32_t GetFontSize() const
			{
				return m_FontSize;
			}
			void SetFontSize(int32_t value)
			{
				m_FontSize = std::clamp(value, 0, std::numeric_limits<int32_t>::max());
			}
	};
}

namespace kxf::UI::DataView::CellAttributes
{
	class CellBGOptions final: public WithOptions<CellBGOption, CellBGOption::Default>
	{
		friend class Renderer;
		friend class RenderEngine;

		private:
			bool RequiresNeedAlteration() const
			{
				return GetOptionFlags() != CellBGOption::Default;
			}

		public:
			bool IsDefault() const
			{
				return !RequiresNeedAlteration();
			}
	};
}
