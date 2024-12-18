#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/systhemectrl.h>
#include <wx/stattext.h>
#include <wx/renderer.h>

namespace kxf::UI
{
	enum class LabelStyle: uint32_t
	{
		None = 0,

		Caption = 1 << 0,
		HeaderLine = 1 << 1,
		InteractiveSelection = 1 << 2,
		Hyperlink = 1 << 3,
		ThemeColors = 1 << 4,
	};
}
namespace kxf
{
	class GDIContext;

	kxf_FlagSet_Declare(UI::LabelStyle);
}

namespace kxf::UI
{
	class KXF_API Label: public WindowRefreshScheduler<wxSystemThemedControl<wxStaticText>>
	{
		public:
			static constexpr FlagSet<LabelStyle> DefaultStyle = LabelStyle::None;

		private:
			wxEvtHandler m_EvtHandler;

			String m_Label;
			GDIBitmap m_Icon;
			GDIBitmap m_IconDisabled;
			Color m_ColorNormal;
			Color m_ColorHighlight;
			Color m_ColorClick;
			Color m_ColorDisabled;
			Size m_BestSize;

			FlagSet<LabelStyle> m_Style = DefaultStyle;
			int m_WrapLength = -1;
			int m_State = wxCONTROL_NONE;
			bool m_IsMultilne = false;
			FlagSet<Alignment> m_MultiLineAlignStyle = Alignment::Left|Alignment::Top;
			FlagSet<Alignment> m_AlignStyle = Alignment::CenterVertical;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnEnter(wxMouseEvent& event);
			void OnLeave(wxMouseEvent& event);
			void OnMouseDown(wxMouseEvent& event);
			void OnMouseUp(wxMouseEvent& event);

			Color GetStateColor() const;
			void SetupColors(const Color& color)
			{
				ScheduleRefresh();

				m_ColorNormal = color;
				m_ColorHighlight = color;
				m_ColorClick = color;
				m_ColorDisabled = wxColour(color).MakeDisabled();
			}
			bool IsLabelMultiline(const String& label)
			{
				return label.Find('\r') != wxNOT_FOUND || label.Find('\n') != wxNOT_FOUND;
			}
			Size CalcBestSize(GDIContext* dc = nullptr);

		protected:
			void DoEnable(bool enable) override
			{
				Refresh();
				return wxStaticText::DoEnable(enable);
			}
			wxSize DoGetBestSize() const override
			{
				return m_BestSize;
			}
			wxSize GetMinSize() const override
			{
				return DoGetBestSize();
			}
			void DoSetLabel(const String& label)
			{
				ScheduleRefresh();

				m_Label = label;
				m_IsMultilne = IsLabelMultiline(label);
				m_BestSize = CalcBestSize();
			}

		public:
			Label() = default;
			Label(wxWindow* parent,
				  wxWindowID id,
				  const String& label,
				  FlagSet<LabelStyle> style = DefaultStyle
			)
			{
				Create(parent, id, label, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& label,
						FlagSet<LabelStyle> style = DefaultStyle
			);
			~Label()
			{
				PopEventHandler();
			}

		public:
			long GetWindowStyleFlag() const override
			{
				return m_Style.ToInt() | wxStaticText::GetWindowStyleFlag();
			}
			void SetWindowStyleFlag(long style) override
			{
				m_Style = static_cast<LabelStyle>(style);
				wxStaticText::SetWindowStyleFlag(style);
			}
			void Wrap(int width)
			{
				ScheduleRefresh();
				m_WrapLength = width;
			}

			wxString GetLabel() const override
			{
				return m_Label;
			}
			void SetLabel(const wxString& label) override
			{
				DoSetLabel(label);
			}
			void SetLabelText(const wxString& label) override
			{
				DoSetLabel(label);
			}
			void SetLabelAlignment(FlagSet<Alignment> singleLine, FlagSet<Alignment> multiLine = Alignment::Left|Alignment::Top)
			{
				ScheduleRefresh();

				m_AlignStyle = singleLine;
				m_MultiLineAlignStyle = multiLine;
			}

			bool AcceptsFocus() const override
			{
				return false;
			}
			bool AcceptsFocusFromKeyboard() const override
			{
				return false;
			}

			GDIBitmap GetBitmap()
			{
				return m_Icon;
			}
			void SetBitmap(const GDIBitmap& image);

			bool SetForegroundColour(const wxColour& color) override
			{
				SetupColors(color);
				return wxStaticText::SetForegroundColour(color);
			}

			Color GetNormalColor()
			{
				return m_ColorNormal;
			}
			void SetNormalColor(const Color& color)
			{
				ScheduleRefresh();
				m_ColorNormal = color;
			}

			Color GetHighlightColor()
			{
				return m_ColorHighlight;
			}
			void SetHighlightColor(const Color& color)
			{
				ScheduleRefresh();
				m_ColorHighlight = color;
			}

			Color GetClickColor()
			{
				return m_ColorClick;
			}
			void SetClickColor(const Color& color)
			{
				ScheduleRefresh();
				m_ColorClick = color;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(Label);
	};
}
