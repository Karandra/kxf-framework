#pragma once
#include "Common.h"
#include "../../ICheckWidget.h"
#include <wx/checkbox.h>

namespace kxf::WXUI
{
	class KXF_API Check: public EvtHandlerWrapper<Check, wxCheckBox>
	{
		friend class EvtHandlerWrapper;

		private:
			ICheckWidget& m_Widget;

		private:
			bool DoTryBefore(wxEvent& event);

		public:
			Check(ICheckWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			bool GetValue() const override
			{
				if (Is3State())
				{
					return wxCheckBox::Get3StateValue() == wxCHK_CHECKED;
				}
				return wxCheckBox::GetValue();
			}
			void SetValue(bool value) override
			{
				wxCheckBox::SetValue(value);
			}

			wxCheckBoxState GetCheckState() const
			{
				if (Is3State())
				{
					return wxCheckBox::Get3StateValue();
				}
				return wxCheckBox::GetValue() ? wxCHK_CHECKED : wxCHK_UNCHECKED;
			}
			void SetCheckState(wxCheckBoxState state)
			{
				wxCheckBox::Set3StateValue(state);
			}

			void Set3StateValue(wxCheckBoxState) = delete;
			wxCheckBoxState Get3StateValue() = delete;
	};
}
