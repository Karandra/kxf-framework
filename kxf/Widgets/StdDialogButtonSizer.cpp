#include "kxf-pch.h"
#include "StdDialogButtonSizer.h"
#include "kxf/Localization/Common.h"
#include "kxf/Core/Private/Mapping.h"
#include "kxf/Widgets/Controls/Button.h"
#include "kxf/Widgets/Dialogs/Dialog.h"
#include "kxf/Utility/Common.h"

namespace kxf::UI
{
	wxIMPLEMENT_CLASS(StdDialogButtonSizer, wxBoxSizer);

	void StdDialogButtonSizer::ConfigureButton(wxAnyButton* button)
	{
		Size size = Size(button->GetBestSize());
		const int defaultWidth = button->FromDIP(Size(72, wxDefaultCoord).GetWidth());

		if (size.GetWidth() < defaultWidth)
		{
			size.SetWidth(defaultWidth);
		}
		button->SetMinSize(size);
	}

	StdDialogButtonSizer::StdDialogButtonSizer()
		:wxBoxSizer(wxHORIZONTAL)
	{
		// If we have a PDA screen, put yes/no button over all other buttons, otherwise on the left side.
		const bool isPDA = wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA;
		if (isPDA)
		{
			SetOrientation(wxVERTICAL);
		}
	}

	Button* StdDialogButtonSizer::CreateButton(Dialog* dialog, FlagSet<StdButton> buttons, StdButton singleButton, WidgetID id, bool addToList)
	{
		if (buttons & singleButton)
		{
			Button* button = new Button(dialog, id.ToInt());
			button->SetAllowDrawFocus(true);
			if (addToList)
			{
				AddButton(button);
			}
			return button;
		}
		return nullptr;
	}
	bool StdDialogButtonSizer::AddButton(wxAnyButton* button)
	{
		bool isStandard = true;
		switch (button->GetId())
		{
			case wxID_OK:
			case wxID_YES:
			case wxID_SAVE:
			case wxID_RETRY:
			{
				m_ButtonAffirmative = button;
				break;
			}
			case wxID_APPLY:
			{
				m_ButtonApply = button;
				break;
			}
			case wxID_NO:
			{
				m_ButtonNegative = button;
				break;
			}
			case wxID_CANCEL:
			case wxID_CLOSE:
			{
				m_ButtonCancel = button;
				break;
			}
			case wxID_HELP:
			{
				m_ButtonHelp = button;
				break;
			}
			case wxID_CONTEXT_HELP:
			{
				wxWindow* window = GetContainingWindow();
				window->SetExtraStyle(window->GetExtraStyle()|wxWS_EX_CONTEXTHELP);
				break;
			}

			default:
			{
				button->SetLabel(Localization::GetStandardString(button->GetId()));
				m_NonStandardButtons.push_back(button);
				isStandard = false;
			}
		};

		ConfigureButton(button);
		return isStandard;
	}
	void StdDialogButtonSizer::AddCustomButton(Button* button, const String& label, bool prepend)
	{
		if (label.IsEmpty())
		{
			button->SetLabel(Localization::GetStandardString(button->GetId()));
		}
		else
		{
			button->SetLabel(label);
		}
		ConfigureButton(button);

		int border = GetSpacing(button);
		auto flags = Utility::CombineEnumFlags<int>(*kxf::Private::MapAlignment(Alignment::Center), *kxf::Private::MapDirection(Direction::Left|Direction::Right));
		if (prepend)
		{
			if (!m_HasFirstPrepend)
			{
				m_HasFirstPrepend = true;
				InsertSpacer(0, button->FromDIP(Size(6, wxDefaultCoord).GetWidth()));
			}
			Insert(1, button, 0, flags, border);
		}
		else
		{
			Add(button, 0, flags, border);
		}
	}
	void StdDialogButtonSizer::Realize()
	{
		/* Std behavior, Windows layout */
		// Right-justify buttons
		Add(0, 0, 1, wxEXPAND, 0);

		auto AddStdButton = [this](wxAnyButton* button)
		{
			if (button)
			{
				auto flags = Utility::CombineEnumFlags<int>(*kxf::Private::MapAlignment(Alignment::Center), *kxf::Private::MapDirection(Direction::Left|Direction::Right));
				Add(button, 0, flags, button->ConvertDialogToPixels(Size(2, 0)).GetWidth());
			}
		};
		AddStdButton(m_ButtonAffirmative);
		AddStdButton(m_ButtonNegative);
		AddStdButton(m_ButtonCancel);
		AddStdButton(m_ButtonApply);
		AddStdButton(m_ButtonHelp);

		// Custom layout
		auto SetLabel = [this](wxAnyButton* button)
		{
			if (button)
			{
				button->SetLabel(Localization::GetStandardString(button->GetId()));
			}
		};
		SetLabel(m_ButtonAffirmative);
		SetLabel(m_ButtonApply);
		SetLabel(m_ButtonNegative);
		SetLabel(m_ButtonCancel);
		SetLabel(m_ButtonHelp);

		auto flags = Utility::CombineEnumFlags<int>(*kxf::Private::MapAlignment(Alignment::Center), *kxf::Private::MapDirection(Direction::Left));
		for (wxAnyButton* button: m_NonStandardButtons)
		{
			Add(button, 0, flags, GetSpacing(button));
		}
	}
}
