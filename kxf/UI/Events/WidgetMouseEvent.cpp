#include "kxf-pch.h"
#include "WidgetMouseEvent.h"
#include "kxf/Utility/Numeric.h"
#include <wx/event.h>

namespace kxf
{
	WidgetMouseEvent::WidgetMouseEvent(IWidget& widget, const wxMouseEvent& mouseEventWX) noexcept
		:WidgetEvent(widget), m_KeyboardState(mouseEventWX), m_MouseState(mouseEventWX)
	{
		m_ClickCount = mouseEventWX.GetClickCount();
		m_LinesPerAction = mouseEventWX.GetLinesPerAction();
		m_ColumnsPerAction = mouseEventWX.GetColumnsPerAction();
		m_Magnification = mouseEventWX.GetMagnification();

		m_WheelDelta = mouseEventWX.GetWheelDelta();
		m_WheelRotation = mouseEventWX.GetWheelRotation();
		m_WheelInverted = mouseEventWX.IsWheelInverted();
		m_WheelAxis = mouseEventWX.GetWheelAxis() == wxMOUSE_WHEEL_HORIZONTAL ? Orientation::Horizontal : Orientation::Vertical;
	}

	bool WidgetMouseEvent::IsButtonUp(MouseButton button) const
	{
		switch (button)
		{
			case MouseButton::Any:
			{
				return IsLeftUp() || IsRightUp() || IsMiddleUp() || IsAux1Up() || IsAux2Up();
			}
			case MouseButton::Left:
			{
				return IsLeftUp();
			}
			case MouseButton::Right:
			{
				return IsRightUp();
			}
			case MouseButton::Middle:
			{
				return IsMiddleUp();
			}
			case MouseButton::Aux1:
			{
				return IsAux1Up();
			}
			case MouseButton::Aux2:
			{
				return IsAux2Up();
			}
		};
		return false;
	}
	bool WidgetMouseEvent::IsButtonDown(MouseButton button) const
	{
		switch (button)
		{
			case MouseButton::Any:
			{
				return IsLeftDown() || IsRightDown() || IsMiddleDown() || IsAux1Down() || IsAux2Down();
			}
			case MouseButton::Left:
			{
				return IsLeftDown();
			}
			case MouseButton::Right:
			{
				return IsRightDown();
			}
			case MouseButton::Middle:
			{
				return IsMiddleDown();
			}
			case MouseButton::Aux1:
			{
				return IsAux1Down();
			}
			case MouseButton::Aux2:
			{
				return IsAux2Down();
			}
		};
		return false;
	}
	bool WidgetMouseEvent::IsDoubleClick(MouseButton button) const
	{
		switch (button)
		{
			case MouseButton::Any:
			{
				return IsLeftDoubleClick() || IsRightDoubleClick() || IsMiddleDoubleClick() || IsAux1DoubleClick() || IsAux2DoubleClick();
			}
			case MouseButton::Left:
			{
				return IsLeftDoubleClick();
			}
			case MouseButton::Right:
			{
				return IsRightDoubleClick();
			}
			case MouseButton::Middle:
			{
				return IsMiddleDoubleClick();
			}
			case MouseButton::Aux1:
			{
				return IsAux1DoubleClick();
			}
			case MouseButton::Aux2:
			{
				return IsAux2DoubleClick();
			}
		};
		return false;
	}

	bool WidgetMouseEvent::IsButton(MouseButton button) const noexcept
	{
		switch (button)
		{
			case MouseButton::Left:
			{
				return IsLeftDown() || IsLeftUp() || IsLeftDoubleClick();
			}
			case MouseButton::Right:
			{
				return IsRightDown() || IsRightUp() || IsRightDoubleClick();
			}
			case MouseButton::Middle:
			{
				return IsMiddleDown() || IsMiddleUp() || IsMiddleDoubleClick();
			}
			case MouseButton::Aux1:
			{
				return IsAux1Down() || IsAux1Up() || IsAux1DoubleClick();
			}
			case MouseButton::Aux2:
			{
				return IsAux2Down() || IsAux2Up() || IsAux2DoubleClick();
			}
			case MouseButton::Any:
			{
				return IsButtonUp(MouseButton::Any) || IsButtonDown(MouseButton::Any) || IsDoubleClick(MouseButton::Any);
			}
		};
		return false;
	}
	MouseButton WidgetMouseEvent::GetButton() const noexcept
	{
		constexpr MouseButton buttons[] =
		{
			MouseButton::Left,
			MouseButton::Right,
			MouseButton::Middle,
			MouseButton::Aux1,
			MouseButton::Aux2,
			MouseButton::Any
		};

		for (const auto& button: buttons)
		{
			if (IsButton(button))
			{
				return button;
			}
		}
		return MouseButton::None;
	}
}
