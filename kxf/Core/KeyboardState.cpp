#include "kxf-pch.h"
#include "KeyboardState.h"
#include <wx/kbdstate.h>
#include <wx/utils.h>

namespace kxf
{
	KeyboardState KeyboardState::GetCurrent() noexcept
	{
		return ::wxGetMouseState();
	}

	KeyboardState::KeyboardState(const wxKeyboardState& state) noexcept
	{
		m_Modifiers.Add(KeyModifier::Alt, state.AltDown());
		m_Modifiers.Add(KeyModifier::Ctrl, state.ControlDown());
		m_Modifiers.Add(KeyModifier::Shift, state.ShiftDown());
		m_Modifiers.Add(KeyModifier::Meta, state.MetaDown());
	}
}
