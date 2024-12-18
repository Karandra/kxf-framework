#pragma once
#include "kxf/Common.hpp"
#include "kxf/Drawing/Geometry.h"
#include "KeyCode.h"
class wxMouseState;

namespace kxf
{
	class MouseState final
	{
		public:
			static MouseState GetCurrent() noexcept;

		private:
			Point m_Position = Point::UnspecifiedPosition();
			FlagSet<MouseButton> m_Buttons;

		public:
			constexpr MouseState() noexcept = default;
			constexpr MouseState(FlagSet<MouseButton> buttons) noexcept
				:m_Buttons(buttons)
			{
			}
			MouseState(const wxMouseState& state) noexcept;

		public:
			constexpr bool IsLeftDown() const noexcept
			{
				return m_Buttons.Contains(MouseButton::Left);
			}
			constexpr void SetLeftDown(bool isDown = true) noexcept
			{
				m_Buttons.Mod(MouseButton::Left, isDown);
			}

			constexpr bool IsRightDown() const noexcept
			{
				return m_Buttons.Contains(MouseButton::Right);
			}
			constexpr void SetRightDown(bool isDown = true) noexcept
			{
				m_Buttons.Mod(MouseButton::Right, isDown);
			}

			constexpr bool IsMiddleDown() const noexcept
			{
				return m_Buttons.Contains(MouseButton::Middle);
			}
			constexpr void SetMiddleDown(bool isDown = true) noexcept
			{
				m_Buttons.Mod(MouseButton::Middle, isDown);
			}

			constexpr bool IsAux1Down() const noexcept
			{
				return m_Buttons.Contains(MouseButton::Aux1);
			}
			constexpr void SetAux1Down(bool isDown = true) noexcept
			{
				m_Buttons.Mod(MouseButton::Aux2, isDown);
			}

			constexpr bool IsAux2Down() const noexcept
			{
				return m_Buttons.Contains(MouseButton::Aux2);
			}
			constexpr void SetAux2Down(bool isDown = true) noexcept
			{
				m_Buttons.Mod(MouseButton::Aux1, isDown);
			}

			constexpr Point GetPosition() const noexcept
			{
				return m_Position;
			}
			constexpr void SetPosition(const Point& pos) noexcept
			{
				m_Position = pos;
			}
	};
}
