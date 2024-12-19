#include "kxf-pch.h"
#include "Button.h"
#include "../../Events/ButtonWidgetEvent.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"
#include <wx/toplevel.h>
#include <wx/button.h>

namespace
{
	constexpr auto g_DefaultButtonWidth = 72;
	constexpr auto g_DefaultButtonHeight = 23;
	constexpr auto g_ArrowButtonWidth = 17;

	int FromDIPX(const wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(value, wxDefaultCoord)).GetWidth();
	}
	int FromDIPY(const wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(wxDefaultCoord, value)).GetHeight();
	}

	kxf::Size CalcBestSize(const kxf::WXUI::Button& button, kxf::Size size)
	{
		if (!size.IsFullySpecified())
		{
			size = kxf::Size(button.GetSizeFromTextSize(button.GetTextExtent(button.GetLabelText())));
		}

		if (size.GetWidth() <= 0)
		{
			size.SetWidth(FromDIPX(&button, g_DefaultButtonWidth));
		}
		if (size.GetHeight() <= 0 || std::abs(size.GetHeight() - g_DefaultButtonHeight) <= button.GetCharHeight())
		{
			size.SetHeight(FromDIPY(&button, g_DefaultButtonHeight));
		}
		return size;
	}
}

namespace kxf::WXUI
{
	Size Button::GetDefaultSize()
	{
		return Size(wxButton::GetDefaultSize());
	}

	void Button::OnPaint(wxPaintEvent& event)
	{
		IRendererNative& nativeRenderer = IRendererNative::Get();
		auto renderer = m_RendererAware->GetActiveGraphicsRenderer();

		auto gc = renderer->CreateLegacyWindowPaintContext(*this);
		gc->Clear(renderer->GetTransparentBrush());

		const bool isEnabled = IsEnabled();
		const Size clientSize = m_Widget.GetSize();
		const Rect contentRect = {m_Widget.FromDIP<Point>(2, 2), clientSize - m_Widget.FromDIP<Size>(4, 4)};
		int width = clientSize.GetWidth();
		int widthMod = 2;
		if (m_IsDropdownEnbled)
		{
			width -= g_ArrowButtonWidth;
			widthMod = 5;
		}
		Rect rect(-1, -1, width + widthMod, clientSize.GetHeight() + 2);

		FlagSet<NativeWidgetFlag> widgetState = m_WidgetState;
		widgetState.Add(NativeWidgetFlag::Disabled, !isEnabled);

		// Draw the first part
		gc->SetFontBrush(renderer->CreateSolidBrush(isEnabled ? GetForegroundColour() : GetForegroundColour().MakeDisabled()));
		nativeRenderer.DrawPushButton(this, *gc, rect, widgetState);

		// Draw focus rectangle
		if (HasFocus() && m_Widget.IsFocusVisible())
		{
			nativeRenderer.DrawItemFocusRect(this, *gc, contentRect, NativeWidgetFlag::Selected);
		}

		// Draw bitmap and label
		if (BitmapImage bitmap = GetBitmap())
		{
			if (!isEnabled)
			{
				bitmap = bitmap.ConvertToDisabled();
			}
			gc->DrawLabel(GetLabelText(), contentRect, bitmap, Alignment::Center);
		}
		else
		{
			gc->DrawLabel(GetLabelText(), contentRect, Alignment::Center);
		}

		// Draw second part of the button
		if (m_IsDropdownEnbled)
		{
			Rect splitRect = rect;
			splitRect.X() = width + FromDIPX(this, 1);
			splitRect.Y() = -1;
			splitRect.Width() = FromDIPX(this, g_ArrowButtonWidth);
			splitRect.Height() = clientSize.GetHeight() + FromDIPX(this, 2);

			nativeRenderer.DrawPushButton(this, *gc, splitRect, widgetState);
			nativeRenderer.DrawDropArrow(this, *gc, splitRect, widgetState);
		}
	}
	void Button::OnResize(wxSizeEvent& event)
	{
		Refresh();
		event.Skip();
	}
	void Button::OnKillFocus(wxFocusEvent& event)
	{
		ScheduleRefresh();
		m_WidgetState = NativeWidgetFlag::None;

		event.Skip();
	}
	void Button::OnMouseEnter(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_WidgetState = NativeWidgetFlag::Current;

		event.Skip();
	}
	void Button::OnMouseLeave(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_WidgetState = NativeWidgetFlag::None;

		event.Skip();
	}
	void Button::OnLeftButtonUp(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_WidgetState = NativeWidgetFlag::Current;

		const Point pos = Point(event.GetPosition());
		if (m_IsDropdownEnbled && pos.GetX() > (GetClientSize().GetWidth() - g_ArrowButtonWidth))
		{
			m_Widget.ProcessEvent(ButtonWidgetEvent::EvtDropdown, m_Widget);
		}
		else
		{
			m_Widget.ProcessEvent(ButtonWidgetEvent::EvtClick, m_Widget);
		}

		event.Skip();
	}
	void Button::OnLeftButtonDown(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_WidgetState = NativeWidgetFlag::Pressed;

		event.Skip();
	}

	wxSize Button::DoGetBestSize() const
	{
		return CalcBestSize(*this, Size(wxAnyButton::DoGetBestSize()));
	}
	wxSize Button::DoGetBestClientSize() const
	{
		return CalcBestSize(*this, Size(wxAnyButton::DoGetBestClientSize()));
	}
	wxSize Button::DoGetSizeFromTextSize(int xlen, int ylen) const
	{
		Size size = Size(ConvertDialogToPixels(wxSize(16, 0)));
		if (xlen > 0)
		{
			size.Width() += xlen;
		}
		if (ylen > 0)
		{
			size.Height() += ylen;
		}
		return size;
	}

	bool Button::Create(wxWindow* parent,
						const String& label,
						const Point& pos,
						const Size& size
	)
	{
		if (wxAnyButton::Create(parent, wxID_NONE, pos, size, 0, wxDefaultValidator))
		{
			SetLabel(label);
			SetBackgroundStyle(wxBG_STYLE_PAINT);
			EnableSystemTheme();
			MakeOwnerDrawn();

			m_EvtHandler.Bind(wxEVT_PAINT, &Button::OnPaint, this);
			m_EvtHandler.Bind(wxEVT_SIZE, &Button::OnResize, this);
			m_EvtHandler.Bind(wxEVT_KILL_FOCUS, &Button::OnKillFocus, this);
			m_EvtHandler.Bind(wxEVT_LEFT_UP, &Button::OnLeftButtonUp, this);
			m_EvtHandler.Bind(wxEVT_LEFT_DOWN, &Button::OnLeftButtonDown, this);
			m_EvtHandler.Bind(wxEVT_LEAVE_WINDOW, &Button::OnMouseLeave, this);
			m_EvtHandler.Bind(wxEVT_ENTER_WINDOW, &Button::OnMouseEnter, this);

			PushEventHandler(&m_EvtHandler);
			m_EvtHandler.SetClientData(this);

			return m_Widget.QueryInterface(m_RendererAware);
		}
		return false;
	};

	bool Button::IsDefault() const
	{
		const wxWindow* tlwParent = ::wxGetTopLevelParent(const_cast<Button*>(this));
		if (tlwParent && tlwParent->IsKindOf(wxCLASSINFO(wxTopLevelWindow)))
		{
			return static_cast<const wxTopLevelWindow*>(tlwParent)->GetDefaultItem() == this;
		}
		return false;
	}
	wxWindow* Button::SetDefault()
	{
		ScheduleRefresh();

		wxWindow* tlwParent = wxGetTopLevelParent(this);
		if (tlwParent && tlwParent->IsKindOf(wxCLASSINFO(wxTopLevelWindow)))
		{
			return static_cast<wxTopLevelWindow*>(tlwParent)->SetDefaultItem(this);
		}
		return nullptr;
	}
}
