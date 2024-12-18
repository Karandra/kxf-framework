#pragma once
#include "kxf-gui/Widgets/Common.h"
#include "kxf/Drawing/GDIRenderer/GDIFont.h"
#include "kxf/Drawing/GDIRenderer/GDIIcon.h"
#include "kxf/wxWidgets/ConstantsMapping.h"
#include "kxf/Utility/Common.h"
#include <wx/richtooltip.h>

namespace kxf::UI
{
	enum class RichToolTipKind
	{
		None = wxTipKind::wxTipKind_None,
		Auto = wxTipKind::wxTipKind_Auto,

		Top = wxTipKind::wxTipKind_Top,
		TopLeft = wxTipKind::wxTipKind_TopLeft,
		TopRight = wxTipKind::wxTipKind_TopRight,
		Bottom = wxTipKind::wxTipKind_Bottom,
		BottomLeft = wxTipKind::wxTipKind_BottomLeft,
		BottomRight = wxTipKind::wxTipKind_BottomRight
	};
}
namespace kxf::UI
{
	class KXF_API RichToolTip: public wxObject
	{
		public:
			static constexpr StdIcon DefaultIcon = StdIcon::Information;
			static constexpr RichToolTipKind DefaultKind = RichToolTipKind::Auto;
			static inline const TimeSpan DefaultDelay = TimeSpan::Milliseconds(0);

		private:
			wxRichToolTip m_ToolTip;

			String m_Title;
			String m_Message;
			GDIIcon m_Icon;
			GDIFont m_Font;
			StdIcon m_IconID = DefaultIcon;
			RichToolTipKind m_Kind = DefaultKind;
			Color m_Color1;
			Color m_Color2;
			TimeSpan m_Timeout = TimeSpan::Milliseconds(-1);
			TimeSpan m_Delay = TimeSpan::Milliseconds(-1);

		public:
			RichToolTip(String title = {}, String message = {})
				:m_ToolTip(title, message), m_Title(std::move(title)), m_Message(std::move(message))
			{
			}

		public:
			void Show(wxWindow* window, const Rect& rect = {})
			{
				if (rect.IsEmpty())
				{
					m_ToolTip.ShowFor(window, nullptr);
				}
				else
				{
					wxRect temp = rect;
					m_ToolTip.ShowFor(window, &temp);
				}
			}

			String GetTitle() const
			{
				return m_Title;
			}
			String GetMessage() const
			{
				return m_Message;
			}

			RichToolTipKind GetKind() const
			{
				return m_Kind;
			}
			void SetKind(RichToolTipKind kind)
			{
				m_Kind = kind;
				m_ToolTip.SetTipKind(static_cast<wxTipKind>(kind));
			}

			// Appearance
			GDIFont GetTitleFont() const
			{
				return m_Font;
			}
			void SetTitleFont(const GDIFont& font)
			{
				m_Font = font;
				m_ToolTip.SetTitleFont(font.ToWxFont());
			}
			void SetBackgroundColour(const Color& color1, const Color& color2 = wxNullColour)
			{
				m_Color1 = color1;
				m_Color2 = color2;
				m_ToolTip.SetBackgroundColour(color1, color2);
			}
			void GetBackgroundColour(Color* color1 = nullptr, Color* color2 = nullptr) const
			{
				Utility::SetIfNotNull(color1, m_Color1);
				Utility::SetIfNotNull(color2, m_Color2);
			}

			// Icon
			StdIcon GetIconID() const
			{
				return m_IconID;
			}
			GDIIcon GetIcon() const
			{
				return m_Icon;
			}
			void SetIcon(const GDIIcon& icon)
			{
				m_Icon = icon;
				m_IconID = StdIcon::None;
				m_ToolTip.SetIcon(icon.ToWxIcon());
			}
			void SetIcon(StdIcon iconID);

			// Animation
			TimeSpan GetTimeout() const
			{
				return m_Timeout;
			}
			void SetTimeout(TimeSpan timeout)
			{
				m_Timeout = timeout;
				m_ToolTip.SetTimeout(timeout.GetMilliseconds(), GetDelay().GetMilliseconds());
			}

			TimeSpan GetDelay() const
			{
				return m_Delay;
			}
			void SetDelay(TimeSpan delay)
			{
				m_Delay = delay;
				m_ToolTip.SetTimeout(GetTimeout().GetMilliseconds(), delay.GetMilliseconds());
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(RichToolTip);
	};
}
