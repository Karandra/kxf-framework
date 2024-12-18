#include "kxf-pch.h"
#include "TextEditor.h"
#include "../Renderers/TextRenderer.h"
#include "kxf/UI/Controls/TextBox.h"

namespace kxf::UI::DataView
{
	wxWindow* TextEditor::CreateControl(wxWindow& parent, const Rect& cellRect, Any value)
	{
		const FlagSet<TextBoxStyle> style = TextBox::DefaultStyle|TextBoxStyle::ProcessEnter|(IsEditable() ? TextBoxStyle::None : TextBoxStyle::ReadOnly);
		const TextValue textValue = Renderer::FromAnyUsing<TextValue>(value);

		TextBox* editor = new TextBox(&parent,
									  wxID_NONE,
									  textValue.GetText(),
									  cellRect.GetPosition(),
									  cellRect.GetSize(),
									  style,
									  GetValidator()
		);

		// Adjust size of 'KxTextBox' editor to fit text, even if it means being
		// wider than the corresponding column (this is how Explorer behaves).
		const int fittingWidth = editor->GetSizeFromTextSize(editor->GetTextExtent(editor->GetValue())).GetX();
		const int currentWidth = editor->GetSize().GetX();
		const int maxWidth = editor->GetParent()->GetSize().x - editor->GetPosition().x;

		// Adjust size so that it fits all content. Don't change anything if the allocated
		// space is already larger than needed and don't extend wxDVC's boundaries.
		const int width = std::min(std::max(currentWidth, fittingWidth), maxWidth);
		if (width != currentWidth)
		{
			editor->SetSize(Size(width, -1));
		}

		// Select the text in the control and place the cursor at the end
		editor->SetInsertionPointEnd();
		editor->SelectAll();
		return editor;
	}
	Any TextEditor::GetValue(wxWindow& control) const
	{
		return String(static_cast<TextBox&>(control).GetValue());
	}
}
