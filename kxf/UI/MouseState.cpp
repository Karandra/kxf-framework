#include "kxf-pch.h"
#include "MouseState.h"
#include <wx/mousestate.h>
#include <wx/utils.h>

namespace kxf
{
	MouseState MouseState::GetCurrent() noexcept
	{
		return ::wxGetMouseState();
	}

	MouseState::MouseState(const wxMouseState& state) noexcept
		:m_Position({state.GetX(), state.GetY()})
	{
		m_Buttons.Add(MouseButton::Left, state.LeftIsDown());
		m_Buttons.Add(MouseButton::Right, state.RightIsDown());
		m_Buttons.Add(MouseButton::Middle, state.MiddleIsDown());
		m_Buttons.Add(MouseButton::Aux1, state.Aux1IsDown());
		m_Buttons.Add(MouseButton::Aux2, state.Aux2IsDown());
	}
}
