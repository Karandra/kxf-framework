#include "kxf-pch.h"
#include "ComboControl.h"
#include "ComboPopup.h"

namespace kxf::UI
{
	wxIMPLEMENT_ABSTRACT_CLASS(ComboControl, wxComboCtrl);

	void ComboControl::DoShowPopup(const wxRect& rect, int flags)
	{
		wxComboCtrl::DoShowPopup(rect, flags);
		m_PopupCtrl->OnDoShowPopup();
	}

	bool ComboControl::Create(wxWindow* window,
							  wxWindowID id,
							  const String& value,
							  long style,
							  const wxValidator& validator
	)
	{
		if (wxComboCtrl::Create(window, id, value, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style, validator))
		{
			return true;
		}
		return false;
	}

	bool ComboControl::ShouldDrawFocus() const
	{
		switch (m_FocusDrawMode)
		{
			case DrawFocus::Always:
			{
				return true;
			}
			case DrawFocus::Never:
			{
				return false;
			}
		};
		return wxComboCtrl::ShouldDrawFocus();
	}
	void ComboControl::SetFocusDrawMode(DrawFocus mode)
	{
		m_FocusDrawMode = mode;
		Refresh();
	}
	void ComboControl::SetPopupControl(ComboPopup* popup)
	{
		m_PopupCtrl = popup;
		wxComboCtrl::SetPopupControl(popup);
	}
}
