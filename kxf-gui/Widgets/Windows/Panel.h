#pragma once
#include "kxf-gui/Widgets/Common.h"
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/systhemectrl.h>

namespace kxf::UI
{
	class KXF_API Panel: public wxSystemThemedControl<wxPanel>
	{
		public:
			static constexpr FlagSet<WindowStyle> DefaultStyle = WindowStyle::TabTraversal|WindowBorder::None;

		public:
			Panel() = default;
			Panel(wxWindow* parent,
				  wxWindowID id,
				  FlagSet<WindowStyle> style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						FlagSet<WindowStyle> style = DefaultStyle
			);
			bool Create(wxWindow* parent,
						wxWindowID id,
						const Point& pos,
						const Size& size,
						FlagSet<WindowStyle> style = DefaultStyle,
						const String& name = {}
			)
			{
				return Create(parent, id, style);
			}

		public:
			bool ShouldInheritColours() const override
			{
				return true;
			}
			bool SetBackgroundColour(const wxColour& color) override
			{
				return wxPanel::SetBackgroundColour(color);
			}
			bool SetForegroundColour(const wxColour& color) override
			{
				return wxPanel::SetForegroundColour(color);
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(Panel);
	};

	using ScrolledPanel = wxScrolled<Panel>;
}
