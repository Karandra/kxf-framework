#pragma once
#include "Common.h"

namespace kxf
{
	enum class KeyCode: uint32_t
	{
		None = 0,

		Control_A = 1,
		Control_B,
		Control_C,
		Control_D,
		Control_E,
		Control_F,
		Control_G,
		Control_H,
		Control_I,
		Control_J,
		Control_K,
		Control_L,
		Control_M,
		Control_N,
		Control_O,
		Control_P,
		Control_Q,
		Control_R,
		Control_S,
		Control_T,
		Control_U,
		Control_V,
		Control_W,
		Control_X,
		Control_Y,
		Control_Z,

		Backspace = 8,
		Tab = 9,
		Enter = 13,
		Escape = 27,

		// Values from 33 to 126 are reserved for the standard ASCII characters
		Space = 32,
		Delete = 127,

		// Values from 128 to 255 are reserved for ASCII extended characters.
		// There isn't a single fixed standard for the meaning.

		// These are not compatible with Unicode characters.
		// If you want to get a Unicode character from a key event, use 'WidgetKeyEvent::GetUnicodeKey'.
		START = 300,
		LeftButton,
		RightButton,
		Cancel,
		MiddleButton,
		Clear,
		Shift,
		Alt,
		Ctrl,
		Menu,
		Pause,
		Capital,
		End,
		Home,
		Left,
		Up,
		Right,
		Down,
		Select,
		Print,
		Execute,
		Snapshot,
		Insert,
		Help,
		NumPad_0,
		NumPad_1,
		NumPad_2,
		NumPad_3,
		NumPad_4,
		NumPad_5,
		NumPad_6,
		NumPad_7,
		NumPad_8,
		NumPad_9,
		Multiply,
		Add,
		Separator,
		Subtract,
		Decimal,
		Divide,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		NumLock,
		Scroll,
		PageUp,
		PageDown,
		NumPad_Space,
		NumPad_Tab,
		NumPad_Enter,
		NumPad_F1,
		NumPad_F2,
		NumPad_F3,
		NumPad_F4,
		NumPad_Home,
		NumPad_Left,
		NumPad_Up,
		NumPad_Right,
		NumPad_Down,
		NumPad_PageUp,
		NumPad_PageDown,
		NumPad_End,
		NumPad_Begin,
		NumPad_Insert,
		NumPad_Delete,
		NumPad_Equal,
		NumPad_Multiply,
		NumPad_Add,
		NumPad_Separator,
		NumPad_Subtract,
		NumPad_Decimal,
		NumPad_Divide,

		MetaLeft,
		MetaRight,
		MetaMenu,
		Control,

		// Hardware-specific buttons
		Special1 = MetaMenu + 2,
		Special2,
		Special3,
		Special4,
		Special5,
		Special6,
		Special7,
		Special8,
		Special9,
		Special10,
		Special11,
		Special12,
		Special13,
		Special14,
		Special15,
		Special16,
		Special17,
		Special18,
		Special19,
		Special20,

		BrowserBack,
		BrowserForward,
		BrowserRefresh,
		BrowserStop,
		BrowserSearch,
		BrowserFavorites,
		BrowserHome,
		VolumeMute,
		VolumeDown,
		VolumeUp,
		MediaNextTrack,
		MediaPrevTrack,
		MediaStop,
		MediaPlayPause,
		LaunchMail,
		LaunchApp1,
		LaunchApp2
	};

	enum class KeyModifier: uint32_t
	{
		None = 0,

		Alt = FlagSetValue<KeyModifier>(0),
		Ctrl = FlagSetValue<KeyModifier>(1),
		Shift = FlagSetValue<KeyModifier>(2),
		Meta = FlagSetValue<KeyModifier>(3),

		AltGR = Alt|Ctrl
	};
	kxf_FlagSet_Declare(KeyModifier);

	enum class MouseButton: uint32_t
	{
		Any = std::numeric_limits<uint32_t>::max(),
		None = 0,

		Left = FlagSetValue<KeyModifier>(0),
		Right = FlagSetValue<KeyModifier>(1),
		Middle = FlagSetValue<KeyModifier>(2),
		Aux1 = FlagSetValue<KeyModifier>(3),
		Aux2 = FlagSetValue<KeyModifier>(4)
	};
	kxf_FlagSet_Declare(MouseButton);

	enum class KeyCategory: uint32_t
	{
		None = 0,

		Arrow = FlagSetValue<KeyCategory>(0),
		Paging = FlagSetValue<KeyCategory>(1),
		Jump = FlagSetValue<KeyCategory>(2),
		Tab = FlagSetValue<KeyCategory>(3),
		Delete = FlagSetValue<KeyCategory>(4),

		Navigation = Arrow|Paging|Jump
	};
	kxf_FlagSet_Declare(KeyCategory);
}
