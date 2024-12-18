#include "kxf-pch.h"
#include "StatusBarEx.h"
#include "kxf/Drawing/GDIRenderer/UxTheme.h"
#include "kxf/Drawing/GDIRenderer/Private/UxThemeDefines.h"
#include "kxf/Drawing/GDIRenderer/GDIWindowContext.h"
#include "kxf/Drawing/GDIRenderer/GDIMemoryContext.h"
#include "kxf/Utility/Common.h"

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(StatusBarEx, StatusBar);

	void StatusBarEx::OnPaint(wxPaintEvent& event)
	{
		GDIAutoBufferedPaintContext dc(*this);
		dc.SetBackgroundTransparent();
		dc.SetTextForeground(GetForegroundColour());

		const Size clientSize = Size(GetClientSize());
		UxTheme::DrawParentBackground(*this, dc, clientSize);

		// Draw the background
		{
			dc.SetBrush(GetBackgroundColour());

			Rect backgroundRect({0, 0}, clientSize);
			backgroundRect.Inflate(2);
			dc.DrawRectangle(backgroundRect);

			if (m_ProgressPos > 0 && m_ProgressRange > 0)
			{
				if (UxTheme theme(*this, UxThemeClass::Progress); theme)
				{
					theme.DrawProgressBar(dc, -1, PP_FILL, m_State & wxCONTROL_CURRENT ? PBFS_PARTIAL : PBFS_NORMAL, backgroundRect, m_ProgressPos, m_ProgressRange);
				}
				else
				{
					backgroundRect.SetWidth(m_ProgressPos == m_ProgressRange ? clientSize.GetWidth() : clientSize.GetWidth() * ((double)m_ProgressPos / m_ProgressRange));

					dc.SetBrush(Color(GetBackgroundColour()).RotateHue(Angle::FromDegrees(30)));
					dc.DrawRectangle(backgroundRect);
				}
			}
		}

		// Draw edge line
		if (m_BorderColor)
		{
			dc.SetPen(wxPen(m_BorderColor, 1));
			dc.DrawLine({0, 0}, {clientSize.GetWidth() + 1, 0});
		}

		// Draw size grip
		int sizeGripWidth = 0;
		const wxTopLevelWindow* topLevelWindow = GetTLWParent();
		if (topLevelWindow && !topLevelWindow->IsMaximized() && topLevelWindow->HasFlag(wxRESIZE_BORDER))
		{
			if (UxTheme theme(*this, UxThemeClass::Status); theme)
			{
				Size gripSize = theme.GetPartSize(dc, SP_GRIPPER, 0);
				Point gripPos(clientSize.GetWidth() - gripSize.GetWidth() - 1, GetMinHeight() - gripSize.GetHeight() - 1);
				theme.DrawBackground(dc, SP_GRIPPER, 0, Rect(gripPos, gripSize));
			}
			else
			{
				sizeGripWidth = clientSize.GetHeight();
			}
		}

		// Draw text
		int spacer = 3;
		int splitterX = 0;
		Rect rect(spacer, 0, 0, GetMinHeight());

		const size_t fieldsCount = GetFieldsCount();
		for (size_t i = 0; i < fieldsCount; i++)
		{
			if (wxRect tempRect; GetFieldRect(i, tempRect))
			{
				rect = Rect(tempRect);
				rect.SetX(rect.GetX() + spacer);

				String label;
				if (GetStatusWidth(i) != 0)
				{
					label = GetStatusText(i);
				}

				// Calc max width
				int maxWidth = rect.GetWidth() - 2 * GetCharWidth();
				if (i == fieldsCount - 1)
				{
					maxWidth -= sizeGripWidth;
				}

				// If icons are enabled update max width
				int imageIndex = Drawing::InvalidImageIndex;
				if (auto it = m_Images.find(i); it != m_Images.end())
				{
					imageIndex = it->second;
				}
				if (imageIndex != Drawing::InvalidImageIndex && HasImageList())
				{
					maxWidth -= GetImageList()->GetSize().GetWidth();
				}

				// Ellipsize label if needed
				if (!label.IsEmpty())
				{
					label = wxControl::Ellipsize(label, dc.ToWxDC(), GetEllipsizeMode(), maxWidth);
				}

				// Draw the label and/or icon
				if (imageIndex == Drawing::InvalidImageIndex || !HasImageList())
				{
					dc.DrawLabel(label, rect, Alignment::CenterVertical);
				}
				else
				{
					dc.DrawLabel(label, rect, GetImageList()->GetBitmap(imageIndex).ToWxBitmap(), Alignment::CenterVertical);
				}

				if (m_IsSeparatorsVisible && m_BorderColor && i != fieldsCount - 1)
				{
					splitterX += rect.GetWidth();
					dc.SetPen(m_BorderColor);
					dc.DrawLine(Point(splitterX, spacer), Point(splitterX, GetMinHeight() - spacer));
				}
			}
		}
	}
	void StatusBarEx::OnSize(wxSizeEvent& event)
	{
		ScheduleRefresh();
		event.Skip();
	}
	void StatusBarEx::OnMouseDown(wxMouseEvent& event)
	{
		m_State = wxCONTROL_SELECTED|wxCONTROL_CURRENT|wxCONTROL_FOCUSED;

		event.Skip();
		ScheduleRefresh();
	}
	void StatusBarEx::OnEnter(wxMouseEvent& event)
	{
		m_State = wxCONTROL_SELECTED|wxCONTROL_CURRENT;

		event.Skip();
		ScheduleRefresh();
	}
	void StatusBarEx::OnLeave(wxMouseEvent& event)
	{
		m_State = wxCONTROL_NONE;

		event.Skip();
		ScheduleRefresh();
	}

	wxEllipsizeMode StatusBarEx::GetEllipsizeMode() const
	{
		if (m_Style & StatusBarStyle::EllipsizeStart)
		{
			return wxEllipsizeMode::wxELLIPSIZE_START;
		}
		else if (m_Style & StatusBarStyle::EllipsizeMiddle)
		{
			return wxEllipsizeMode::wxELLIPSIZE_MIDDLE;
		}
		else if (m_Style & StatusBarStyle::EllipsizeEnd)
		{
			return wxEllipsizeMode::wxELLIPSIZE_END;
		}
		return wxEllipsizeMode::wxELLIPSIZE_NONE;
	}
	wxTopLevelWindow* StatusBarEx::GetTLWParent() const
	{
		wxWindow* parent = GetParent();
		if (parent && parent->IsKindOf(wxCLASSINFO(wxTopLevelWindow)))
		{
			return static_cast<wxTopLevelWindow*>(parent);
		}
		return nullptr;
	}

	void StatusBarEx::MakeTopmost()
	{
		::SetWindowPos(GetHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS|SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING|SWP_NOREDRAW);
	}

	bool StatusBarEx::Create(wxWindow* parent,
							 wxWindowID id,
							 int fieldsCount,
							 FlagSet<StatusBarStyle> style
	)
	{
		if (StatusBar::Create(parent, id, fieldsCount, style))
		{
			EnableSystemTheme();
			SetSeparatorsVisible();
			UseBackgroundColors();
			SetBackgroundStyle(wxBG_STYLE_PAINT);

			// Configure fields
			SetFieldsCount(fieldsCount);
			SetStatusWidths(std::vector<int>(fieldsCount, -1));
			for (int i = 0; i < GetFieldsCount(); i++)
			{
				m_Images[i] = Drawing::InvalidImageIndex;
			}

			Bind(wxEVT_PAINT, &StatusBarEx::OnPaint, this);
			Bind(wxEVT_SIZE, &StatusBarEx::OnSize, this);
			Bind(wxEVT_ENTER_WINDOW, &StatusBarEx::OnEnter, this);
			Bind(wxEVT_LEAVE_WINDOW, &StatusBarEx::OnLeave, this);
			Bind(wxEVT_LEFT_DOWN, &StatusBarEx::OnMouseDown, this);
			Bind(wxEVT_LEFT_UP, &StatusBarEx::OnEnter, this);

			return true;
		}
		return false;
	}

	bool StatusBarEx::SetForegroundColour(const wxColour& color)
	{
		ScheduleRefresh();
		return StatusBar::SetForegroundColour(color);
	}
	bool StatusBarEx::SetBackgroundColour(const wxColour& color)
	{
		ScheduleRefresh();

		if (color.IsOk())
		{
			return StatusBar::SetBackgroundColour(color);
		}
		else
		{
			return StatusBar::SetBackgroundColour(Color(GetParent()->GetBackgroundColour()).ChangeLightness(Angle::FromDegrees(110)));
		}
	}

	void StatusBarEx::SetMinHeight(int height)
	{
		StatusBar::SetMinHeight(height);
		wxFrame* parentFrame = dynamic_cast<wxFrame*>(GetParent());
		if (parentFrame)
		{
			SetMinSize(Size(wxDefaultCoord, height));
			parentFrame->SetStatusBar(this);
		}
		ScheduleRefresh();
	}

	void StatusBarEx::UseDefaultColors()
	{
		SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
		SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
		SetBorderColor(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));

		ScheduleRefresh();
	}
	void StatusBarEx::UseBackgroundColors()
	{
		SetBackgroundColour(Color(GetParent()->GetBackgroundColour()).ChangeLightness(Angle::FromDegrees(110)));
		SetForegroundColour(Color(GetBackgroundColour()).Negate());
		SetBorderColor(Color(GetParent()->GetBackgroundColour()).ChangeLightness(Angle::FromDegrees(75)));

		ScheduleRefresh();
	}
}
