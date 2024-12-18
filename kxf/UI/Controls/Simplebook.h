#pragma once
#include "kxf/UI/Common.h"
#include <wx/simplebook.h>

namespace kxf::UI
{
	class KXF_API Simplebook: public wxSimplebook
	{
		public:
			static constexpr long DefaultStyle = 0;

		public:
			Simplebook() = default;
			Simplebook(wxWindow* parent,
					   wxWindowID id,
					   long style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						long style = DefaultStyle
			);

		public:
			bool ShouldInheritColours() const override
			{
				return true;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(Simplebook);
	};
}