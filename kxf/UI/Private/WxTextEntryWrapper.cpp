#include "kxf-pch.h"
#include "WxTextEntryWrapper.h"
#include <wx/textentry.h>

#define DispatchCall(ret, name, ...)	\
if (m_TextEntry)	\
{	\
	return m_TextEntry->name(__VA_ARGS__);	\
}	\
else if (m_TextInterface)	\
{	\
	return m_TextInterface->name(__VA_ARGS__);	\
}	\
return ret;

namespace kxf::WXUI::Private
{
	void WxTextEntryWrapper::Initialize(wxTextEntry& textEntry) noexcept
	{
		m_TextEntry = &textEntry;
		m_TextInterface = dynamic_cast<wxTextCtrlIface*>(&textEntry);

		if (auto evtHandler = dynamic_cast<wxEvtHandler*>(&textEntry))
		{
			evtHandler->Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
			{
				m_IsModified = true;
				event.Skip();
			});
		}
	}
	void WxTextEntryWrapper::Initialize(wxTextCtrlIface& textInterface) noexcept
	{
		m_TextEntry = dynamic_cast<wxTextEntry*>(&textInterface);
		m_TextInterface = &textInterface;

		if (auto evtHandler = dynamic_cast<wxEvtHandler*>(&textInterface))
		{
			evtHandler->Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
			{
				m_IsModified = true;
				event.Skip();
			});
		}
	}

	bool WxTextEntryWrapper::CanCut() const
	{
		DispatchCall(false, CanCut);
	}
	bool WxTextEntryWrapper::CanCopy() const
	{
		DispatchCall(false, CanCopy);
	}
	bool WxTextEntryWrapper::CanPaste() const
	{
		DispatchCall(false, CanPaste);
	}
	bool WxTextEntryWrapper::CanUndo() const
	{
		DispatchCall(false, CanRedo);
	}
	bool WxTextEntryWrapper::CanRedo() const
	{
		DispatchCall(false, CanRedo);
	}

	void WxTextEntryWrapper::Cut()
	{
		DispatchCall(, Cut);
	}
	void WxTextEntryWrapper::Copy()
	{
		DispatchCall(, Copy);
	}
	void WxTextEntryWrapper::Paste()
	{
		DispatchCall(, Paste);
	}
	void WxTextEntryWrapper::Undo()
	{
		DispatchCall(, Undo);
	}
	void WxTextEntryWrapper::Redo()
	{
		DispatchCall(, Redo);
	}

	void WxTextEntryWrapper::ClearText()
	{
		DispatchCall(, ChangeValue, {});
	}
	bool WxTextEntryWrapper::IsTextEmpty() const
	{
		DispatchCall(false, IsEmpty);
	}

	bool WxTextEntryWrapper::IsEditable() const
	{
		DispatchCall(false, IsEditable);
	}
	void WxTextEntryWrapper::SetEditable(bool isEditable)
	{
		DispatchCall(, SetEditable, isEditable);
	}

	bool WxTextEntryWrapper::IsModified() const
	{
		return m_IsModified;
	}
	void WxTextEntryWrapper::SetModified(bool isModified)
	{
		m_IsModified = isModified;
	}

	size_t WxTextEntryWrapper::GetLengthLimit() const
	{
		return m_LengthLimit;
	}
	void WxTextEntryWrapper::SetLengthLimit(size_t limit)
	{
		m_LengthLimit = limit;
		DispatchCall(, SetMaxLength, limit);
	}

	size_t WxTextEntryWrapper::GetTabWidth() const
	{
		return m_TabWidth;
	}
	void WxTextEntryWrapper::SetTabWidth(size_t width)
	{
		m_TabWidth = width;
	}

	size_t WxTextEntryWrapper::GetInsertionPoint() const
	{
		DispatchCall(npos, GetInsertionPoint);
	}
	void WxTextEntryWrapper::SetInsertionPoint(size_t pos)
	{
		DispatchCall(, SetInsertionPoint, pos != npos ? pos : -1);
	}

	void WxTextEntryWrapper::SelectRange(size_t from, size_t to)
	{
		if (from == to || from == npos)
		{
			DispatchCall(, SelectNone);
		}
		else if (from == 0 && to == npos)
		{
			DispatchCall(, SelectAll);
			m_TextEntry->SelectAll();
		}
		else
		{
			DispatchCall(, SetSelection, from, to);
		}
	}
	std::pair<size_t, size_t> WxTextEntryWrapper::GetSelectionRange() const
	{
		long from = 0;
		long to = 0;
		[&]()
		{
			DispatchCall(, GetSelection, &from, &to);
		}();

		return {static_cast<size_t>(from), static_cast<size_t>(to)};
	}
	String WxTextEntryWrapper::GetRange(size_t from, size_t to) const
	{
		if (from != npos)
		{
			DispatchCall({}, GetRange, from, to != npos ? to : -1);
		}
		return {};
	}
	void WxTextEntryWrapper::RemoveRange(size_t from, size_t to)
	{
		if (from != npos)
		{
			DispatchCall(, Remove, from, to != npos ? to : -1);
		}
	}
	void WxTextEntryWrapper::ReplaceRange(size_t from, size_t to, const String& text)
	{
		if (from != npos)
		{
			DispatchCall(, Replace, from, to != npos ? to : -1, text);
		}
	}

	size_t WxTextEntryWrapper::GetTextLength() const
	{
		DispatchCall(0, GetLastPosition);
	}
	String WxTextEntryWrapper::GetText() const
	{
		DispatchCall({}, GetValue);
	}
	void WxTextEntryWrapper::SetText(const String& text)
	{
		DispatchCall(, ChangeValue, text);
	}
	void WxTextEntryWrapper::AppendText(const String& text)
	{
		DispatchCall(, AppendText, text);
	}

	String WxTextEntryWrapper::GetHint() const
	{
		DispatchCall({}, GetHint);
	}
	void WxTextEntryWrapper::SetHint(const String& hint)
	{
		auto DoCall = [&]()
		{
			DispatchCall(false, SetHint, hint);
		};
		DoCall();
	}
}
