#include "kxf-pch.h"
#include "Label.h"
#include "kxf-gui/Drawing/GDIRenderer/UxTheme.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIWindowContext.h"

namespace
{
	constexpr wxEllipsizeMode g_EllipsizeMode = wxELLIPSIZE_END;
	constexpr int g_MinSingleLineHeight = 23;
}

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Label, wxStaticText);

	Color Label::GetStateColor() const
	{
		if (IsThisEnabled())
		{
			switch (m_State)
			{
				case wxCONTROL_NONE:
				{
					return m_ColorNormal;
				}
				case wxCONTROL_FOCUSED:
				{
					return m_ColorHighlight;
				}
				case wxCONTROL_PRESSED:
				{
					return m_ColorClick;
				}
			};
			return m_ColorNormal;
		}
		return m_ColorDisabled;
	}
	Size Label::CalcBestSize(GDIContext* dc)
	{
		const Size padding = Size(ConvertDialogToPixels(wxSize(3, 1)));

		if (m_IsMultilne)
		{
			Size textExtent;
			if (dc)
			{
				textExtent = dc->GetMultiLineTextExtent(m_Label);
			}
			else
			{
				GDIClientContext clientDC(*this);
				textExtent = clientDC.GetMultiLineTextExtent(m_Label);
			}
			return textExtent + padding;
		}
		else
		{
			return (dc ? dc->GetTextExtent(m_Label) : Size(GetTextExtent(m_Label))) + padding;
		}
	}

	void Label::OnEnter(wxMouseEvent& event)
	{
		m_State = wxCONTROL_FOCUSED;
		ScheduleRefresh();
		event.Skip();
	}
	void Label::OnLeave(wxMouseEvent& event)
	{
		m_State = wxCONTROL_NONE;
		ScheduleRefresh();
		event.Skip();
	}
	void Label::OnMouseDown(wxMouseEvent& event)
	{
		m_State = wxCONTROL_PRESSED;
		ScheduleRefresh();
		event.Skip();
	}
	void Label::OnMouseUp(wxMouseEvent& event)
	{
		m_State = wxCONTROL_FOCUSED;
		ScheduleRefresh();
		event.Skip();

		if (m_Style & LabelStyle::Hyperlink)
		{
			wxTextUrlEvent evt(GetId(), event, 0, 0);
			evt.SetEventType(wxEVT_TEXT_URL);
			evt.SetEventObject(this);
			evt.SetString(m_Label);
			ProcessWindowEvent(evt);
		}
	}
	void Label::OnPaint(wxPaintEvent& event)
	{
		GDIPaintContext dc(*this);
		if (IsFrozen())
		{
			return;
		}

		const Rect rect(Point(0, 0), Size(GetSize()));
		const bool isSelected = m_State == wxCONTROL_FOCUSED || m_State == wxCONTROL_PRESSED;
		const bool isEnabled = IsThisEnabled();
		const Color color = GetStateColor();
		const String label = wxControl::Ellipsize(m_Label, dc.AsWXDC(), g_EllipsizeMode, rect.GetWidth(), wxELLIPSIZE_FLAGS_NONE);

		dc.SetBackgroundTransparent();
		dc.SetBackgroundBrush(GetParent()->GetBackgroundColour());
		UxTheme::DrawParentBackground(*this, dc, rect);

		// Draw main part
		if (m_Style & LabelStyle::InteractiveSelection && isSelected)
		{
			wxRendererNative::Get().DrawItemSelectionRect(this, dc.AsWXDC(), rect, wxCONTROL_FOCUSED|wxCONTROL_SELECTED);
		}

		if (m_Style & LabelStyle::Hyperlink && isSelected)
		{
			dc.SetFont(GetFont().MakeUnderlined());
		}

		GDIBitmap image = m_Icon;
		if (!isEnabled)
		{
			image = m_IconDisabled;
		}
		dc.SetTextForeground(color);
		Rect labelRect = Rect(rect);
		labelRect.SetLeft(2);

		Size contentSize;
		if (m_IsMultilne)
		{
			contentSize = CalcBestSize(&dc);

			int offset = 0;
			if (image)
			{
				Rect rect2 = labelRect;
				rect2.Y() += ConvertDialogToPixels(wxSize(0, 2)).GetHeight();

				dc.DrawLabel({}, rect2, image.AsWXBitmap(), m_MultiLineAlignStyle);
				offset = image.GetWidth() + ConvertDialogToPixels(wxSize(2, 0)).GetWidth();
			}

			Point pos = labelRect.GetPosition();
			pos.X() += offset;

			int spacingV = ConvertDialogToPixels(wxSize(0, 1)).GetHeight();
			pos.Y() += spacingV + spacingV / 2;
			dc.DrawText(label, pos);

			// Calculate best size
			if (image)
			{
				contentSize.Width() += image.GetWidth();
			}
			m_BestSize = contentSize;
		}
		else
		{
			contentSize = dc.GetTextExtent(label);
			contentSize.SetHeight(g_MinSingleLineHeight);
			dc.DrawLabel(label, labelRect, image.AsWXBitmap(), m_AlignStyle);
		}

		if (m_Style & LabelStyle::HeaderLine)
		{
			Point pos1 = Point(contentSize.GetWidth() + 5, rect.GetHeight() / 2 + 1);
			Point pos2 = Point(rect.GetWidth() - 4, rect.GetHeight() / 2 + 1);
			if (image)
			{
				pos1.X() += image.GetWidth() + 4;
			}

			Color lineColor = wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR);
			if (!isEnabled)
			{
				lineColor.MakeDisabled();
			}
			dc.SetPen(lineColor);
			dc.DrawLine(pos1, pos2);
		}
	}

	bool Label::Create(wxWindow* parent,
					   wxWindowID id,
					   const String& label,
					   FlagSet<LabelStyle> style
	)
	{
		m_Style = style;
		m_IsMultilne = IsLabelMultiline(label);
		SetBackgroundStyle(wxBG_STYLE_PAINT);
		PushEventHandler(&m_EvtHandler);

		if (wxStaticText::Create(parent, id, label, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt()|wxBORDER_NONE))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);
			SetLabel(label);

			wxColour color = GetForegroundColour();
			if (m_Style & LabelStyle::Caption)
			{
				auto font = GetFont();
				font.SetPointSize(font.GetPointSize() + 2);
				SetFont(font);
			}
			if (m_Style & LabelStyle::ThemeColors)
			{
				color = UxTheme::GetDialogMainInstructionColor(*this);
			}
			SetupColors(color);

			// Initial best size
			m_BestSize = CalcBestSize();

			// Events
			m_EvtHandler.Bind(wxEVT_PAINT, &Label::OnPaint, this);
			m_EvtHandler.Bind(wxEVT_ENTER_WINDOW, &Label::OnEnter, this);
			m_EvtHandler.Bind(wxEVT_LEAVE_WINDOW, &Label::OnLeave, this);
			m_EvtHandler.Bind(wxEVT_LEFT_DOWN, &Label::OnMouseDown, this);
			m_EvtHandler.Bind(wxEVT_LEFT_UP, &Label::OnMouseUp, this);
			return true;
		}
		return false;
	}

	void Label::SetBitmap(const GDIBitmap& image)
	{
		ScheduleRefresh();

		if (image)
		{
			m_Icon = image;
			m_IconDisabled = image.ConvertToDisabled();

			Size size = Size(GetSize());
			if (size.GetHeight() <= image.GetHeight())
			{
				SetSize(size.GetWidth(), image.GetHeight()+4);
			}
		}
		else
		{
			m_Icon = wxNullBitmap;
			m_IconDisabled = wxNullBitmap;
		}
	}
}
