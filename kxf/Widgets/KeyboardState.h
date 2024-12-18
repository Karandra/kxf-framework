#pragma once
#include "kxf/Common.hpp"
#include "KeyCode.h"
class wxKeyboardState;

namespace kxf
{
	class KeyboardState final
	{
		public:
			static KeyboardState GetCurrent() noexcept;

		private:
			FlagSet<KeyModifier> m_Modifiers;

		public:
			constexpr KeyboardState() noexcept = default;
			constexpr KeyboardState(FlagSet<KeyModifier> modifiers) noexcept
				:m_Modifiers(modifiers)
			{
			}
			KeyboardState(const wxKeyboardState& state) noexcept;

		public:
			constexpr bool IsAltDown() const noexcept
			{
				return m_Modifiers.Contains(KeyModifier::Alt);
			}
			constexpr void SetAltDown(bool isDown = true) noexcept
			{
				m_Modifiers.Mod(KeyModifier::Alt, isDown);
			}

			constexpr bool IsCtrlDown() const noexcept
			{
				return m_Modifiers.Contains(KeyModifier::Ctrl);
			}
			constexpr void SetCtrlDown(bool isDown = true) noexcept
			{
				m_Modifiers.Mod(KeyModifier::Ctrl, isDown);
			}

			constexpr bool IsShiftDown() const noexcept
			{
				return m_Modifiers.Contains(KeyModifier::Shift);
			}
			constexpr void SetShiftDown(bool isDown = true) noexcept
			{
				m_Modifiers.Mod(KeyModifier::Shift, isDown);
			}

			constexpr bool IsMetaDown() const noexcept
			{
				return m_Modifiers.Contains(KeyModifier::Meta);
			}
			constexpr void SetMetaDown(bool isDown = true) noexcept
			{
				m_Modifiers.Mod(KeyModifier::Meta, isDown);
			}

			constexpr bool HasModifiers() const noexcept
			{
				return IsAltDown() || IsCtrlDown();
			}
			constexpr bool HasAnyModifiers() const noexcept
			{
				return !m_Modifiers.IsNull();
			}
	};
}
