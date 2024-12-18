#include "kxf-pch.h"
#include "StyledTextBox.h"
#include "WXUI/StyledTextBox.h"

namespace kxf::Widgets
{
	// ComboBox
	StyledTextBox::StyledTextBox()
	{
		InitializeWxWidget();
	}
	StyledTextBox::~StyledTextBox() = default;

	// IWidget
	bool StyledTextBox::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent && Get()->Create(parent->GetWxWindow(), label, pos, size))
		{
			return true;
		}
		return false;
	}

	// ITextEntry
	bool StyledTextBox::CanCut() const
	{
		return Get()->CanCut();
	}
	bool StyledTextBox::CanCopy() const
	{
		return Get()->CanCopy();
	}
	bool StyledTextBox::CanPaste() const
	{
		return Get()->CanPaste();
	}
	bool StyledTextBox::CanUndo() const
	{
		return Get()->CanUndo();
	}
	bool StyledTextBox::CanRedo() const
	{
		return Get()->CanRedo();
	}

	void StyledTextBox::Cut()
	{
		Get()->Cut();
	}
	void StyledTextBox::Copy()
	{
		Get()->Undo();
	}
	void StyledTextBox::Paste()
	{
		Get()->Undo();
	}
	void StyledTextBox::Undo()
	{
		Get()->Undo();
	}
	void StyledTextBox::Redo()
	{
		Get()->Redo();
	}

	void StyledTextBox::ClearText()
	{
		Get()->ClearAll();
	}
	bool StyledTextBox::IsTextEmpty() const
	{
		return Get()->IsEmpty();
	}

	bool StyledTextBox::IsEditable() const
	{
		return Get()->IsEditable();
	}
	void StyledTextBox::SetEditable(bool isEditable)
	{
		Get()->SetEditable(isEditable);
	}

	bool StyledTextBox::IsMultiline() const
	{
		return true;
	}
	void StyledTextBox::SetMultiline(bool isMultiline)
	{
	}

	bool StyledTextBox::IsModified() const
	{
		return Get()->IsModified();
	}
	void StyledTextBox::SetModified(bool isModified)
	{
		Get()->SetModified(isModified);
	}

	size_t StyledTextBox::GetLengthLimit() const
	{
		return m_LengthLimit;
	}
	void StyledTextBox::SetLengthLimit(size_t limit)
	{
		m_LengthLimit = limit;
		Get()->SetMaxLength(limit);
	}

	size_t StyledTextBox::GetTabWidth() const
	{
		return Get()->GetTabWidth();
	}
	void StyledTextBox::SetTabWidth(size_t width)
	{
		Get()->SetTabWidth(width);
	}

	size_t StyledTextBox::GetInsertionPoint() const
	{
		return Get()->GetInsertionPoint();
	}
	void StyledTextBox::SetInsertionPoint(size_t pos)
	{
		Get()->SetInsertionPoint(pos);
	}

	void StyledTextBox::SelectRange(size_t from, size_t to)
	{
		if (from == to || from == npos)
		{
			Get()->SelectNone();
		}
		else if (from == 0 && to == npos)
		{
			Get()->SelectAll();
		}
		else
		{
			Get()->SetSelection(from, to);
		}
	}
	std::pair<size_t, size_t> StyledTextBox::GetSelectionRange() const
	{
		return {static_cast<size_t>(Get()->GetSelectionStart()), static_cast<size_t>(Get()->GetSelectionEnd())};
	}
	String StyledTextBox::GetRange(size_t from, size_t to) const
	{
		if (from != npos)
		{
			return Get()->GetRange(from, to != npos ? to : -1);
		}
		return {};
	}

	void StyledTextBox::RemoveRange(size_t from, size_t to)
	{
		if (from != npos)
		{
			return Get()->Remove(from, to != npos ? to : -1);
		}
	}
	void StyledTextBox::ReplaceRange(size_t from, size_t to, const String& text)
	{
		if (from != npos)
		{
			return Get()->Replace(from, to != npos ? to : -1, text);
		}
	}

	size_t StyledTextBox::GetTextLength() const
	{
		return Get()->GetTextLength();
	}
	String StyledTextBox::GetText() const
	{
		return Get()->GetText();
	}
	void StyledTextBox::SetText(const String& text)
	{
		Get()->ChangeValue(text);
	}
	void StyledTextBox::AppendText(const String& text)
	{
		Get()->AppendText(text);
	}

	String StyledTextBox::GetHint() const
	{
		return Get()->GetHint();
	}
	void StyledTextBox::SetHint(const String& hint)
	{
		Get()->SetHint(hint);
	}

	size_t StyledTextBox::GetRowCount() const
	{
		return Get()->GetLineCount();
	}
	size_t StyledTextBox::GetColumnCount() const
	{
		size_t result = 0;

		const size_t rowCount = Get()->GetLineCount();
		for (size_t i = 0; i < rowCount; i++)
		{
			size_t length = Get()->GetLineLength(i);
			if (length > result)
			{
				result = length;
			}
		}
		return result;
	}
	size_t StyledTextBox::GetRowLength(size_t index) const
	{
		return index != npos ? Get()->GetLineLength(index) : 0;
	}

	UniChar StyledTextBox::GetPasswordMask() const
	{
		return {};
	}
	void StyledTextBox::SetPasswordMask(const UniChar& c) const
	{
	}
	void StyledTextBox::RemovePasswordMask() const
	{
	}

	// ITextWidget
	void StyledTextBox::EnsurePositionVisible(size_t pos)
	{
		if (pos != npos)
		{
			Get()->ShowPosition(pos);
		}
	}

	Point StyledTextBox::MapPositionToCoordinates(size_t pos) const
	{
		return pos != npos ? Point(Get()->PositionToCoords(pos)) : Point::UnspecifiedPosition();
	}
	size_t StyledTextBox::MapCoordinatesToPosition(const Point& point) const
	{
		auto pos = Get()->PositionFromPoint(point);
		return pos >= 0 ? pos : npos;
	}

	Point StyledTextBox::MapPositionToRowColumn(size_t pos) const
	{
		if (pos != npos)
		{
			long x = -1;
			long y = -1;
			Get()->PositionToXY(pos, &x, &y);

			return Point(x, y);
		}
		return Point::UnspecifiedPosition();
	}
	size_t StyledTextBox::MapRowColumnToPosition(const Point& rowColumn) const
	{
		auto pos = Get()->XYToPosition(rowColumn.GetX(), rowColumn.GetY());
		return pos >= 0 ? pos : npos;
	}

	TextWrapMode StyledTextBox::GetWrapMode() const
	{
		switch (Get()->GetWrapMode())
		{
			case wxSTC_WRAP_WORD:
			{
				return TextWrapMode::Word;
			}
			case wxSTC_WRAP_CHAR:
			{
				return TextWrapMode::Character;
			}
			case wxSTC_WRAP_WHITESPACE:
			{
				return TextWrapMode::Whitespace;
			}
		};
		return TextWrapMode::None;
	}
	void StyledTextBox::SetWrapMode(TextWrapMode wrapMode)
	{
		switch (wrapMode)
		{
			case TextWrapMode::Word:
			{
				Get()->SetWrapMode(wxSTC_WRAP_WORD);
				break;
			}
			case TextWrapMode::Character:
			{
				Get()->SetWrapMode(wxSTC_WRAP_CHAR);
				break;
			}
			case TextWrapMode::Whitespace:
			{
				Get()->SetWrapMode(wxSTC_WRAP_WHITESPACE);
				break;
			}
			default:
			{
				Get()->SetWrapMode(wxSTC_WRAP_NONE);
				break;
			}
		};
	}

	// IStyledTextWidget
	bool StyledTextBox::IsIndentationGuidesVisible() const
	{
		return Get()->GetIndentationGuides() == wxSTC_IV_REAL;
	}
	void StyledTextBox::SetIndentationGuidesVisible(bool isVisible)
	{
		Get()->SetIndentationGuides(isVisible ? wxSTC_IV_REAL : wxSTC_IV_NONE);
	}
}
