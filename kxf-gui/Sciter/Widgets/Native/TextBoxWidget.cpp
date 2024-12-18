#include "kxf-pch.h"
#include "TextBoxWidget.h"

namespace
{
	constexpr std::wstring_view g_Stylesheet =
		#include "TextBoxWidget.css"
		;
}

namespace kxf::Sciter
{
	void NativeTextBoxWidget::OnAttached()
	{
		using namespace UI;

		Element node = GetElement();

		// Styles
		FlagSet<TextBoxStyle> widgetStyle;
		widgetStyle.Add(TextBoxStyle::Capitalize, node.GetStyleAttribute("text-transform") == "capitalize");
		widgetStyle.Add(TextBoxStyle::ProcessEnter, node.HasStyleAttribute("native-process-enter"));
		widgetStyle.Add(TextBoxStyle::ProcessTab, node.HasStyleAttribute("native-process-tab"));
		widgetStyle.Add(TextBoxStyle::Multiline, node.GetStyleAttribute("native-text-multiline") == "auto");
		widgetStyle.Add(TextBoxStyle::NoVScroll, node.GetStyleAttribute("native-scrollbar-vertical") == "none");
		widgetStyle.Add(TextBoxStyle::AutoURL, node.GetStyleAttribute("native-text-url") == "auto");

		String textAlign = node.GetStyleAttribute("text-align");
		widgetStyle.Add(TextBoxStyle::AlignLeft, textAlign.IsEmpty() || textAlign == "left");
		widgetStyle.Add(TextBoxStyle::AlignRight, textAlign == "right");
		widgetStyle.Add(TextBoxStyle::AlignCenter, textAlign == "AlignCenter");

		String textWrap = node.GetStyleAttribute("native-text-wrap");
		widgetStyle.Add(TextBoxStyle::BestWrap, textWrap.IsEmpty() || textWrap == "auto");
		widgetStyle.Add(TextBoxStyle::CharWrap, textWrap == "char");
		widgetStyle.Add(TextBoxStyle::WordWrap, textWrap == "word");
		widgetStyle.Add(TextBoxStyle::DoNotWrap, textWrap == "none");

		String technology = node.GetStyleAttribute("native-technology");
		widgetStyle.Add(TextBoxStyle::Rich, technology == "rich");
		widgetStyle.Add(TextBoxStyle::Rich2, textWrap == "rich2");
		
		if (TextBox::Create(GetAttachmentParent(), wxID_NONE, node.GetText(), widgetStyle))
		{
			NativeWidget::OnAttached();
			TextBox::Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
			{
				GetElement().SetText(event.GetString());
				event.Skip();
			});

			if (auto value = node.GetStyleAttributeInt("tab-size"); value && *value >= 0)
			{
				TextBox::SetTabWidth(*value);
			}
			if (auto value = node.GetStyleAttributeInt("native-text-limit"); value && *value >= 0)
			{
				TextBox::SetLimit(*value);
			}
			if (String value = node.GetAttribute("hint"); !value.IsEmpty())
			{
				TextBox::SetHint(value);
			}

			String password = node.GetStyleAttribute("native-password-mask");
			if (password == "auto")
			{
				TextBox::SetDefaultPasswordChar();
			}
			else if (password != "none")
			{
				TextBox::SetPasswordChar(password.front());
			}
		}
	}
	void NativeTextBoxWidget::ChangeValue(const wxString& value)
	{
		TextBox::ChangeValue(value);
		GetElement().SetText(StringViewOf(value));
	}
}

namespace kxf::Sciter
{
	String NativeTextBoxWidgetFactory::GetWidgetStylesheet() const
	{
		return g_Stylesheet;
	}
}
