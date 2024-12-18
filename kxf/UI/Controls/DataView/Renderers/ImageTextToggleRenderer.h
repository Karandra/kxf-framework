#pragma once
#include "../Renderer.h"
#include "TextRenderer.h"
#include "ToggleRenderer.h"
#include "ImageTextRenderer.h"

namespace kxf::UI::DataView
{
	class KXF_API ImageTextToggleValue: public ImageTextValue, public ToggleValue
	{
		public:
			ImageTextToggleValue(String text = {},
								 const BitmapImage& image = {},
								 ToggleState state = ToggleState::None,
								 ToggleType type = ToggleType::None
			)
				:ImageTextValue(std::move(text), image), ToggleValue(state, type)
			{
			}

			ImageTextToggleValue(const BitmapImage& image)
				:ImageTextToggleValue({}, image)
			{
			}
			ImageTextToggleValue(ToggleState state, ToggleType type = ToggleType::None)
				:ImageTextToggleValue({}, {}, state, type)
			{
			}
			ImageTextToggleValue(bool checked, ToggleType type = ToggleType::None)
				:ImageTextToggleValue({}, {}, ToggleState::None, type)
			{
				SetToggleChecked(checked);
			}
			ImageTextToggleValue(bool checked, String text, const BitmapImage& image = {}, ToggleType type = ToggleType::None)
				:ImageTextToggleValue(std::move(text), image, ToggleState::None, type)
			{
				SetToggleChecked(checked);
			}

		public:
			bool FromAny(Any& value);
	};
}

namespace kxf::UI::DataView
{
	class KXF_API ImageTextToggleRenderer: public Renderer, public ToggleRendererBase
	{
		private:
			ImageTextToggleValue m_Value;

		protected:
			bool HasActivator() const override
			{
				return m_Value.HasToggleType();
			}
			Any OnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent = nullptr) override;

			bool SetDisplayValue(Any value) override;
			ToolTip CreateToolTip() const override
			{
				return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
			}

			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			ImageTextToggleRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:Renderer(alignment), ToggleRendererBase(m_Value)
			{
			}

		public:
			String GetDisplayText(Any value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
