#include "kxf-pch.h"
#include "GDIRendererNative.h"
#include "GDIAction.h"
#include "GDIFont.h"
#include "GDIBitmap.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/Core/Private/Mapping.h"
#include "../GraphicsRenderer.h"
#include "UxTheme.h"
#include "Private/UxThemeDefines.h"

namespace
{
	using namespace kxf;

	constexpr int GetListItemState(FlagSet<NativeWidgetFlag> flags) noexcept
	{
		if (flags.Contains(NativeWidgetFlag::Disabled))
		{
			return LISS_DISABLED;
		}

		int itemState = flags.Contains(NativeWidgetFlag::Current) ? LISS_HOT : LISS_NORMAL;
		if (flags.Contains(NativeWidgetFlag::Selected))
		{
			itemState = flags.Contains(NativeWidgetFlag::Current) ? LISS_HOTSELECTED : LISS_SELECTED;
			if (!flags.Contains(NativeWidgetFlag::Focused))
			{
				itemState = LISS_SELECTEDNOTFOCUS;
			}
		}
		return itemState;
	};
	constexpr wxHeaderSortIconType MapSortArrow(Direction sortArrow) noexcept
	{
		switch (sortArrow)
		{
			case Direction::Up:
			{
				return wxHDR_SORT_ICON_UP;
			}
			case Direction::Down:
			{
				return wxHDR_SORT_ICON_DOWN;
			}
		};
		return wxHDR_SORT_ICON_NONE;
	}

	wxHeaderButtonParams ConvertHeaderButtonInfo(const NativeHeaderButtonInfo& buttonInfo)
	{
		wxHeaderButtonParams parameters;
		parameters.m_arrowColour = buttonInfo.ArrowColor;
		parameters.m_labelColour = buttonInfo.LabelColor;
		parameters.m_selectionColour = buttonInfo.SelectionColor;

		parameters.m_labelText = buttonInfo.LabelText;
		parameters.m_labelFont = buttonInfo.LabelFont.ToGDIFont().ToWxFont();
		parameters.m_labelBitmap = buttonInfo.LabelBitmap.ToGDIBitmap().ToWxBitmap();
		parameters.m_labelAlignment = buttonInfo.LabelAlignment.ToInt();

		return parameters;
	}

	class CalcBoundingBox final
	{
		private:
			GDIContext& m_DC;
			Rect m_Rect;

		public:
			CalcBoundingBox(GDIContext& dc, const Rect& rect)
				:m_DC(dc), m_Rect(rect)
			{
			}
			~CalcBoundingBox()
			{
				m_DC.CalcBoundingBox(m_Rect);
			}

		public:
			void UpdateRect(const Rect& rect)
			{
				m_Rect = rect;
			}
			void Dismiss()
			{
				m_Rect = {};
			}
	};
}

namespace kxf
{
	wxRendererNative& GDIRendererNative::GetRenderer() const
	{
		return wxRendererNative::Get();
	}

	String GDIRendererNative::GetName() const
	{
		return "GDIRendererNative (UxTheme + wxRendererNative)";
	}
	Version GDIRendererNative::GetVersion() const
	{
		return {1, 0};
	}

	// CheckBox and checkmark
	void GDIRendererNative::DrawCheckBox(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		CalcBoundingBox calcBoudingBox(dc, rect);

		if (widgetFlags.Contains(NativeWidgetFlag::Radio))
		{
			return GetRenderer().DrawRadioBitmap(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
		}
		else
		{
			return GetRenderer().DrawCheckBox(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
		}
	}

	// Drop arrow for ComboBox
	void GDIRendererNative::DrawComboBoxDropButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		CalcBoundingBox calcBoudingBox(dc, rect);
		return GetRenderer().DrawComboBoxDropButton(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
	}

	// Collapse button (TaskDialog)
	void GDIRendererNative::DrawCollapseButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		CalcBoundingBox calcBoudingBox(dc, rect);
		return GetRenderer().DrawCollapseButton(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
	}

	// Push button
	void GDIRendererNative::DrawPushButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		CalcBoundingBox calcBoudingBox(dc, rect);
		return GetRenderer().DrawPushButton(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
	}

	// TreeView expander button
	Size GDIRendererNative::GetExpanderButtonSize(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags) const
	{
		if (window)
		{
			if (UxTheme theme(const_cast<wxWindow&>(*window), UxThemeClass::TreeView); theme)
			{
				const int partID = widgetFlags.Contains(NativeWidgetFlag::Current) ? TVP_HOTGLYPH : TVP_GLYPH;
				const int stateID = widgetFlags.Contains(NativeWidgetFlag::Expanded) ? GLPS_OPENED : GLPS_CLOSED;

				wxClientDC dc(const_cast<wxWindow*>(window));
				return theme.GetPartSize(dc, partID, stateID);
			}
			else
			{
				// They should be of the same size
				return Size(GetRenderer().GetCheckBoxSize(const_cast<wxWindow*>(window), *MapWidgetFlags(widgetFlags)));
			}
		}
		return {};
	}
	void GDIRendererNative::DrawExpanderButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		if (window)
		{
			CalcBoundingBox calcBoudingBox(dc, rect);

			if (widgetFlags.Contains(NativeWidgetFlag::Flat))
			{
				auto DoDrawExpander = [](wxWindow& window, GDIContext& dc, Rect rect, FlagSet<NativeWidgetFlag> widgetFlags)
				{
					const bool isActive = widgetFlags.Contains(NativeWidgetFlag::Current);
					const bool isDisabled = widgetFlags.Contains(NativeWidgetFlag::Disabled);
					const bool isExpanded = widgetFlags.Contains(NativeWidgetFlag::Expanded);
					const bool isPressed = widgetFlags.Contains(NativeWidgetFlag::Pressed);

					// Setup colors
					Color borderColor;
					Color glyphColor;
					if (isDisabled)
					{
						borderColor = System::GetColor(SystemColor::InactiveBroder);
						glyphColor = System::GetColor(SystemColor::InactiveCaptionText);
					}
					else if (isActive)
					{
						borderColor = System::GetColor(SystemColor::MenuHighlight);
						glyphColor = System::GetColor(SystemColor::ButtonText);
					}
					else if (isPressed)
					{
						borderColor = System::GetColor(SystemColor::LightHot);
						glyphColor = System::GetColor(SystemColor::ButtonText);
					}
					else
					{
						borderColor = System::GetColor(SystemColor::ButtonShadow);
						glyphColor = System::GetColor(SystemColor::ButtonText);
					}

					// Draw background
					dc.SetBrush(System::GetColor(SystemColor::ButtonFace));
					dc.SetPen(borderColor);
					dc.DrawRectangle(rect);

					// Draw plus/minus
					dc.SetPen(GDIPen(glyphColor, window.FromDIP(1)));

					const int length = std::min(rect.GetWidth(), rect.GetHeight()) - window.FromDIP(4) * 2;
					auto DrawLine = [&dc, lengthHalf = length / 2, base = rect.GetCenter()](Orientation orientation)
					{
						Point pos1 = base;
						Point pos2 = base;

						if (orientation == Orientation::Horizontal)
						{
							pos1.X() -= lengthHalf;
							pos2.X() += lengthHalf;
						}
						else if (orientation == Orientation::Vertical)
						{
							pos1.Y() -= lengthHalf;
							pos2.Y() += lengthHalf;
						}
						dc.DrawLine(pos1, pos2);
					};

					// Draw lines
					DrawLine(Orientation::Horizontal);
					if (!isExpanded)
					{
						DrawLine(Orientation::Vertical);
					}
				};
				DoDrawExpander(*window, dc, rect, widgetFlags);
			}
			else if (UxTheme theme(const_cast<wxWindow&>(*window), UxThemeClass::TreeView); theme)
			{
				const int partID = widgetFlags.Contains(NativeWidgetFlag::Current) ? TVP_HOTGLYPH : TVP_GLYPH;
				const int stateID = widgetFlags.Contains(NativeWidgetFlag::Expanded) ? GLPS_OPENED : GLPS_CLOSED;

				theme.DrawBackground(dc, partID, stateID, Rect(rect.GetPosition(), theme.GetPartSize(dc, partID, stateID)));
			}
			else
			{
				GetRenderer().DrawTreeItemButton(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
			}
		}
	}

	// Progress meter
	void GDIRendererNative::DrawProgressMeter(wxWindow* window, GDIContext& dc, const Rect& rect, int value, int range, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		if (window)
		{
			// Progress bar looks really ugly when it's smaller than 10x10 pixels, so don't draw it at all in this case.
			const Size minSize = Size(window->FromDIP(wxSize(10, 10)));
			if (rect.GetWidth() < minSize.GetWidth() || rect.GetHeight() < minSize.GetHeight())
			{
				return;
			}

			CalcBoundingBox calcBoudingBox(dc, rect);
			if (UxTheme theme(*window, UxThemeClass::Progress); theme)
			{
				const int bar = widgetFlags.Contains(NativeWidgetFlag::Vertical) ? PP_BARVERT : PP_BAR;
				const int fill = widgetFlags.Contains(NativeWidgetFlag::Vertical) ? PP_FILLVERT : PP_FILL;

				if (widgetFlags.Contains(NativeWidgetFlag::Paused))
				{
					theme.DrawProgressBar(dc, bar, fill, PBFS_PAUSED, rect, value, range);
				}
				else if (widgetFlags.Contains(NativeWidgetFlag::Error))
				{
					theme.DrawProgressBar(dc, bar, fill, PBFS_ERROR, rect, value, range);
				}
				else
				{
					theme.DrawProgressBar(dc, bar, fill, PBFS_NORMAL, rect, value, range);
				}
			}
			else
			{
				GetRenderer().DrawGauge(window, dc.ToWxDC(), rect, value, range, *MapWidgetFlags(widgetFlags));
			}
		}
	}

	// Item selection rectangle
	void GDIRendererNative::DrawItemSelectionRect(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		if (window)
		{
			// We'd better draw selection rect ourselves using UxTheme because wxWidgets draws windows background first
			// and this causes solid black background being drawn when a graphics renderer is used. The 'DrawItemSelectionRect'
			// function is the only function that does this.

			CalcBoundingBox calcBoudingBox(dc, rect);
			if (UxTheme theme(*window, UxThemeClass::ListView); theme)
			{
				const int part = LVP_LISTITEM;
				const int state = GetListItemState(widgetFlags);

				// Item selection rect is usually drawn *over* item's content and when it's not transparent we'll get
				// a solid rectangle that obscures the content.
				if (theme.IsBackgroundPartiallyTransparent(part, state))
				{
					theme.DrawBackground(dc, part, state, rect);
				}
				else
				{
					calcBoudingBox.Dismiss();
				}
			}
			else
			{
				GetRenderer().DrawItemSelectionRect(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
			}
		}
	}
	void GDIRendererNative::DrawItemFocusRect(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		if (window)
		{
			CalcBoundingBox calcBoudingBox(dc, rect);
			if (UxTheme theme(*window, UxThemeClass::ListView); theme)
			{
				float borderWidth = theme.GetInt(LVP_LISTITEM, 0, TMT_BORDERSIZE).value_or(1);
				GDIAction::ChangeBrush brushChange(dc, *wxTRANSPARENT_BRUSH);

				GDIPen pen(theme.GetColor(LVP_GROUPHEADERLINE, 0, TMT_EDGEDKSHADOWCOLOR), borderWidth);
				pen.SetDashStyle(DashStyle::Dot);
				GDIAction::ChangePen penChange(dc, pen);

				if (borderWidth > 1)
				{
					Rect newRect = rect.Clone().Deflate(std::round(borderWidth / 2.0f));
					dc.DrawRectangle(newRect);

					calcBoudingBox.UpdateRect(newRect);
				}
				else
				{
					dc.DrawRectangle(rect);
				}
			}
			else
			{
				GetRenderer().DrawFocusRect(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
			}
		}
	}
	void GDIRendererNative::DrawItemFocusRect(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		if (!window)
		{
			return;
		}

		if (UxTheme theme(*window, UxThemeClass::ListView); theme)
		{
			IGraphicsRenderer& renderer = gc.GetRenderer();

			float borderWidth = theme.GetInt(LVP_LISTITEM, 0, TMT_BORDERSIZE).value_or(1);
			auto pen = renderer.CreatePen(theme.GetColor(LVP_GROUPHEADERLINE, 0, TMT_EDGEDKSHADOWCOLOR), borderWidth);
			pen->SetStyle(PenStyle::Dash);
			pen->SetDashStyle(DashStyle::Dot);

			if (borderWidth > 1.0f)
			{
				Rect newRect = rect.Clone().Deflate(std::round(borderWidth / 2.0f));
				gc.DrawRectangle(newRect, renderer.GetTransparentBrush(), *pen);
			}
			else
			{
				gc.DrawRectangle(rect, renderer.GetTransparentBrush(), *pen);
			}
		}
	}

	// Item text
	void GDIRendererNative::DrawItemText(wxWindow* window, GDIContext& dc, const String& text, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags, FlagSet<Alignment> alignment, EllipsizeMode ellipsizeMode)
	{
		CalcBoundingBox calcBoudingBox(dc, rect);
		GetRenderer().DrawItemText(window, dc.ToWxDC(), text, rect, *Private::MapAlignment(alignment), *MapWidgetFlags(widgetFlags), static_cast<wxEllipsizeMode>(ellipsizeMode));
	}

	// Title bar button
	void GDIRendererNative::DrawTitleBarButton(wxWindow* window, GDIContext& dc, const Rect& rect, NativeTitleBarButton button, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		auto DoDraw = [&](wxTitleBarButton button)
		{
			CalcBoundingBox calcBoudingBox(dc, rect);
			GetRenderer().DrawTitleBarBitmap(window, dc.ToWxDC(), rect, button, *MapWidgetFlags(widgetFlags));
		};

		switch (button)
		{
			case NativeTitleBarButton::Help:
			{
				DoDraw(wxTITLEBAR_BUTTON_HELP);
				break;
			}
			case NativeTitleBarButton::Close:
			{
				DoDraw(wxTITLEBAR_BUTTON_CLOSE);
				break;
			}
			case NativeTitleBarButton::Maximize:
			{
				DoDraw(wxTITLEBAR_BUTTON_MAXIMIZE);
				break;
			}
			case NativeTitleBarButton::Minimize:
			{
				DoDraw(wxTITLEBAR_BUTTON_ICONIZE);
				break;
			}
			case NativeTitleBarButton::Restore:
			{
				DoDraw(wxTITLEBAR_BUTTON_RESTORE);
				break;
			}
		};
	}

	// ComboBox
	void GDIRendererNative::DrawComboBox(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		// Not sure if wxWidgets renderer makes any difference but if case it does let's respect the flag

		CalcBoundingBox calcBoudingBox(dc, rect);
		if (widgetFlags.Contains(NativeWidgetFlag::Editable))
		{
			GetRenderer().DrawComboBox(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
		}
		else
		{
			GetRenderer().DrawChoice(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
		}
	}

	// TextBox
	void GDIRendererNative::DrawTextBox(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		CalcBoundingBox calcBoudingBox(dc, rect);
		GetRenderer().DrawTextCtrl(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
	}

	// Splitter widget
	NativeSplitterInfo GDIRendererNative::GetSplitterInfo(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags) const
	{
		auto info = GetRenderer().GetSplitterParams(window);
		return {info.border, info.widthSash, info.isHotSensitive};
	}

	void GDIRendererNative::DrawSplitterBorder(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		CalcBoundingBox calcBoudingBox(dc, rect);
		GetRenderer().DrawSplitterBorder(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
	}
	void GDIRendererNative::DrawSplitterSash(wxWindow* window, GDIContext& dc, const Rect& rect, int position, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		GDIAction::Clip clip(dc, rect);

		CalcBoundingBox calcBoudingBox(dc, rect);
		GetRenderer().DrawSplitterSash(window, dc.ToWxDC(), rect.GetSize(), position, widgetFlags.Contains(NativeWidgetFlag::Vertical) ? wxVERTICAL : wxHORIZONTAL);
	}

	// Header widget
	int GDIRendererNative::GetHeaderButtonHeight(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags) const
	{
		return GetRenderer().GetHeaderButtonHeight(const_cast<wxWindow*>(window));
	}
	int GDIRendererNative::GetHeaderButtonMargin(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags) const
	{
		return GetRenderer().GetHeaderButtonMargin(const_cast<wxWindow*>(window));
	}

	int GDIRendererNative::DrawHeaderButton(wxWindow* window,
											GDIContext& dc,
											const Rect& rect,
											FlagSet<NativeWidgetFlag> widgetFlags,
											Direction sortArrow,
											const NativeHeaderButtonInfo* buttonInfo
	)
	{
		CalcBoundingBox calcBoudingBox(dc, rect);
		if (buttonInfo)
		{
			wxHeaderButtonParams parameters = ConvertHeaderButtonInfo(*buttonInfo);
			return GetRenderer().DrawHeaderButton(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags), MapSortArrow(sortArrow), &parameters);
		}
		else
		{
			return GetRenderer().DrawHeaderButton(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags), MapSortArrow(sortArrow));
		}
	}
	int GDIRendererNative::DrawHeaderButtonContent(wxWindow* window,
												   GDIContext& dc,
												   const Rect& rect,
												   FlagSet<NativeWidgetFlag> widgetFlags,
												   Direction sortArrow,
												   const NativeHeaderButtonInfo* buttonInfo
	)
	{
		CalcBoundingBox calcBoudingBox(dc, rect);
		if (buttonInfo)
		{
			wxHeaderButtonParams parameters = ConvertHeaderButtonInfo(*buttonInfo);
			return GetRenderer().DrawHeaderButtonContents(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags), MapSortArrow(sortArrow), &parameters);
		}
		else
		{
			return GetRenderer().DrawHeaderButtonContents(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags), MapSortArrow(sortArrow));
		}
	}
}
