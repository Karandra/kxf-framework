#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"

namespace KxDataView2
{
	class KX_API View;
	class KX_API MainWindow;
	class KX_API Node;
	class KX_API Column;
}

namespace KxDataView2
{
	class KX_API ToolTip final
	{
		friend class MainWindow;

		public:
			template<class... Args> static ToolTip CreateDefaultForRenderer(Args&&... arg)
			{
				ToolTip tooltip(std::forward<Args>(arg)...);
				tooltip.DisplayOnlyIfClipped();
				return tooltip;
			}

		private:
			wxString m_Caption;
			wxString m_Message;
			std::variant<wxBitmap, KxIconType> m_Icon;
			const Column* m_AnchorColumn = nullptr;
			bool m_AutoHide = true;
			bool m_DisplayOnlyIfClipped = false;

		private:
			const Column& SelectAnchorColumn(const Column& currentColumn) const;
			wxPoint GetPopupPosition(const Node& node, const Column& column) const;
			wxPoint AdjustPopupPosition(const Node& node, const wxPoint& pos) const;
			wxString StripMarkupIfNeeded(const Node& node, const Column& column, const wxString& text) const;

			bool Show(const Node& node, const Column& column);

		public:
			ToolTip() = default;
			ToolTip(const wxString& message, KxIconType icon = KxICON_NONE)
				:m_Message(message), m_Icon(icon)
			{
			}
			ToolTip(const wxString& caption, const wxString& message, KxIconType icon = KxICON_NONE)
				:m_Caption(caption), m_Message(message), m_Icon(icon)
			{
			}
			ToolTip(const wxString& caption, const wxString& message, const wxBitmap& bitmap)
				:m_Caption(caption), m_Message(message), m_Icon(bitmap)
			{
			}

		public:
			bool IsOK() const;
			
			wxString GetCaption() const
			{
				return m_Caption;
			}
			void SetCaption(const wxString& value)
			{
				m_Caption = value;
			}

			wxString GetMessage() const
			{
				return m_Message;
			}
			void SetMessage(const wxString& value)
			{
				m_Message = value;
			}

			KxIconType GetIconID() const
			{
				if (const auto& value = std::get_if<KxIconType>(&m_Icon))
				{
					return *value;
				}
				return KxICON_NONE;
			}
			wxBitmap GetIconBitmap() const
			{
				if (const auto& value = std::get_if<wxBitmap>(&m_Icon))
				{
					return *value;
				}
				return wxNullBitmap;
			}
			void SetIcon(KxIconType icon)
			{
				m_Icon = icon;
			}
			void SetIcon(const wxBitmap& icon)
			{
				m_Icon = icon;
			}
			void SetIcon(const wxIcon& icon)
			{
				m_Icon = icon;
			}

			void SetAnchorColumn(const Column* column = nullptr)
			{
				m_AnchorColumn = column;
			}
			const Column* GetAnchorColumn() const
			{
				return m_AnchorColumn;
			}

			bool ShouldAutoHide() const
			{
				return m_AutoHide;
			}
			void AutoHide(bool value = true)
			{
				m_AutoHide = value;
			}

			bool ShouldDisplayOnlyIfClipped() const
			{
				return m_DisplayOnlyIfClipped;
			}
			void DisplayOnlyIfClipped(bool value = true)
			{
				m_DisplayOnlyIfClipped = value;
			}
	};
}
