#pragma once
#include "Common.h"
#include "GDIContext.h"
#include "../IRendererNative.h"
#include <wx/renderer.h>

namespace kxf
{
	class KXF_API GDIRendererNative: public IRendererNative
	{
		private:
			wxRendererNative& GetRenderer() const;

		public:
			GDIRendererNative() = default;

		public:
			String GetName() const override;
			Version GetVersion() const override;

		public:
			// CheckBox and checkmark
			Size GetCheckMarkSize(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const override
			{
				return Size(GetRenderer().GetCheckMarkSize(const_cast<wxWindow*>(window)));
			}
			Size GetCheckBoxSize(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const override
			{
				return Size(GetRenderer().GetCheckBoxSize(const_cast<wxWindow*>(window), *MapWidgetFlags(widgetFlags)));
			}
			void DrawCheckBox(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// Drop arrow for ComboBox
			void DrawComboBoxDropButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// Drop arrow (the same as in ComboBox but can be used without its background)
			void DrawDropArrow(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override
			{
				return GetRenderer().DrawDropArrow(window, dc.AsWXDC(), rect, *MapWidgetFlags(widgetFlags));
			}

			// Collapse button (TaskDialog)
			Size GetCollapseButtonSize(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const override
			{
				wxClientDC dc(const_cast<wxWindow*>(window));
				return Size(GetRenderer().GetCollapseButtonSize(const_cast<wxWindow*>(window), dc));
			}
			void DrawCollapseButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// Push button
			void DrawPushButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// TreeView expander button
			Size GetExpanderButtonSize(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const override;
			void DrawExpanderButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// Progress meter
			void DrawProgressMeter(wxWindow* window, GDIContext& dc, const Rect& rect, int value, int range, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// Item selection rectangle
			void DrawItemSelectionRect(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;
			void DrawItemFocusRect(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;
			void DrawItemFocusRect(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// Item text
			void DrawItemText(wxWindow* window, GDIContext& dc, const String& text, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}, FlagSet<Alignment> alignment = {}, EllipsizeMode ellipsizeMode = EllipsizeMode::End) override;

			// Title bar button
			void DrawTitleBarButton(wxWindow* window, GDIContext& dc, const Rect& rect, NativeTitleBarButton button, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// ComboBox
			void DrawComboBox(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// TextBox
			void DrawTextBox(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// Splitter widget
			NativeSplitterInfo GetSplitterInfo(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const override;

			void DrawSplitterBorder(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;
			void DrawSplitterSash(wxWindow* window, GDIContext& dc, const Rect& rect, int position, FlagSet<NativeWidgetFlag> widgetFlags = {}) override;

			// Header widget
			int GetHeaderButtonHeight(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const override;
			int GetHeaderButtonMargin(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const override;

			int DrawHeaderButton(wxWindow* window,
								 GDIContext& dc,
								 const Rect& rect,
								 FlagSet<NativeWidgetFlag> widgetFlags = {},
								 Direction sortArrow = Direction::None,
								 const NativeHeaderButtonInfo* buttonInfo = nullptr
			) override;

			int DrawHeaderButtonContent(wxWindow* window,
										GDIContext& dc,
										const Rect& rect,
										FlagSet<NativeWidgetFlag> widgetFlags = {},
										Direction sortArrow = Direction::None,
										const NativeHeaderButtonInfo* buttonInfo = nullptr
			) override;
	};
}
