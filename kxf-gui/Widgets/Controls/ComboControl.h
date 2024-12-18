#pragma once
#include "kxf-gui/Widgets/Common.h"
#include <wx/combo.h>

namespace kxf::UI
{
	class ComboPopup;
}

namespace kxf::UI
{
	class KXF_API ComboControl: public wxComboCtrl
	{
		public:
			enum class DrawFocus
			{
				Always = 1,
				Never = 0,
				Auto = -1,
			};
			static constexpr long DefaultStyle = 0;

		private:
			ComboPopup* m_PopupCtrl = nullptr;
			DrawFocus m_FocusDrawMode = DrawFocus::Auto;

		protected:
			void DoShowPopup(const wxRect& rect, int flags) override;

		public:
			ComboControl() = default;
			ComboControl(wxWindow* window,
						 wxWindowID id,
						 const String& value = {},
						 long style = DefaultStyle,
						 const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(window, id, value, style, validator);
			}
			bool Create(wxWindow* window,
						wxWindowID id,
						const String& value = {},
						long style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			);

		public:
			virtual bool ShouldDrawFocus() const;
			void SetFocusDrawMode(DrawFocus mode);
			
			ComboPopup* GetPopupControl()
			{
				return m_PopupCtrl;
			}
			void SetPopupControl(ComboPopup* popup);

		public:
			wxDECLARE_DYNAMIC_CLASS_NO_COPY(ComboControl);
	};
}
