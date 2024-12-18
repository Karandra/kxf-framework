#include "kxf-pch.h"
#include "SimpleStdDialog.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(SimpleStdDialog, StdDialog);

	bool SimpleStdDialog::Create(wxWindow* parent,
								 wxWindowID id,
								 wxWindow* pView,
								 const wxString& caption,
								 const Point& pos,
								 const Size& size,
								 FlagSet<StdButton> buttons,
								 FlagSet<DialogStyle> style
	)
	{
		m_View = pView;
		if (StdDialog::Create(parent, id, caption, pos, size, buttons, style))
		{
			m_View->Reparent(m_ContentPanel);
			PostCreate(pos);
			return true;
		}
		return false;
	}
}
