#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/textctrl.h>

namespace kxf::UI
{
	enum class TextBoxStyle: uint32_t
	{
		None = 0,

		AlignLeft = wxTE_LEFT,
		AlignRight = wxTE_RIGHT,
		AlignCenter = wxTE_CENTER,

		ProcessEnter = wxTE_PROCESS_ENTER,
		ProcessTab = wxTE_PROCESS_TAB,
		Capitalize = wxTE_CAPITALIZE,
		Multiline = wxTE_MULTILINE,
		Password = wxTE_PASSWORD,
		ReadOnly = wxTE_READONLY,
		Rich = wxTE_RICH,
		Rich2 = wxTE_RICH2,
		AutoURL = wxTE_AUTO_URL,
		NoHideSelection = wxTE_NOHIDESEL,
		NoVScroll = wxTE_NO_VSCROLL,
		DoNotWrap = wxTE_DONTWRAP,
		CharWrap = wxTE_CHARWRAP,
		WordWrap = wxTE_WORDWRAP,
		BestWrap = wxTE_BESTWRAP,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::TextBoxStyle);
}

namespace kxf::UI
{
	class KXF_API TextBox: public WindowRefreshScheduler<wxTextCtrl>
	{
		public:
			static constexpr FlagSet<TextBoxStyle> DefaultStyle = TextBoxStyle::AlignLeft;

		private:
			WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;

		public:
			TextBox() = default;
			TextBox(wxWindow* parent,
					wxWindowID id,
					const String& value = {},
					FlagSet<TextBoxStyle> style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, value, style, validator);
			}
			TextBox(wxWindow* parent,
					wxWindowID id,
					const String& value,
					const Point& pos,
					const Size& size,
					FlagSet<TextBoxStyle> style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, value, pos, size, style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& value = {},
						FlagSet<TextBoxStyle> style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			)
			{
				return Create(parent, id, value, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& value,
						const Point& pos,
						const Size& size,
						FlagSet<TextBoxStyle> style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			);

		public:
			bool SetTabWidth(size_t width = 16);
			void SetLimit(size_t max);
			
			bool SetPasswordChar(const UniChar& c);
			bool SetDefaultPasswordChar()
			{
				// Bold dot
				return SetPasswordChar(0x25CF);
			}
			bool SetHint(const wxString& label) override;
			
			void SetValue(const wxString& value) override;
			void SetValueEvent(const String& value);
			
			void Clear() override;
			void ClearEvent();

			wxDECLARE_DYNAMIC_CLASS(TextBox);
	};
}
