#pragma once
#include "kxf/Widgets/Common.h"
#include <wx/splitter.h>

namespace kxf::UI
{
	enum class SplitterWindowStyle: uint32_t
	{
		None = 0,

		LiveUpdate = wxSP_LIVE_UPDATE,
		AllowUnsplit = wxSP_PERMIT_UNSPLIT,
		SashNone = wxSP_NOSASH,
		Sash3D = wxSP_3DSASH,
		Border3D = wxSP_3DBORDER,
		NoXPTheme = wxSP_NO_XP_THEME,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::SplitterWindowStyle);
}

namespace kxf::UI
{
	class KXF_API SplitterWindow: public wxSplitterWindow
	{
		public:
			static constexpr FlagSet<SplitterWindowStyle> DefaultStyle = SplitterWindowStyle::LiveUpdate|SplitterWindowStyle::Sash3D;

		private:
			Color m_SashColor;
			int m_InitialPosition = 0;

		private:
			void OnDoubleClickSash(int x, int y) override;
			void OnDrawSash(wxPaintEvent& event);

		public:
			SplitterWindow() = default;
			SplitterWindow(wxWindow* parent,
						   wxWindowID id = wxID_ANY,
						   FlagSet<SplitterWindowStyle> style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id = wxID_ANY,
						FlagSet<SplitterWindowStyle> style = DefaultStyle
			);

		public:
			bool ShouldInheritColours() const override
			{
				return true;
			}
			bool SplitHorizontally(wxWindow* window1, wxWindow* window2, int sashPosition = 0) override;
			bool SplitVertically(wxWindow* window1, wxWindow* window2, int sashPosition = 0) override;

			int GetInitialPosition() const
			{
				return m_InitialPosition;
			}
			void SetInitialPosition(int pos);

			Color GetSashColor() const
			{
				return m_SashColor;
			}
			void SetSashColor(const Color& color);

		public:
			wxDECLARE_DYNAMIC_CLASS(SplitterWindow);
	};
}
