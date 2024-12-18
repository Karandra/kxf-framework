#pragma once
#include "kxf/UI/Common.h"
#include "StdDialog.h"

namespace kxf::UI
{
	class KXF_API SimpleStdDialog: public StdDialog
	{
		private:
			wxWindow* m_View = nullptr;

		private:
			wxOrientation GetWindowResizeSide() const override
			{
				return wxBOTH;
			}

		public:
			SimpleStdDialog() = default;
			bool Create(wxWindow* parent,
						wxWindowID id,
						wxWindow* pView,
						const wxString& caption,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						FlagSet<StdButton> buttons = DefaultButtons,
						FlagSet<DialogStyle> style = DefaultStyle
			);

		public:
			wxWindow* GetDialogMainCtrl() const override
			{
				return m_View;
			}
			int GetViewSizerProportion() const override
			{
				return 1;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(SimpleStdDialog);
	};
}
