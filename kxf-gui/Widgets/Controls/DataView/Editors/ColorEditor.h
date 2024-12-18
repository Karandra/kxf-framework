#pragma once
#include "../Editor.h"
#include "kxf/Core/WithOptions.h"
#include <wx/colourdata.h>
class wxGenericColourDialog;

namespace kxf::UI::DataView
{
	enum class ColorEditorOption: uint32_t
	{
		None = 0,

		GenericEditor = 1 << 0,
		FullEditor = 1 << 1,
		ShowAlpha = 1 << 2,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::DataView::ColorEditorOption);
}

namespace kxf::UI::DataView
{
	class KXF_API ColorValue: public WithOptions<ColorEditorOption, ColorEditorOption::None>
	{
		protected:
			Color m_Color;
			std::array<Color, wxColourData::NUM_CUSTOM> m_PaletteColors;

		public:
			ColorValue(const Color& color = {});

		public:
			bool FromAny(Any value);
			void Clear()
			{
				*this = {};
			}

			bool HasColor() const
			{
				return m_Color.IsValid();
			}
			Color GetColor() const
			{
				return m_Color;
			}
			void SetColor(const Color& color)
			{
				m_Color = color;
			}
			void ClearColor()
			{
				m_Color = wxNullColour;
			}

			size_t GetPaletteColorsLimit() const
			{
				return m_PaletteColors.size();
			}
			Color GetPaletteColor(size_t index) const
			{
				if (index < m_PaletteColors.size())
				{
					return m_PaletteColors[index];
				}
				return wxNullColour;
			}
			bool SetPaletteColor(size_t index, const Color& color)
			{
				if (index < m_PaletteColors.size())
				{
					m_PaletteColors[index] = color;
					return true;
				}
				return false;
			}
			void ClearPaletteColors()
			{
				m_PaletteColors.fill(wxNullColour);
			}

			wxColourData ToColorData() const;
			void FromColorData(const wxColourData& colorData);
	};
}

namespace kxf::UI::DataView
{
	class KXF_API ColorEditor: public Editor
	{
		private:
			ColorValue m_Value;
			wxDialog* m_Dialog = nullptr;

		protected:
			wxWindow* CreateControl(wxWindow& parent, const Rect& cellRect, Any value) override;
			Any GetValue(wxWindow& control) const override;
	};
}
