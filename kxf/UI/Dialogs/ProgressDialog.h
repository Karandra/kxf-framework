#pragma once
#include "kxf/UI/Common.h"
#include "StdDialog.h"
#include "kxf/UI/Controls/ProgressBar.h"

namespace kxf::UI
{
	class KXF_API ProgressDialog: public RTTI::Implementation<ProgressDialog, StdDialog, ProgressBarWrapper>
	{
		public:
			static constexpr int DefaultProgressWidth = 300;
			static constexpr int DefaultProgressHeight = 16;
		
		private:
			ProgressBar* m_View = nullptr;

		private:
			wxOrientation GetViewLabelSizerOrientation() const override;
			bool IsEnterAllowed(wxKeyEvent& event, WidgetID* idOut = nullptr) const override;

		public:
			ProgressDialog() = default;
			ProgressDialog(wxWindow* parent,
						   wxWindowID id,
						   const String& caption,
						   const Point& pos = Point::UnspecifiedPosition(),
						   const Size& size = Size::UnspecifiedSize(),
						   FlagSet<StdButton> buttons = DefaultButtons,
						   FlagSet<DialogStyle> style = DefaultStyle
			)
			{
				Create(parent, id, caption, pos, size, buttons, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& caption,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						FlagSet<StdButton> buttons = DefaultButtons,
						FlagSet<DialogStyle> style = DefaultStyle
			);

		public:
			ProgressBar* GetProgressBar() const
			{
				return m_View;
			}
			ProgressBar* GetDialogMainCtrl() const override
			{
				return GetProgressBar();
			}
			int GetViewSizerProportion() const override
			{
				return 0;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(ProgressDialog);
	};
}
