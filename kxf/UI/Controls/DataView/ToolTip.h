#pragma once
#include "Common.h"
#include "kxf/UI/StdIcon.h"

namespace kxf::UI::DataView
{
	class View;
	class MainWindow;
	class Node;
	class Column;
}

namespace kxf::UI::DataView
{
	class KXF_API ToolTip final
	{
		friend class MainWindow;

		public:
			using StdIcon = kxf::StdIcon;

		public:
			template<class... Args>
			static ToolTip CreateDefaultForRenderer(Args&&... arg)
			{
				ToolTip tooltip(std::forward<Args>(arg)...);
				tooltip.DisplayOnlyIfClipped();
				return tooltip;
			}

		private:
			String m_Caption;
			String m_Message;
			std::variant<GDIBitmap, StdIcon> m_Icon;
			const Column* m_AnchorColumn = nullptr;

			const Column* m_ClipTestColumn = nullptr;
			bool m_DisplayOnlyIfClipped = false;
			bool m_AutoHide = true;

		private:
			const Column& SelectAnchorColumn(const Column& currentColumn) const;
			const Column& SelectClipTestColumn(const Column& currentColumn) const;

			Point GetPopupPosition(const Node& node, const Column& column) const;
			Point AdjustPopupPosition(const Node& node, const Point& pos) const;
			String StripMarkupIfNeeded(const Node& node, const Column& column, const String& text) const;

			bool Show(const Node& node, const Column& column);

		public:
			ToolTip(const String& message = {}, StdIcon icon = StdIcon::None)
				:m_Message(message), m_Icon(icon)
			{
			}
			ToolTip(const String& caption, const String& message, StdIcon icon = StdIcon::None)
				:m_Caption(caption), m_Message(message), m_Icon(icon)
			{
			}
			ToolTip(const String& caption, const String& message, const GDIBitmap& bitmap)
				:m_Caption(caption), m_Message(message), m_Icon(bitmap)
			{
			}

		public:
			bool IsOK() const;

			String GetCaption() const
			{
				return m_Caption;
			}
			void SetCaption(const String& value)
			{
				m_Caption = value;
			}

			String GetMessage() const
			{
				return m_Message;
			}
			void SetMessage(const String& value)
			{
				m_Message = value;
			}

			bool HasAnyIcon() const
			{
				return !m_Icon.valueless_by_exception() && (GetIconID() != StdIcon::None || !GetIconBitmap().IsNull());
			}
			StdIcon GetIconID() const
			{
				if (const auto& value = std::get_if<StdIcon>(&m_Icon))
				{
					return *value;
				}
				return StdIcon::None;
			}
			GDIBitmap GetIconBitmap() const
			{
				if (const auto& value = std::get_if<GDIBitmap>(&m_Icon))
				{
					return *value;
				}
				return {};
			}
			void SetIcon(StdIcon icon)
			{
				m_Icon = icon;
			}
			void SetIcon(const GDIBitmap& icon)
			{
				m_Icon = icon;
			}
			void SetIcon(const GDIIcon& icon)
			{
				m_Icon = icon;
			}

			void SetAnchorColumn(const Column& column)
			{
				m_AnchorColumn = &column;
			}
			void ResetAnchorColumn()
			{
				m_AnchorColumn = nullptr;
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

			const Column* GetClipTestColumn() const
			{
				return m_ClipTestColumn;
			}
			bool ShouldDisplayOnlyIfClipped() const
			{
				return m_DisplayOnlyIfClipped;
			}
			void DisplayOnlyIfClipped(bool value = true)
			{
				m_DisplayOnlyIfClipped = value;
				if (!value)
				{
					m_ClipTestColumn = nullptr;
				}
			}
			void DisplayOnlyIfClipped(const Column& column)
			{
				m_DisplayOnlyIfClipped = true;
				m_ClipTestColumn = &column;
			}
	};
}
