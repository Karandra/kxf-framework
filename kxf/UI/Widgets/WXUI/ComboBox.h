#pragma once
#include "Common.h"
#include "../../IComboBoxWidget.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/systhemectrl.h>
#include <wx/combobox.h>

namespace kxf::WXUI
{
	class KXF_API ComboBox: public EvtHandlerWrapper<ComboBox, UI::WindowRefreshScheduler<wxSystemThemedControl<wxComboBox>>>
	{
		friend class EvtHandlerWrapper;

		private:
			IComboBoxWidget& m_Widget;

		private:
			bool DoTryBefore(wxEvent& event);

		public:
			ComboBox(IComboBoxWidget& widget)
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
			bool SetVisibleItemCount(size_t count);
	};
}
