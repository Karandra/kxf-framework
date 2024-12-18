#include "kxf-pch.h"
#include "WxTextCtrlWrapper.h"
#include <wx/textctrl.h>
#include <CommCtrl.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::WXUI::Private
{
	// WxTextEntryWrapper
	void WxTextCtrlWrapper::Initialize(wxTextCtrl& textCtrl) noexcept
	{
		m_TextCtrl = &textCtrl;
		WxTextEntryWrapper::Initialize(textCtrl);
	}

	size_t WxTextCtrlWrapper::GetLengthLimit() const
	{
		return ::SendMessageW(m_TextCtrl->GetHandle(), EM_GETLIMITTEXT, 0, 0);
	}
	void WxTextCtrlWrapper::SetLengthLimit(size_t limit)
	{
		::SendMessageW(m_TextCtrl->GetHandle(), EM_SETLIMITTEXT, static_cast<WPARAM>(limit), 0);
		WxTextEntryWrapper::SetLengthLimit(limit);
	}

	size_t WxTextCtrlWrapper::GetTabWidth() const
	{
		return WxTextEntryWrapper::GetTabWidth();
	}
	void WxTextCtrlWrapper::SetTabWidth(size_t width)
	{
		const auto value = static_cast<unsigned int>(width);
		::SendMessageW(m_TextCtrl->GetHandle(), EM_SETTABSTOPS, 1, reinterpret_cast<LPARAM>(&value));

		WxTextEntryWrapper::SetTabWidth(width);
	}

	String WxTextCtrlWrapper::GetHint() const
	{
		wchar_t buffer[std::numeric_limits<int16_t>::max()] = {};
		if (::SendMessageW(m_TextCtrl->GetHandle(), EM_GETCUEBANNER, reinterpret_cast<WPARAM>(buffer), static_cast<LPARAM>(std::size(buffer))))
		{
			return buffer;
		}
		return {};
	}
	void WxTextCtrlWrapper::SetHint(const String& hint)
	{
		m_TextCtrl->SetHint(hint);
		::SendMessageW(m_TextCtrl->GetHandle(), EM_SETCUEBANNER, TRUE, hint.wc_str().unsafe_intptr());
	}

	// WxTextCtrlWrapper
	bool WxTextCtrlWrapper::IsMultiline() const
	{
		return m_TextCtrl->IsMultiLine();
	}
	void WxTextCtrlWrapper::SetMultiline(bool isMultiline)
	{
		FlagSet style = m_TextCtrl->GetWindowStyle();
		style.Mod(wxTE_MULTILINE, isMultiline);

		m_TextCtrl->SetWindowStyle(*style);
	}

	size_t WxTextCtrlWrapper::GetRowCount() const
	{
		return m_TextCtrl->GetNumberOfLines();
	}
	size_t WxTextCtrlWrapper::GetColumnCount() const
	{
		size_t result = 0;

		const size_t rowCount = m_TextCtrl->GetNumberOfLines();
		for (size_t i = 0; i < rowCount; i++)
		{
			size_t length = m_TextCtrl->GetLineLength(i);
			if (length > result)
			{
				result = length;
			}
		}
		return result;
	}
	size_t WxTextCtrlWrapper::GetRowLength(size_t index) const
	{
		return m_TextCtrl->GetLineLength(index);
	}

	UniChar WxTextCtrlWrapper::GetPasswordMask() const
	{
		return ::SendMessageW(m_TextCtrl->GetHandle(), EM_GETPASSWORDCHAR, 0, 0);
	}
	void WxTextCtrlWrapper::SetPasswordMask(const UniChar& c) const
	{
		if (c)
		{
			::SendMessageW(m_TextCtrl->GetHandle(), EM_SETPASSWORDCHAR, c.GetValue(), 0);
		}
	}
	void WxTextCtrlWrapper::RemovePasswordMask() const
	{
		::SendMessageW(m_TextCtrl->GetHandle(), EM_SETPASSWORDCHAR, 0, 0);
	}

	void WxTextCtrlWrapper::EnsurePositionVisible(size_t pos)
	{
		if (pos != npos)
		{
			m_TextCtrl->ShowPosition(pos);
		}
	}
	Point WxTextCtrlWrapper::MapPositionToCoordinates(size_t pos) const
	{
		return pos != npos ? Point(m_TextCtrl->PositionToCoords(pos)) : Point::UnspecifiedPosition();
	}
	size_t WxTextCtrlWrapper::MapCoordinatesToPosition(const Point& point) const
	{
		long pos = -1;
		if (m_TextCtrl->HitTest(point, &pos) == wxTE_HT_ON_TEXT && pos >= 0)
		{
			return pos;
		}
		return npos;
	}

	Point WxTextCtrlWrapper::MapPositionToRowColumn(size_t pos) const
	{
		if (pos != npos)
		{
			long x = -1;
			long y = -1;
			m_TextCtrl->PositionToXY(pos, &x, &y);

			return Point(x, y);
		}
		return Point::UnspecifiedPosition();
	}
	size_t WxTextCtrlWrapper::MapRowColumnToPosition(const Point& rowColumn) const
	{
		auto pos = m_TextCtrl->XYToPosition(rowColumn.GetX(), rowColumn.GetY());
		return pos >= 0 ? pos : npos;
	}

	TextWrapMode WxTextCtrlWrapper::GetWrapMode() const
	{
		const FlagSet style = m_TextCtrl->GetWindowStyle();

		if (style.Contains(wxTE_WORDWRAP))
		{
			return TextWrapMode::Word;
		}
		else if (style.Contains(wxTE_CHARWRAP))
		{
			return TextWrapMode::Character;
		}
		return TextWrapMode::None;
	}
	void WxTextCtrlWrapper::SetWrapMode(TextWrapMode wrapMode)
	{
		FlagSet style = m_TextCtrl->GetWindowStyle();
		style.Remove(wxTE_WORDWRAP);
		style.Remove(wxTE_CHARWRAP);
		style.Remove(wxTE_DONTWRAP);

		style.Add(wxTE_DONTWRAP, wrapMode == TextWrapMode::None);
		style.Add(wxTE_WORDWRAP, wrapMode == TextWrapMode::Word || wrapMode == TextWrapMode::Whitespace);
		style.Add(wxTE_CHARWRAP, wrapMode == TextWrapMode::Character);

		m_TextCtrl->SetWindowStyle(*style);
	}
}
