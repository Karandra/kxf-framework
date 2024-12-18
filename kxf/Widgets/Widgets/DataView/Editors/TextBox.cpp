#include "kxf-pch.h"
#include "TextBox.h"
#include "kxf/Widgets/Widgets/TextBox.h"

namespace kxf::DataView
{
	// IDataViewCellEditor
	std::shared_ptr<IWidget> TextBox::CreateWidget(std::shared_ptr<IWidget> parent, const EditorInfo& editorInfo)
	{
		m_Widget = NewWidget<Widgets::TextBox>(parent);
		return m_Widget;
	}

	bool TextBox::BeginEdit(const EditorInfo& editorInfo, Any value)
	{
		if (m_Widget)
		{
			auto text = std::move(value).GetAs<String>();
			m_Widget->SetText(text);

			// Adjust size of the editor to fit text, even if it means being
			// wider than the corresponding column (this is how Windows Explorer behaves).
			auto textExtent = m_Widget->GetTextExtent(text);

			// Adjust size so that it fits all content. Don't change anything if the allocated
			// space is already larger than needed and don't exceed the widget boundaries.
			auto width = std::max(editorInfo.CellRect.GetWidth(), textExtent.GetWidth());
			auto height = std::max(editorInfo.CellRect.GetHeight(), textExtent.GetHeight());
			m_Widget->SetSize({width, height}, WidgetSizeFlag::Widget|WidgetSizeFlag::WidgetMax|WidgetSizeFlag::WidgetMin);

			// Select the text in the control and place the cursor at the end
			m_Widget->SetEditable(editorInfo.IsEditable);
			m_Widget->SetInsertionPoint(ITextWidget::npos);
			m_Widget->SelectAll();
			return true;
		}
		return false;
	}
	Any TextBox::GetValue(const EditorInfo& editorInfo) const
	{
		if (m_Widget)
		{
			return m_Widget->GetText();
		}
		return {};
	}
}
