#include "kxf-pch.h"
#include "StatusBarEx.h"
#include "kxf/Utility/Common.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"
#include "kxf-gui/Drawing/GDIRenderer/UxTheme.h"
#include "kxf/Win32/Include-GUI.h"

namespace kxf::WXUI
{
	void StatusBarEx::OnPaint(wxPaintEvent& event)
	{
		auto renderer = m_RendererAware->GetActiveGraphicsRenderer();
		auto gc = renderer->CreateLegacyWindowPaintContext(*this);

		auto backgroundBrush = renderer->CreateSolidBrush(GetBackgroundColour());
		auto foregroundBrush = renderer->CreateSolidBrush(GetForegroundColour());
		gc->Clear(renderer->GetTransparentBrush());

		const auto clientRect = RectF(GetClientRect());
		IRendererNative& nativeRenderer = IRendererNative::Get();

		// Draw progress bar
		if (m_ProgressPos > 0 && m_ProgressRange > 0)
		{
			nativeRenderer.DrawProgressMeter(this, *gc, clientRect, m_ProgressPos, m_ProgressRange);
		}

		// Draw edge line
		auto borderPen = renderer->CreatePen(m_BorderColor);
		if (m_BorderColor)
		{
			gc->DrawLine(clientRect.GetPosition(), {clientRect.GetWidth() + FromDIP(1), 0.0f}, *borderPen);
		}

		// Draw size grip
		int sizeGripWidth = 0;
		const wxTopLevelWindow* topLevelWindow = GetTLWParent();
		if (topLevelWindow && !topLevelWindow->IsMaximized() && topLevelWindow->HasFlag(wxRESIZE_BORDER))
		{
			if (UxTheme theme(*this, UxThemeClass::Status); theme)
			{
				gc->DrawGDI(clientRect, [&](GDIContext& dc)
				{
					Size gripSize = theme.GetPartSize(dc, SP_GRIPPER, 0);
					int spacing = FromDIP(1);

					Point gripPos(clientRect.GetWidth() - gripSize.GetWidth() - spacing, GetMinHeight() - gripSize.GetHeight() - spacing);
					theme.DrawBackground(dc, SP_GRIPPER, 0, {gripPos, gripSize});
				});
			}
			else
			{
				sizeGripWidth = clientRect.GetHeight();
			}
		}

		// Draw pane text
		float spacer = FromDIP(3);
		float splitterX = 0;
		float charWidth = gc->GetTextExtent("W").GetWidth();

		const size_t paneCount = GetPaneCount();
		for (size_t i = 0; i < paneCount; i++)
		{
			if (auto rect = GetPaneRect(i); rect.IsFullySpecified())
			{
				rect.X() += spacer;

				// Calc max width
				float maxWidth = rect.GetWidth() - (2 * charWidth);
				if (i == paneCount - 1)
				{
					maxWidth -= sizeGripWidth;
				}

				// If icons are enabled update max width
				const IImage2D* image = m_Icons[i].get();
				if (image)
				{
					maxWidth -= image->GetWidth();
				}

				// Draw the label and/or icon
				String label = gc->EllipsizeText(GetPaneLabel(i), maxWidth, GetEllipsizeMode());
				if (image)
				{
					gc->DrawLabel(label, rect, *image, Alignment::CenterVertical);
				}
				else
				{
					gc->DrawLabel(label, rect, Alignment::CenterVertical);
				}

				if (m_IsSeparatorsVisible && m_BorderColor && i != paneCount - 1)
				{
					splitterX += rect.GetWidth();
					gc->DrawLine({splitterX, spacer}, {splitterX, GetMinHeight() - spacer}, *borderPen);
				}
			}
		}
	}
	void StatusBarEx::OnSize(wxSizeEvent& event)
	{
		Refresh();
		event.Skip();
	}
	void StatusBarEx::OnEnter(wxMouseEvent& event)
	{
		m_WidgetState = NativeWidgetFlag::Selected|NativeWidgetFlag::Current;
		ScheduleRefresh();

		event.Skip();
	}
	void StatusBarEx::OnLeave(wxMouseEvent& event)
	{
		m_WidgetState = NativeWidgetFlag::None;
		ScheduleRefresh();

		event.Skip();
	}

	EllipsizeMode StatusBarEx::GetEllipsizeMode() const
	{
		FlagSet style = GetWindowStyle();
		if (style.Contains(wxSTB_ELLIPSIZE_START))
		{
			return EllipsizeMode::Start;
		}
		else if (style.Contains(wxSTB_ELLIPSIZE_MIDDLE))
		{
			return EllipsizeMode::Middle;
		}
		else if (style.Contains(wxSTB_ELLIPSIZE_END))
		{
			return EllipsizeMode::End;
		}
		return EllipsizeMode::None;
	}
	wxTopLevelWindow* StatusBarEx::GetTLWParent() const
	{
		wxWindow* parent = ::wxGetTopLevelParent(const_cast<StatusBarEx*>(this));
		if (parent && parent->IsKindOf(wxCLASSINFO(wxTopLevelWindow)))
		{
			return static_cast<wxTopLevelWindow*>(parent);
		}
		return nullptr;
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

	bool StatusBarEx::Create(wxWindow* parent,
							 const String& label,
							 const Point& pos,
							 const Size& size
	)
	{
		if (StatusBar::Create(parent, label, pos, size))
		{
			EnableSystemTheme();
			SetSeparatorsVisible();
			UseBackgroundColors();
			SetBackgroundStyle(wxBG_STYLE_PAINT);

			Bind(wxEVT_PAINT, &StatusBarEx::OnPaint, this);
			Bind(wxEVT_SIZE, &StatusBarEx::OnSize, this);
			Bind(wxEVT_ENTER_WINDOW, &StatusBarEx::OnEnter, this);
			Bind(wxEVT_LEAVE_WINDOW, &StatusBarEx::OnLeave, this);

			return m_Widget.QueryInterface(m_RendererAware);
		}
		return false;
	}

	// wxWindow
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

	// StatusBarEx
	void StatusBarEx::SetPaneIcon(size_t index, const IImage2D& icon)
	{
		if (index < m_Icons.size())
		{
			if (icon)
			{
				m_Icons[index] = icon.CloneImage2D();
			}
			else
			{
				m_Icons[index] = nullptr;
			}
			ScheduleRefresh();
		}
	}
}
