#pragma once
#include "Common.h"
#include "../../ILabelWidget.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include "kxf/Drawing/IRendererNative.h"
#include "kxf/Network/URI.h"
#include <wx/systhemectrl.h>
#include <wx/stattext.h>
#include <wx/renderer.h>

namespace kxf::WXUI
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
	kxf_FlagSet_Declare(WXUI::LabelStyle);
}

namespace kxf::WXUI
{
	class KXF_API Label: public EvtHandlerWrapper<Label, UI::WindowRefreshScheduler<wxSystemThemedControl<wxStaticText>>>
	{
		private:
			wxEvtHandler m_EvtHandler;
			ILabelWidget& m_Widget;
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;

			String m_Label;
			URI m_URI;
			BitmapImage m_Icon;
			Color m_ColorHighlight;
			Color m_ColorClick;
			Size m_BestSize;

			int m_WrapLength = -1;
			bool m_IsMultilne = false;
			FlagSet<LabelStyle> m_Style;
			NativeWidgetFlag m_State = NativeWidgetFlag::None;
			FlagSet<Alignment> m_MultiLineAlignStyle = Alignment::Left|Alignment::Top;
			FlagSet<Alignment> m_AlignStyle = Alignment::CenterVertical;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnEnter(wxMouseEvent& event);
			void OnLeave(wxMouseEvent& event);
			void OnMouseDown(wxMouseEvent& event);
			void OnMouseUp(wxMouseEvent& event);

			Color GetStateColor() const;
			bool IsLabelMultiline(const String& label)
			{
				return label.Contains('\r')  || label.Contains('\n');
			}
			Size CalcBestSize(std::shared_ptr<IGraphicsContext> gc = nullptr);

		protected:
			// wxWindow
			void DoEnable(bool enable) override
			{
				ScheduleRefresh();
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
			void DoSetLabel(String label)
			{
				ScheduleRefresh();

				m_Label = std::move(label);
				m_IsMultilne = IsLabelMultiline(m_Label);
				m_BestSize = CalcBestSize();
			}

		public:
			Label(ILabelWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}
			~Label()
			{
				if (m_EvtHandler.GetClientData() == this)
				{
					PopEventHandler();
				}
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						FlagSet<LabelStyle> style = {}
			);

		public:
			// wxWindow+
			wxString GetLabel() const override
			{
				return m_Label;
			}
			void SetLabel(const wxString& label) override
			{
				DoSetLabel(RemoveMnemonics(label));
			}
			void SetLabelText(const wxString& label) override
			{
				DoSetLabel(label);
			}
			void SetLabelAlignment(FlagSet<Alignment> singleLine, FlagSet<Alignment> multiLine = {})
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

			// Label
			void Wrap(int width)
			{
				ScheduleRefresh();
				m_WrapLength = width;
			}
			void SetURI(URI uri) noexcept
			{
				m_URI = std::move(uri);
			}

			BitmapImage GetBitmap() const
			{
				return m_Icon;
			}
			void SetBitmap(const BitmapImage& image);

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
	};
}
