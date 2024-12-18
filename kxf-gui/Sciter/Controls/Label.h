#pragma once
#include "Common.h"
#include <wx/control.h>

namespace kxf::Sciter
{
	class KXF_API Label: public WindowWrapper<wxControl>
	{
		public:
			Label() = default;
			Label(const Label&) = delete;
			Label(wxWindow* parent,
				  wxWindowID id,
				  const String& label,
				  const Point& pos = Point::UnspecifiedPosition(),
				  const Size& size = Size::UnspecifiedSize(),
				  long style = wxBORDER_NONE)
			{
				Create(parent, id, label, pos, size, style);
			}

			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						long style = wxBORDER_NONE)
			{
				if (WindowWrapper::Create(parent, id, pos, size, style))
				{
					SetLabel(label);
					return true;
				}
				return false;
			}

		public:
			Label& operator=(const Label&) = delete;
	};
}
