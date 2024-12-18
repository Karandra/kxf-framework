#pragma once
#include "../Renderer.h"

namespace kxf::UI::DataView
{
	class KXF_API TextValue
	{
		protected:
			String m_Text;

		public:
			TextValue(String text = {})
				:m_Text(std::move(text))
			{
			}

		public:
			bool FromAny(Any& value);

			bool HasText() const
			{
				return !m_Text.IsEmpty();
			}
			const String& GetText() const&
			{
				return m_Text;
			}
			String GetText() &&
			{
				return std::move(m_Text);
			}
			void SetText(String text)
			{
				m_Text = std::move(text);
			}
			void ClearText()
			{
				m_Text.clear();
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API TextRenderer: public Renderer
	{
		private:
			TextValue m_Value;

		protected:
			bool SetDisplayValue(Any value) override;
			ToolTip CreateToolTip() const override
			{
				return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
			}

			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			TextRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
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
