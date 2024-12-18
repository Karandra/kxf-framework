#include "kxf-pch.h"
#include "ProgressDialog.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ProgressDialog, StdDialog);

	bool ProgressDialog::Create(wxWindow* parent,
								wxWindowID id,
								const String& caption,
								const Point& pos,
								const Size& size,
								FlagSet<StdButton> buttons,
								FlagSet<DialogStyle> style
	)
	{
		m_DialogResizeSide = wxHORIZONTAL;

		if (StdDialog::Create(parent, id, caption, pos, size, buttons, style))
		{
			m_View = new ProgressBar(m_ContentPanel, wxID_NONE, 100);
			m_View->SetInitialSize(FromDIP(Size(DefaultProgressWidth, DefaultProgressHeight)));
			SetProgressBar(m_View);

			PostCreate(pos);
			return true;
		}
		return false;
	}

	bool ProgressDialog::IsEnterAllowed(wxKeyEvent& event, WidgetID* idOut) const
	{
		return StdDialog::IsEnterAllowed(event, idOut);
	}
	wxOrientation ProgressDialog::GetViewLabelSizerOrientation() const
	{
		return StdDialog::GetViewLabelSizerOrientation();
	}
}
