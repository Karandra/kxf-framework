#include "kxf-pch.h"
#include "Dialog.h"
#include "kxf-gui/Widgets/Controls/Button.h"
#include "kxf/Localization/Common.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Dialog, wxDialog);

	bool Dialog::Create(wxWindow* parent,
						wxWindowID id,
						const String& title,
						const Point& pos,
						const Size& size,
						FlagSet<DialogStyle> style
	)
	{
		if (wxDialog::Create(parent, id, title, pos, size, style.ToInt()))
		{
			SetInitialSize(size);
			SetDefaultBackgroundColor();
			CenterIfNoPosition(pos);

			return true;
		}
		return false;
	}

	StdDialogButtonSizer* Dialog::CreateStdDialogButtonSizer(FlagSet<StdButton> buttons, FlagSet<StdButton> defaultButtons)
	{
		auto* stdButtonSizer = new UI::StdDialogButtonSizer();

		const bool addToList = true;
		Button* ok = stdButtonSizer->CreateButton(this, buttons, StdButton::OK, wxID_OK, addToList);
		Button* yes = stdButtonSizer->CreateButton(this, buttons, StdButton::Yes, wxID_YES, addToList);
		Button* no = stdButtonSizer->CreateButton(this, buttons, StdButton::No, wxID_NO, addToList);
		stdButtonSizer->CreateButton(this, buttons, StdButton::Retry, wxID_RETRY, addToList);
		stdButtonSizer->CreateButton(this, buttons, StdButton::Apply, wxID_APPLY, addToList);
		stdButtonSizer->CreateButton(this, buttons, StdButton::Close, wxID_CLOSE, addToList);
		stdButtonSizer->CreateButton(this, buttons, StdButton::Cancel, wxID_CANCEL, addToList);
		stdButtonSizer->CreateButton(this, buttons, StdButton::Help, wxID_HELP, addToList);

		if (defaultButtons & StdButton::No)
		{
			if (no)
			{
				no->SetDefault();
				no->SetFocus();
			}
		}
		else
		{
			if (ok)
			{
				ok->SetDefault();
				ok->SetFocus();
			}
			else if (yes)
			{
				yes->SetDefault();
				yes->SetFocus();
			}
		}

		if (buttons & StdButton::OK)
		{
			SetAffirmativeId(wxID_OK);
		}
		else if (buttons & StdButton::Yes)
		{
			SetAffirmativeId(wxID_YES);
		}
		else if (buttons & StdButton::Close)
		{
			SetAffirmativeId(wxID_CLOSE);
		}

		stdButtonSizer->Realize();
		return stdButtonSizer;
	}
	void Dialog::SetStdLabels(wxSizer* sizer)
	{
		for (auto& sizerItem: sizer->GetChildren())
		{
			if (wxWindow* window = sizerItem->GetWindow())
			{
				window->SetLabel(Localization::GetStandardString(window->GetId()));

				Size size = Size(window->GetSize());
				size.Scale(0.85f, 1.0f);
				size.SetHeight(23);
				window->SetMaxSize(size);
			}
		}
	}
	void Dialog::CenterIfNoPosition(const Point& pos)
	{
		if (!pos.IsFullySpecified())
		{
			CenterOnScreen(wxBOTH);
		}
	}
}
