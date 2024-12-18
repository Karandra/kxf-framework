#include "kxf-pch.h"
#include "ComboBox.h"
#include <CommCtrl.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ComboBox, wxComboBox);

	bool ComboBox::Create(wxWindow* parent,
						  wxWindowID id,
						  const String& value,
						  const Point& pos,
						  const Size& size,
						  FlagSet<ComboBoxStyle> style,
						  const wxValidator& validator
	)
	{
		if (wxComboBox::Create(parent, id, value, pos, size, 0, nullptr, style.ToInt(), validator))
		{
			EnableSystemTheme();
			return true;
		}
		return false;
	}

	bool ComboBox::SetVisibleItemsCount(size_t count)
	{
		return ::SendMessageW(GetHandle(), CB_SETMINVISIBLE, (WPARAM)count, 0);
	}
}
