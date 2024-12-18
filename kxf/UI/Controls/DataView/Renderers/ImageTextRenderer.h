#pragma once
#include "../Renderer.h"
#include "kxf/Core/WithOptions.h"
#include "TextRenderer.h"
#include "ImageRenderer.h"

namespace kxf::UI::DataView
{
	enum class ImageTextValueOption: uint32_t
	{
		None = 0,
		VCenterText = 1 << 0
	};

	class KXF_API ImageTextValue: public TextValue, public ImageValue, public WithOptions<ImageTextValueOption, ImageTextValueOption::VCenterText>
	{
		public:
			using Options = ImageTextValueOption;

		public:
			ImageTextValue() = default;
			ImageTextValue(const BitmapImage& image)
				:ImageValue(image)
			{
			}
			ImageTextValue(String text)
				:TextValue(std::move(text))
			{
			}
			ImageTextValue(String text, const BitmapImage& image)
				:TextValue(std::move(text)), ImageValue(image)
			{
			}

		public:
			bool FromAny(Any& value);
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::DataView::ImageTextValueOption);
}

namespace kxf::UI::DataView
{
	class KXF_API ImageTextRenderer: public Renderer
	{
		private:
			ImageTextValue m_Value;

		protected:
			bool SetDisplayValue(Any value) override;
			ToolTip CreateToolTip() const override
			{
				return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
			}

			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			ImageTextRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:Renderer(alignment)
			{
			}

		public:
			String GetDisplayText(Any value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
