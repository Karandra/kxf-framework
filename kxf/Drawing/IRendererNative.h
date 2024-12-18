#pragma once
#include "Common.h"
#include "kxf/Core/Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/Version.h"
#include "kxf/RTTI/RTTI.h"
#include "BitmapImage.h"
#include "Font.h"
class wxWindow;

namespace kxf
{
	class GDIContext;
	class IGraphicsContext;
}

namespace kxf
{
	enum class NativeWidgetFlag: uint64_t
	{
		None = 0,

		// Generic [0, 15]
		Current = 1 << 0,
		Selected = 1 << 1,
		Focused = 1 << 2,
		Pressed = 1 << 3,
		Disabled = 1 << 4,
		Expanded = 1 << 5,
		Editable = 1 << 6,

		// CheckBox, RadioBox, etc [16, 23]
		Radio = 1 << 17,
		Checked = 1 << 18,
		Checkable = 1 << 19,
		Indeterminate = 1 << 20,

		// Special [24, 31]
		DefaultItem = 1 << 24,
		Vertical = 1 << 25,
		CellItem = 1 << 26,
		SizeGrip = 1 << 27,
		SubMenu = 1 << 28,
		Paused = 1 << 29,
		Error = 1 << 30,
		Flat = 1 << 31

		// Reserved [32, 63]
	};
	kxf_FlagSet_Declare(NativeWidgetFlag);

	enum class NativeTitleBarButton: uint32_t
	{
		None = 0,

		Help,
		Close,
		Maximize,
		Minimize,
		Restore
	};
	struct NativeHeaderButtonInfo final
	{
		Color ArrowColor;
		Color SelectionColor;
		Color LabelColor;
		String LabelText;
		Font LabelFont;
		BitmapImage LabelBitmap;
		FlagSet<Alignment> LabelAlignment = Alignment::Left|Alignment::CenterVertical;
	};

	struct NativeSplitterInfo final
	{
		int BorderWidth = 0;
		int SashWidth = 0;
		bool HotSensitive = false;
	};
}

namespace kxf
{
	class KXF_API IRendererNative: public RTTI::Interface<IRendererNative>
	{
		kxf_RTTI_DeclareIID(IRendererNative, {0x4fb9dfc6, 0xfba2, 0x4b99, {0x83, 0x92, 0x38, 0xd3, 0x26, 0x71, 0x1b, 0x6c}});

		public:
			static IRendererNative& Get();

			static FlagSet<NativeWidgetFlag> MapWidgetFlags(FlagSet<int> flagsWx) noexcept;
			static FlagSet<int> MapWidgetFlags(FlagSet<NativeWidgetFlag> flags) noexcept;

		public:
			virtual ~IRendererNative() = default;

		public:
			virtual String GetName() const = 0;
			virtual Version GetVersion() const = 0;

		public:
			// CheckBox and checkmark
			virtual Size GetCheckMarkSize(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const = 0;
			virtual Size GetCheckBoxSize(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const = 0;
			virtual void DrawCheckBox(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawCheckBox(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// Drop arrow for ComboBox
			virtual void DrawComboBoxDropButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawComboBoxDropButton(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// Drop arrow (the same as in ComboBox but can be used without its background)
			virtual void DrawDropArrow(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawDropArrow(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// Collapse button (TaskDialog)
			virtual Size GetCollapseButtonSize(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const = 0;
			virtual void DrawCollapseButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawCollapseButton(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// Push button
			virtual void DrawPushButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawPushButton(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// TreeView expander button
			virtual Size GetExpanderButtonSize(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const = 0;
			virtual void DrawExpanderButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawExpanderButton(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// Progress meter
			virtual void DrawProgressMeter(wxWindow* window, GDIContext& dc, const Rect& rect, int value, int range, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawProgressMeter(wxWindow* window, IGraphicsContext& gc, const Rect& rect, int value, int range, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// Item selection rectangle
			virtual void DrawItemSelectionRect(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawItemSelectionRect(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			virtual void DrawItemFocusRect(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawItemFocusRect(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// Item text
			virtual void DrawItemText(wxWindow* window, GDIContext& dc, const String& text, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}, FlagSet<Alignment> alignment = {}, EllipsizeMode ellipsizeMode = EllipsizeMode::End) = 0;
			virtual void DrawItemText(wxWindow* window, IGraphicsContext& gc, const String& text, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}, FlagSet<Alignment> alignment = {}, EllipsizeMode ellipsizeMode = EllipsizeMode::End);

			// Title bar button
			virtual void DrawTitleBarButton(wxWindow* window, GDIContext& dc, const Rect& rect, NativeTitleBarButton button, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawTitleBarButton(wxWindow* window, IGraphicsContext& gc, const Rect& rect, NativeTitleBarButton button, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// ComboBox
			virtual void DrawComboBox(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawComboBox(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// TextBox
			virtual void DrawTextBox(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawTextBox(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// Splitter widget
			virtual NativeSplitterInfo GetSplitterInfo(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const = 0;

			virtual void DrawSplitterBorder(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawSplitterBorder(wxWindow* window, IGraphicsContext& gc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags = {});

			virtual void DrawSplitterSash(wxWindow* window, GDIContext& dc, const Rect& rect, int position, FlagSet<NativeWidgetFlag> widgetFlags = {}) = 0;
			virtual void DrawSplitterSash(wxWindow* window, IGraphicsContext& gc, const Rect& rect, int position, FlagSet<NativeWidgetFlag> widgetFlags = {});

			// Header widget
			virtual int GetHeaderButtonHeight(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const = 0;
			virtual int GetHeaderButtonMargin(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags = {}) const = 0;

			virtual int DrawHeaderButton(wxWindow* window,
										 GDIContext& dc,
										 const Rect& rect,
										 FlagSet<NativeWidgetFlag> widgetFlags = {},
										 Direction sortArrow = Direction::None,
										 const NativeHeaderButtonInfo* buttonInfo = nullptr
			) = 0;
			virtual int DrawHeaderButton(wxWindow* window,
										 IGraphicsContext& gc,
										 const Rect& rect,
										 FlagSet<NativeWidgetFlag> widgetFlags = {},
										 Direction sortArrow = Direction::None,
										 const NativeHeaderButtonInfo* buttonInfo = nullptr
			);

			virtual int DrawHeaderButtonContent(wxWindow* window,
												GDIContext& dc,
												const Rect& rect,
												FlagSet<NativeWidgetFlag> widgetFlags = {},
												Direction sortArrow = Direction::None,
												const NativeHeaderButtonInfo* buttonInfo = nullptr
			) = 0;
			virtual int DrawHeaderButtonContent(wxWindow* window,
												IGraphicsContext& gc,
												const Rect& rect,
												FlagSet<NativeWidgetFlag> widgetFlags = {},
												Direction sortArrow = Direction::None,
												const NativeHeaderButtonInfo* buttonInfo = nullptr
			);
	};
}
