#pragma once
#include "../Renderer.h"

namespace kxf::UI::DataView
{
	class KXF_API ToggleValue
	{
		private:
			ToggleState m_State = ToggleState::None;
			ToggleType m_Type = ToggleType::None;
			bool m_Allow3State = false;

		public:
			ToggleValue(ToggleState state = ToggleState::None, ToggleType type = ToggleType::None)
				:m_State(state), m_Type(type)
			{
			}
			ToggleValue(bool checked, ToggleType type = ToggleType::None)
				:m_Type(type)
			{
				SetToggleChecked(checked);
			}

		public:
			bool FromAny(Any& value);

			bool HasToggleState() const
			{
				return m_State != ToggleState::None;
			}
			ToggleState& GetToggleState()
			{
				return m_State;
			}
			ToggleState GetToggleState() const
			{
				return m_State;
			}
			void SetToggleState(ToggleState state)
			{
				m_State = state;
			}
			void SetToggleChecked(bool checked)
			{
				m_State = checked ? ToggleState::Checked : ToggleState::Unchecked;
			}
			void ClearToggleState()
			{
				m_State = ToggleState::None;
			}

			bool HasToggleType() const
			{
				return m_Type != ToggleType::None;
			}
			ToggleType GetToggleType() const
			{
				return m_Type;
			}
			void SetToggleType(ToggleType type)
			{
				m_Type = type;
			}
			void ClearToggleType()
			{
				m_Type = ToggleType::None;
			}

			bool IsToggle3StateAllowed() const
			{
				return m_Allow3State;
			}
			void ToggleAllow3State(bool allow = true)
			{
				m_Allow3State = allow;
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API ToggleRendererBase
	{
		private:
			ToggleValue& m_Value;

		protected:
			bool DoOnActivateCell(const Rect& toggleRect, ToggleState& state, const wxMouseEvent* mouseEvent = nullptr) const;

		public:
			ToggleRendererBase(ToggleValue& value)
				:m_Value(value)
			{
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API ToggleRenderer: public Renderer, public ToggleRendererBase
	{
		private:
			ToggleValue m_Value;

		protected:
			bool HasActivator() const override
			{
				return m_Value.HasToggleType();
			}
			Any OnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent = nullptr) override;

			bool SetDisplayValue(Any value) override;
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			ToggleRenderer(FlagSet<Alignment> alignment = Alignment::CenterVertical|Alignment::CenterHorizontal)
				:Renderer(alignment), ToggleRendererBase(m_Value)
			{
			}

		public:
			String GetDisplayText(Any value) const override
			{
				return {};
			}
	};
}
