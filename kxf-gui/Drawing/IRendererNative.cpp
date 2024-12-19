#include "kxf-pch.h"
#include "IRendererNative.h"
#include "GraphicsRenderer.h"
#include "GDIRenderer/GDIRendererNative.h"

namespace
{
	kxf::Rect ResetPosition(const kxf::Rect& rect) noexcept
	{
		return rect.Clone().SetPosition({0, 0});
	}
}

namespace kxf
{
	IRendererNative& IRendererNative::Get()
	{
		static GDIRendererNative instance;
		return instance;
	}

	FlagSet<NativeWidgetFlag> IRendererNative::MapWidgetFlags(FlagSet<int> flagsWx) noexcept
	{
		FlagSet<NativeWidgetFlag> flags;

		flags.Add(NativeWidgetFlag::Current, flagsWx.Contains(wxCONTROL_CURRENT));
		flags.Add(NativeWidgetFlag::Selected, flagsWx.Contains(wxCONTROL_SELECTED));
		flags.Add(NativeWidgetFlag::Disabled, flagsWx.Contains(wxCONTROL_DISABLED));
		flags.Add(NativeWidgetFlag::Focused, flagsWx.Contains(wxCONTROL_FOCUSED));
		flags.Add(NativeWidgetFlag::Pressed, flagsWx.Contains(wxCONTROL_PRESSED));

		flags.Add(NativeWidgetFlag::Checked, flagsWx.Contains(wxCONTROL_CHECKED));
		flags.Add(NativeWidgetFlag::Checkable, flagsWx.Contains(wxCONTROL_CHECKABLE));
		flags.Add(NativeWidgetFlag::Indeterminate, flagsWx.Contains(wxCONTROL_UNDETERMINED));

		flags.Add(NativeWidgetFlag::Radio, flagsWx.Contains(wxCONTROL_SPECIAL));
		flags.Add(NativeWidgetFlag::Flat, flagsWx.Contains(wxCONTROL_FLAT));
		flags.Add(NativeWidgetFlag::SubMenu, flagsWx.Contains(wxCONTROL_ISSUBMENU));
		flags.Add(NativeWidgetFlag::CellItem, flagsWx.Contains(wxCONTROL_CELL));
		flags.Add(NativeWidgetFlag::Expanded, flagsWx.Contains(wxCONTROL_EXPANDED));
		flags.Add(NativeWidgetFlag::SizeGrip, flagsWx.Contains(wxCONTROL_SIZEGRIP));
		flags.Add(NativeWidgetFlag::DefaultItem, flagsWx.Contains(wxCONTROL_ISDEFAULT));

		return flags;
	}
	FlagSet<int> IRendererNative::MapWidgetFlags(FlagSet<NativeWidgetFlag> flags) noexcept
	{
		FlagSet<int> flagsWx;

		flagsWx.Add(wxCONTROL_CURRENT, flags.Contains(NativeWidgetFlag::Current));
		flagsWx.Add(wxCONTROL_SELECTED, flags.Contains(NativeWidgetFlag::Selected));
		flagsWx.Add(wxCONTROL_DISABLED, flags.Contains(NativeWidgetFlag::Disabled));
		flagsWx.Add(wxCONTROL_FOCUSED, flags.Contains(NativeWidgetFlag::Focused));
		flagsWx.Add(wxCONTROL_PRESSED, flags.Contains(NativeWidgetFlag::Pressed));

		flagsWx.Add(wxCONTROL_UNDETERMINED, flags.Contains(NativeWidgetFlag::Indeterminate));
		flagsWx.Add(wxCONTROL_CHECKABLE, flags.Contains(NativeWidgetFlag::Checkable));
		flagsWx.Add(wxCONTROL_CHECKED, flags.Contains(NativeWidgetFlag::Checked));
		flagsWx.Add(wxCONTROL_FLAT, flags.Contains(NativeWidgetFlag::Flat));

		flagsWx.Add(wxCONTROL_ISDEFAULT, flags.Contains(NativeWidgetFlag::DefaultItem));
		flagsWx.Add(wxCONTROL_ISSUBMENU, flags.Contains(NativeWidgetFlag::SubMenu));
		flagsWx.Add(wxCONTROL_SIZEGRIP, flags.Contains(NativeWidgetFlag::SizeGrip));
		flagsWx.Add(wxCONTROL_EXPANDED, flags.Contains(NativeWidgetFlag::Expanded));
		flagsWx.Add(wxCONTROL_CELL, flags.Contains(NativeWidgetFlag::CellItem));
		// No wx flag for 'NativeWidgetFlag::Radio' option

		return flagsWx;
	}

	// CheckBox and checkmark
	void IRendererNative::DrawCheckBox(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawCheckBox(window, dc, ResetPosition(rect), widgetFlags);
		});
	}

	// Drop arrow for ComboBox
	void IRendererNative::DrawComboBoxDropButton(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawComboBoxDropButton(window, dc, ResetPosition(rect), widgetFlags);
		});
	}

	// Drop arrow (the same as in ComboBox but can be used without its background)
	void IRendererNative::DrawDropArrow(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawDropArrow(window, dc, ResetPosition(rect), widgetFlags);
		});
	}

	// Collapse button (TaskDialog)
	void IRendererNative::DrawCollapseButton(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawCollapseButton(window, dc, ResetPosition(rect), widgetFlags);
		});
	}

	// Push button
	void IRendererNative::DrawPushButton(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawPushButton(window, dc, ResetPosition(rect), widgetFlags);
		});
	}

	// TreeView expander button
	void IRendererNative::DrawExpanderButton(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawExpanderButton(window, dc, ResetPosition(rect), widgetFlags);
		});
	}

	// Progress meter
	void IRendererNative::DrawProgressMeter(wxWindow* window, IGraphicsContext& gc, const Rect& rect, int value, int range, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawProgressMeter(window, dc, ResetPosition(rect), value, range, widgetFlags);
		});
	}

	// Item selection rectangle
	void IRendererNative::DrawItemSelectionRect(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawItemSelectionRect(window, dc, ResetPosition(rect), widgetFlags);
		});
	}
	void IRendererNative::DrawItemFocusRect(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawItemFocusRect(window, dc, ResetPosition(rect), widgetFlags);
		});
	}

	// Item text
	void IRendererNative::DrawItemText(wxWindow* window, IGraphicsContext& gc, const String& text, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags, FlagSet<Alignment> alignment, EllipsizeMode ellipsizeMode)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawItemText(window, dc, text, ResetPosition(rect), widgetFlags, alignment, ellipsizeMode);
		});
	}

	// Title bar button
	void IRendererNative::DrawTitleBarButton(wxWindow* window, IGraphicsContext& gc, const Rect& rect, NativeTitleBarButton button, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawTitleBarButton(window, dc, ResetPosition(rect), button, widgetFlags);
		});
	}

	// ComboBox
	void IRendererNative::DrawComboBox(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawComboBox(window, dc, ResetPosition(rect), widgetFlags);
		});
	}

	// TextBox
	void IRendererNative::DrawTextBox(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawTextBox(window, dc, ResetPosition(rect), widgetFlags);
		});
	}

	// Splitter widget
	void IRendererNative::DrawSplitterBorder(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawSplitterBorder(window, dc, ResetPosition(rect), widgetFlags);
		});
	}
	void IRendererNative::DrawSplitterSash(wxWindow* window, IGraphicsContext& gc, const Rect& rect, int position, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			DrawSplitterSash(window, dc, ResetPosition(rect), position, widgetFlags);
		});
	}

	// Header widget
	int IRendererNative::DrawHeaderButton(wxWindow* window,
										  IGraphicsContext& gc,
										  const Rect& rect,
										  FlagSet<NativeWidgetFlag> widgetFlags,
										  Direction sortArrow,
										  const NativeHeaderButtonInfo* buttonInfo
	)
	{
		int result = 0;
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			result = DrawHeaderButton(window, dc, ResetPosition(rect), widgetFlags, sortArrow, buttonInfo);
		});
		return result;
	}
	int IRendererNative::DrawHeaderButtonContent(wxWindow* window,
												 IGraphicsContext& gc,
												 const Rect& rect,
												 FlagSet<NativeWidgetFlag> widgetFlags,
												 Direction sortArrow,
												 const NativeHeaderButtonInfo* buttonInfo
	)
	{
		int result = 0;
		gc.DrawGDI(rect, [&](GDIContext& dc)
		{
			result = DrawHeaderButtonContent(window, dc, ResetPosition(rect), widgetFlags, sortArrow, buttonInfo);
		});
		return result;
	}
}
