#include "kxf-pch.h"
#include "Label.h"
#include "../../Events/WidgetTextEvent.h"
#include "kxf/System/SystemInformation.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"
#include "kxf-gui/Drawing/GDIRenderer/UxTheme.h"

namespace
{
	constexpr int g_MinSingleLineHeight = 23;
}

namespace kxf::WXUI
{
	Color Label::GetStateColor() const
	{
		Color color;

		auto UseAssignedOrDefault = [&](const Color& defaultColor = {}) -> Color
		{
			if (defaultColor)
			{
				return defaultColor;
			}
			else if (m_Style & LabelStyle::ThemeColors)
			{
				return UxTheme::GetDialogMainInstructionColor(*this);
			}
			return GetForegroundColour();
		};
		switch (m_State)
		{
			case NativeWidgetFlag::Focused:
			{
				color = UseAssignedOrDefault(m_ColorHighlight);
				break;
			}
			case NativeWidgetFlag::Pressed:
			{
				color = UseAssignedOrDefault(m_ColorClick);
				break;
			}
			default:
			{
				color = UseAssignedOrDefault();
				break;
			}
		};

		if (!IsEnabled())
		{
			color.MakeDisabled();
		}
		return color;
	}
	Size Label::CalcBestSize(std::shared_ptr<IGraphicsContext> gc)
	{
		const SizeF padding = m_Widget.DialogUnitsToPixels<Size>(3, 1);

		if (!gc)
		{
			auto renderer = m_RendererAware->GetActiveGraphicsRenderer();
			gc = renderer->CreateLegacyMeasuringContext(this);
		}
		return gc->GetTextExtent(m_Label) + padding;
	}

	void Label::OnPaint(wxPaintEvent& event)
	{
		IRendererNative& nativeRenderer = IRendererNative::Get();
		auto renderer = m_RendererAware->GetActiveGraphicsRenderer();

		auto gc = renderer->CreateLegacyWindowPaintContext(*this);
		gc->Clear(renderer->GetTransparentBrush());

		const bool isSelected = m_State == NativeWidgetFlag::Focused || m_State == NativeWidgetFlag::Pressed;
		const bool isEnabled = IsEnabled();
		const Rect rect = {{}, Size(GetClientSize())};
		const Color color = GetStateColor();
		const String label = gc->EllipsizeText(m_Label, rect.GetWidth(), EllipsizeMode::End);

		// Draw main part
		if (m_Style & LabelStyle::InteractiveSelection && isSelected)
		{
			nativeRenderer.DrawItemSelectionRect(this, *gc, rect, NativeWidgetFlag::Focused|NativeWidgetFlag::Selected);
		}

		if (m_Style & LabelStyle::Hyperlink && isSelected)
		{
			Font font = GetFont();
			font.AddStyle(FontStyle::Underline);

			gc->SetFont(renderer->CreateFont(font));
		}

		BitmapImage image = isEnabled ? m_Icon : m_Icon.ConvertToDisabled();
		Rect labelRect = Rect(rect);
		labelRect.SetLeft(2);

		Size contentSize;
		if (m_IsMultilne)
		{
			contentSize = CalcBestSize(gc);

			int offset = 0;
			if (image)
			{
				Rect rect2 = labelRect;
				rect2.Y() += ConvertDialogToPixels(wxSize(0, 2)).GetHeight();

				gc->DrawLabel({}, rect2, image, m_MultiLineAlignStyle);
				offset = image.GetWidth() + ConvertDialogToPixels(wxSize(2, 0)).GetWidth();
			}

			Point pos = labelRect.GetPosition();
			pos.X() += offset;

			int spacingV = ConvertDialogToPixels(wxSize(0, 1)).GetHeight();
			pos.Y() += spacingV + spacingV / 2;
			gc->DrawText(label, pos);

			// Calculate best size
			if (image)
			{
				contentSize.Width() += image.GetWidth();
			}
			m_BestSize = contentSize;
		}
		else
		{
			contentSize = gc->GetTextExtent(label);
			contentSize.SetHeight(g_MinSingleLineHeight);
			gc->DrawLabel(label, labelRect, image, m_AlignStyle);
		}

		if (m_Style & LabelStyle::HeaderLine)
		{
			Point pos1 = {contentSize.GetWidth() + 5, rect.GetHeight() / 2 + 1};
			Point pos2 = {rect.GetWidth() - 4, rect.GetHeight() / 2 + 1};
			if (image)
			{
				pos1.X() += image.GetWidth() + 4;
			}

			Color lineColor = System::GetColor(SystemColor::ScrollBar);
			if (!isEnabled)
			{
				lineColor.MakeDisabled();
			}

			auto pen = renderer->CreatePen(lineColor);
			gc->DrawLine(pos1, pos2, *pen);
		}
	}
	void Label::OnEnter(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_State = NativeWidgetFlag::Focused;

		event.Skip();
	}
	void Label::OnLeave(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_State = NativeWidgetFlag::None;

		event.Skip();
	}
	void Label::OnMouseDown(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_State = NativeWidgetFlag::Pressed;

		event.Skip();
	}
	void Label::OnMouseUp(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_State = NativeWidgetFlag::Focused;

		event.Skip();
		if (m_Style & LabelStyle::Hyperlink && m_URI)
		{
			WidgetMouseEvent mouseEvent(m_Widget, event);
			m_Widget.ProcessEvent(WidgetTextEvent::EvtURI, m_Widget, m_URI, std::move(mouseEvent));
		}
	}

	bool Label::Create(wxWindow* parent,
							 const String& label,
							 const Point& pos,
							 const Size& size,
							 FlagSet<LabelStyle> style)
	{
		m_Style = style;
		m_IsMultilne = IsLabelMultiline(label);
		SetBackgroundStyle(wxBG_STYLE_PAINT);

		if (wxStaticText::Create(parent, wxID_NONE, label, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), wxBORDER_NONE))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);
			SetLabel(label);

			Color color = GetForegroundColour();
			if (m_Style & LabelStyle::Caption)
			{
				auto font = GetFont();
				font.SetPointSize(font.GetPointSize() + 2);
				SetFont(font);
			}

			// Initial best size
			m_BestSize = CalcBestSize();

			// Events
			m_EvtHandler.Bind(wxEVT_PAINT, &Label::OnPaint, this);
			m_EvtHandler.Bind(wxEVT_ENTER_WINDOW, &Label::OnEnter, this);
			m_EvtHandler.Bind(wxEVT_LEAVE_WINDOW, &Label::OnLeave, this);
			m_EvtHandler.Bind(wxEVT_LEFT_DOWN, &Label::OnMouseDown, this);
			m_EvtHandler.Bind(wxEVT_LEFT_UP, &Label::OnMouseUp, this);

			PushEventHandler(&m_EvtHandler);
			m_EvtHandler.SetClientData(this);

			return m_Widget.QueryInterface(m_RendererAware);
		}
		return false;
	}

	void Label::SetBitmap(const BitmapImage& image)
	{
		ScheduleRefresh();

		if (image)
		{
			m_Icon = image;

			Size size = Size(GetSize());
			if (size.GetHeight() <= image.GetHeight())
			{
				SetSize(size.GetWidth(), image.GetHeight() + 4);
			}
		}
		else
		{
			m_Icon = {};
		}
	}
}
