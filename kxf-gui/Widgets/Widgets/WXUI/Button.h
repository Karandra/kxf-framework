#pragma once
#include "Common.h"
#include "../../IButtonWidget.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
#include "kxf/Drawing/IRendererNative.h"
#include <wx/anybutton.h>
#include <wx/systhemectrl.h>

namespace kxf::WXUI
{
	class KXF_API Button: public EvtHandlerWrapper<Button, UI::WindowRefreshScheduler<wxSystemThemedControl<wxAnyButton>>>
	{
		public:
			static Size GetDefaultSize();

		private:
			IButtonWidget& m_Widget;
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;
			wxEvtHandler m_EvtHandler;

			NativeWidgetFlag m_WidgetState = NativeWidgetFlag::None;
			bool m_IsDropdownEnbled = false;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnResize(wxSizeEvent& event);
			void OnKillFocus(wxFocusEvent& event);
			void OnMouseEnter(wxMouseEvent& event);
			void OnMouseLeave(wxMouseEvent& event);
			void OnLeftButtonUp(wxMouseEvent& event);
			void OnLeftButtonDown(wxMouseEvent& event);

		protected:
			// wxWindow
			wxSize DoGetBestSize() const override;
			wxSize DoGetBestClientSize() const override;
			wxSize DoGetSizeFromTextSize(int xlen, int ylen = -1) const override;

		public:
			Button(IButtonWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}
			~Button()
			{
				if (m_EvtHandler.GetClientData() == this)
				{
					PopEventHandler();
				}
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			// wxWindow
			bool Enable(bool enable = true) override
			{
				ScheduleRefresh();
				return wxAnyButton::Enable(enable);
			}
			void SetLabel(const wxString& label) override
			{
				ScheduleRefresh();
				wxAnyButton::SetLabel(label);
			}

			// Button
			bool IsDefault() const;
			wxWindow* SetDefault();

			bool IsDropdownEnabled() const
			{
				return m_IsDropdownEnbled;
			}
			void SetDropdownEnbled(bool show = true)
			{
				m_IsDropdownEnbled = show;
				ScheduleRefresh();
			}
	};
}
