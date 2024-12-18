#include "kxf-pch.h"
#include "WidgetKeyEvent.h"
#include "kxf/Utility/Numeric.h"
#include "kxf/wxWidgets/MapConstants.h"
#include <wx/event.h>

namespace kxf
{
	WidgetKeyEvent::WidgetKeyEvent(IWidget& widget, const wxKeyEvent& keyEventWX) noexcept
		:WidgetEvent(widget), m_KeyboardState(keyEventWX)
	{
		m_Position = {keyEventWX.GetX(), keyEventWX.GetY()};
		m_KeyCode = wxWidgets::MapKeyCode(static_cast<wxKeyCode>(keyEventWX.GetKeyCode()));
		m_UnicodeKey = keyEventWX.GetUnicodeKey();
		m_NativeKeyCode = keyEventWX.GetRawKeyCode();
		m_NativeKeyFlags = keyEventWX.GetRawKeyFlags();
		m_IsNextEventAllowed = keyEventWX.IsNextEventAllowed();

		#if wxCHECK_VERSION(3, 1, 6)
		m_IsAutoRepeat = keyEventWX.IsAutoRepeat();
		#endif
	}
	WidgetKeyEvent::WidgetKeyEvent(IWidget& widget, KeyCode keyCode) noexcept
		:WidgetEvent(widget), m_KeyCode(keyCode)
	{
		// TODO: Add proper conversion, ASCII7 for now.
		if (Utility::TestRange<uint32_t>(ToInt(m_KeyCode), 33, 126))
		{
			m_UnicodeKey = ToInt(m_KeyCode);
		}
	}
	WidgetKeyEvent::WidgetKeyEvent(IWidget& widget, uint32_t nativeKeyCode, FlagSet<uint32_t> nativeKeyFlags) noexcept
		:WidgetEvent(widget), m_NativeKeyCode(nativeKeyCode), m_NativeKeyFlags(nativeKeyFlags)
	{
		// TODO: Add native key event mapping.
		// wParam: m_NativeKeyCode, lParam: m_NativeKeyFlags.
	}

	bool WidgetKeyEvent::IsKeyInCategory(FlagSet<KeyCategory> category) const noexcept
	{
		if (category.IsNull())
		{
			return false;
		}

		switch (m_KeyCode)
		{
			case KeyCode::Left:
			case KeyCode::Right:
			case KeyCode::Up:
			case KeyCode::Down:
			case KeyCode::NumPad_Left:
			case KeyCode::NumPad_Right:
			case KeyCode::NumPad_Up:
			case KeyCode::NumPad_Down:
			{
				return category.Contains(KeyCategory::Arrow);
			}

			case KeyCode::PageDown:
			case KeyCode::PageUp:
			case KeyCode::NumPad_PageUp:
			case KeyCode::NumPad_PageDown:
			{
				return category.Contains(KeyCategory::Paging);
			}

			case KeyCode::Home:
			case KeyCode::End:
			case KeyCode::NumPad_Home:
			case KeyCode::NumPad_End:
			{
				return category.Contains(KeyCategory::Jump);
			}

			case KeyCode::Tab:
			case KeyCode::NumPad_Tab:
			{
				return category.Contains(KeyCategory::Tab);
			}

			case KeyCode::Delete:
			case KeyCode::Backspace:
			case KeyCode::NumPad_Delete:
			{
				return category.Contains(KeyCategory::Delete);
			}
		};
		return false;
	}
}
