#pragma once
#include "Common.h"
#include "kxf/UI/StdIcon.h"

namespace kxf
{
	class IDataViewWidget;
}
namespace kxf::WXUI::DataView
{
	class MainWindow;
}

namespace kxf::DataView
{
	class KXF_API ToolTip final
	{
		friend class WXUI::DataView::MainWindow;

		private:
			String m_Caption;
			String m_Message;
			std::variant<StdIcon, BitmapImage> m_Icon = StdIcon::None;
			DataView::Column* m_AnchorColumn = nullptr;

			DataView::Column* m_ClipTestColumn = nullptr;
			bool m_DisplayOnlyIfClipped = false;
			bool m_AutoHide = true;

		private:
			const DataView::Column& SelectAnchorColumn(const DataView::Column& currentColumn) const;
			const DataView::Column& SelectClipTestColumn(const DataView::Column& currentColumn) const;

			Point GetPopupPosition(const DataView::Node& node, const DataView::Column& column) const;
			Point AdjustPopupPosition(const DataView::Node& node, const DataView::Column& column, const Point& pos) const;
			String StripMarkupIfNeeded(const DataView::Node& node, const DataView::Column& column, const String& text) const;

			bool IsNull() const noexcept;

		public:
			ToolTip() noexcept = default;
			ToolTip(String message, StdIcon icon = StdIcon::None)
				:m_Message(std::move(message)), m_Icon(icon)
			{
			}
			ToolTip(String caption, String message, StdIcon icon = StdIcon::None)
				:m_Caption(std::move(caption)), m_Message(std::move(message)), m_Icon(icon)
			{
			}
			ToolTip(String caption, String message, BitmapImage icon)
				:m_Caption(std::move(caption)), m_Message(std::move(message)), m_Icon(std::move(icon))
			{
			}

		public:
			bool Show(const DataView::Node& node, const DataView::Column& column);

			String GetCaption() const
			{
				return m_Caption;
			}
			void SetCaption(String caption) noexcept
			{
				m_Caption = std::move(caption);
			}

			String GetMessage() const
			{
				return m_Message;
			}
			void SetMessage(String message) noexcept
			{
				m_Message = std::move(message);
			}

			bool HasAnyIcon() const
			{
				return !m_Icon.valueless_by_exception() && (GetIconID() != StdIcon::None || !GetIconBitmap().IsNull());
			}
			StdIcon GetIconID() const noexcept
			{
				if (const auto& value = std::get_if<StdIcon>(&m_Icon))
				{
					return *value;
				}
				return StdIcon::None;
			}
			BitmapImage GetIconBitmap() const
			{
				if (const auto& value = std::get_if<BitmapImage>(&m_Icon))
				{
					return *value;
				}
				return {};
			}
			void SetIcon(StdIcon icon) noexcept
			{
				m_Icon = icon;
			}
			void SetIcon(BitmapImage icon) noexcept
			{
				m_Icon = std::move(icon);
			}

			void SetAnchorColumn(DataView::Column* column) noexcept
			{
				m_AnchorColumn = column;
			}
			DataView::Column* GetAnchorColumn() const noexcept
			{
				return m_AnchorColumn;
			}

			bool ShouldAutoHide() const noexcept
			{
				return m_AutoHide;
			}
			void SetAutoHide(bool value = true) noexcept
			{
				m_AutoHide = value;
			}

			DataView::Column* GetClipTestColumn() const noexcept
			{
				return m_ClipTestColumn;
			}
			bool ShouldDisplayOnlyIfClipped() const noexcept
			{
				return m_DisplayOnlyIfClipped;
			}
			void DisplayOnlyIfClipped(bool value = true) noexcept
			{
				m_DisplayOnlyIfClipped = value;
				if (!value)
				{
					m_ClipTestColumn = nullptr;
				}
			}
			void DisplayOnlyIfClipped(DataView::Column* column) noexcept
			{
				m_DisplayOnlyIfClipped = true;
				m_ClipTestColumn = column;
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}
