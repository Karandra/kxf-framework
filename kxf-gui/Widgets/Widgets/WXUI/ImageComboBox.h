#pragma once
#include "Common.h"
#include "../../IImageComboBoxWidget.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
#include <wx/systhemectrl.h>
#include <wx/bmpcbox.h>

namespace kxf::WXUI
{
	class KXF_API ImageComboBox: public EvtHandlerWrapper<ImageComboBox, UI::WindowRefreshScheduler<wxSystemThemedControl<wxBitmapComboBox>>>
	{
		friend class EvtHandlerWrapper;

		private:
			IImageComboBoxWidget& m_Widget;

		private:
			bool DoTryBefore(wxEvent& event);

		public:
			ImageComboBox(IImageComboBoxWidget& widget)
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
