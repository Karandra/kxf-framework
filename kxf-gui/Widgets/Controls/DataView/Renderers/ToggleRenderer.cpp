#include "kxf-pch.h"
#include "ToggleRenderer.h"
#include "../View.h"
#include "../Node.h"
#include "../Column.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIGraphicsContext.h"

namespace kxf::UI::DataView
{
	bool ToggleValue::FromAny(Any& value)
	{
		if (!value.GetAs(*this))
		{
			// Check for 'ToggleState'
			if (ToggleState state = ToggleState::None; value.CheckType<ToggleState>() && value.GetAs(state))
			{
				switch (state)
				{
					case ToggleState::Checked:
					case ToggleState::Unchecked:
					case ToggleState::Indeterminate:
					{
						m_State = state;
						return true;
					}
				};
			}

			// Check for bool
			if (bool isChecked = false; value.CheckType<bool>() && value.GetAs(isChecked))
			{
				m_State = isChecked ? ToggleState::Checked : ToggleState::Unchecked;
				return true;
			}

			// Check for 'ToggleType'
			if (ToggleType type = ToggleType::None; value.CheckType<ToggleType>() && value.GetAs(type))
			{
				switch (type)
				{
					case ToggleType::None:
					case ToggleType::CheckBox:
					case ToggleType::RadioBox:
					{
						m_Type = type;
						return true;
					}
				};
			}
			return false;
		}
		return true;
	}
}

namespace kxf::UI::DataView
{
	bool ToggleRendererBase::DoOnActivateCell(const Rect& toggleRect, ToggleState& state, const wxMouseEvent* mouseEvent) const
	{
		// Only react to clicks directly on the checkbox, not elsewhere in the same cell.
		if (mouseEvent && !toggleRect.Contains(Point(mouseEvent->GetPosition())))
		{
			return false;
		}

		ToggleState nextState = state;
		switch (state)
		{
			case ToggleState::Checked:
			{
				nextState = m_Value.IsToggle3StateAllowed() ? ToggleState::Indeterminate : ToggleState::Unchecked;
				break;
			}
			case ToggleState::Unchecked:
			{
				nextState = ToggleState::Checked;
				break;
			}
			case ToggleState::Indeterminate:
			case ToggleState::None:
			{
				nextState = ToggleState::Unchecked;
				break;
			}
		};

		state = nextState;
		return true;
	}
}

namespace kxf::UI::DataView
{
	Any ToggleRenderer::OnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent)
	{
		ToggleState state = m_Value.GetToggleState();
		if (DoOnActivateCell(GetRenderEngine().GetToggleSize(), state, mouseEvent))
		{
			return state;
		}
		return {};
	}
	bool ToggleRenderer::SetDisplayValue(Any value)
	{
		m_Value = {};
		return m_Value.FromAny(value);
	}

	void ToggleRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		if (m_Value.HasToggleType())
		{
			IGraphicsContext& gc = GetGraphicsContext();
			GetRenderEngine().DrawToggle(gc, cellRect, cellState, m_Value.GetToggleState(), m_Value.GetToggleType());
		}
	}
	Size ToggleRenderer::GetCellSize() const
	{
		if (m_Value.HasToggleType())
		{
			return GetRenderEngine().GetToggleSize();
		}
		return Size(0, 0);
	}
}
