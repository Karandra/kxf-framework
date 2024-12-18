#include "kxf-pch.h"
#include "Validator.h"

#include <wx/spinctrl.h>
#include "kxf/Win32/UndefMacros.h"

namespace
{
	class SpinCtrlDoubleWrapper final: public wxSpinCtrlDouble
	{
		public:
			wxString DoValueToText(double value) override
			{
				return wxSpinCtrlDouble::DoValueToText(value);
			}
	};

	void GetTextSelection(HWND textArea, int& selectionStart, int& selectionEnd)
	{
		DWORD start = 0;
		DWORD end = 0;
		::SendMessageW(textArea, EM_GETSEL, reinterpret_cast<WPARAM>(&start), reinterpret_cast<LPARAM>(&end));

		selectionStart = start;
		selectionEnd = end;
	}
	void ProcessTextSelection(wxString& value, int& position, int selectionStart, int selectionEnd)
	{
		if (size_t selectionLength = selectionEnd - selectionStart; selectionLength != 0)
		{
			// Remove selected text because pressing a key would make it disappear
			value.erase(selectionStart, selectionLength);

			// And adjust the insertion point to have correct position in the new string
			if (position > selectionStart)
			{
				if (position >= selectionEnd)
				{
					position -= selectionLength;
				}
				else
				{
					position = selectionStart;
				}
			}
		}
	}
}

namespace kxf::UI::Private
{
	void GetValidatorControlValue(const wxValidator& validator, wxString& value, int& position, XChar& character)
	{
		// Look in "common/valnum.cpp" at line 166 (v3.1.3).
		/*
			wxString val;
			int pos;
			GetCurrentValueAndInsertionPoint(val, pos);
		*/
		// The 'pos' (called 'position' in this function) is going to be uninitialized
		// if 'GetCurrentValueAndInsertionPoint' returns before assigning it because
		// 'wxNumValidatorBase::GetTextEntry' function can't get 'wxTextEntry' from
		// spin controls as they have no dedicated 'wxTextEntry' object (only HWND of it).

		// Pure virtual 'wxNumValidatorBase::IsCharOk' is called right after 'GetCurrentValueAndInsertionPoint'
		// without any checks, so we can do anything that function would need to do in order to get user input.

		// If no appropriate control will be found, ensure that uninitialized int don't cause any troubles
		position = 0;

		auto ProcessSelectionRange = [](HWND textArea, const wxString& value, int& position)
		{
			int selectionStart = 0;
			int selectionEnd = 0;
			GetTextSelection(textArea, selectionStart, selectionEnd);

			if (selectionEnd == 0 && !value.IsEmpty())
			{
				selectionEnd = value.size();
			}

			position = selectionStart;
			return std::pair(selectionStart, selectionEnd);
		};

		if (wxTextEntry* textEntry = dynamic_cast<wxTextCtrl*>(validator.GetWindow()))
		{
			// This part replicates 'wxNumValidatorBase::GetCurrentValueAndInsertionPoint' itself

			value = textEntry->GetValue();
			position = textEntry->GetInsertionPoint();

			long selectionStart = 0;
			long selectionEnd = 0;
			textEntry->GetSelection(&selectionStart, &selectionEnd);

			ProcessTextSelection(value, position, selectionStart, selectionEnd);
		}
		else if (wxSpinCtrl* spinInteger = dynamic_cast<wxSpinCtrl*>(validator.GetWindow()))
		{
			if (HWND textArea = ::GetWindow(spinInteger->GetHandle(), GW_HWNDPREV))
			{
				switch (spinInteger->GetBase())
				{
					case 2:
					{
						value = Format("{:0b}", spinInteger->GetValue());
						break;
					}
					case 8:
					{
						value = Format("{:0o}", spinInteger->GetValue());
						break;
					}
					case 16:
					{
						value = Format("{:0x}", spinInteger->GetValue());
						break;
					}
					default:
					{
						value = Format("{:0d}", spinInteger->GetValue());
						break;
					}
				};

				auto [selectionStart, selectionEnd] = ProcessSelectionRange(textArea, value, position);
				ProcessTextSelection(value, position, selectionStart, selectionEnd);
			}
		}
		else if (wxSpinCtrlDouble* spinDouble = dynamic_cast<wxSpinCtrlDouble*>(validator.GetWindow()))
		{
			if (HWND textArea = ::GetWindow(spinDouble->GetHandle(), GW_HWNDPREV))
			{
				// Hacky, but works
				SpinCtrlDoubleWrapper* wrapper = reinterpret_cast<SpinCtrlDoubleWrapper*>(spinDouble);
				value = wrapper->DoValueToText(spinDouble->GetValue());

				auto [selectionStart, selectionEnd] = ProcessSelectionRange(textArea, value, position);
				ProcessTextSelection(value, position, selectionStart, selectionEnd);
			}
		}

		// Clamp position just in case something gone wrong
		position = std::clamp<int>(position, 0, value.size());
	}
}
