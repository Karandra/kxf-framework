#include "kxf-pch.h"
#include "ToolTip.h"
#include "Column.h"
#include "../../Widgets/WXUI/DataView/View.h"
#include "../../IDataViewWidget.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Drawing/SizeRatio.h"
#include "kxf/System/SystemInformation.h"
#include <wx/textwrapper.h>

namespace kxf::DataView
{
	const DataView::Column& ToolTip::SelectAnchorColumn(const DataView::Column& currentColumn) const
	{
		if (m_AnchorColumn && m_AnchorColumn->IsVisible())
		{
			return *m_AnchorColumn;
		}
		return currentColumn;
	}
	const DataView::Column& ToolTip::SelectClipTestColumn(const DataView::Column& currentColumn) const
	{
		if (m_ClipTestColumn && m_ClipTestColumn->IsVisible())
		{
			return *m_ClipTestColumn;
		}
		return currentColumn;
	}

	Point ToolTip::GetPopupPosition(const DataView::Node& node, const DataView::Column& column) const
	{
		const Rect rect = node.GetCellClientRect(SelectAnchorColumn(column));
		return rect.GetPosition() + Point(0, rect.GetHeight() + 1);
	}
	Point ToolTip::AdjustPopupPosition(const DataView::Node& node, const DataView::Column& column, const Point& pos) const
	{
		auto& widget = column.GetOwningWdget();
		if (auto graphicsAware = widget.QueryInterface<IGraphicsRendererAwareWidget>())
		{
			if (auto context = graphicsAware->GetActiveGraphicsRenderer()->CreateMeasuringContext(&widget))
			{
				const Size smallIcon = Geometry::SizeRatio::FromSystemSmallIcon();
				Size textExtent = context->GetTextExtent(m_Message);
				Size offset;

				if (auto icon = GetIconBitmap())
				{
					textExtent += icon.GetSize() * 1.5;
					offset = smallIcon / 2;
				}
				else if (auto icon = GetIconID(); icon != StdIcon::None)
				{
					textExtent += smallIcon * 1.5;
					offset = smallIcon / 2;
				}
				else
				{
					offset = smallIcon;
				}

				const Size screenSize = System::GetMetric(SystemSizeMetric::Screen, widget.GetWxWindow());
				Point adjustedPos = widget.ClientToScreen(pos);
				if (int right = adjustedPos.GetX() + textExtent.GetWidth(); right > screenSize.GetWidth())
				{
					adjustedPos.X() -= (right - screenSize.GetWidth()) + offset.GetWidth();
				}
				if (int bottom = adjustedPos.GetY() + textExtent.GetHeight(); bottom > screenSize.GetHeight())
				{
					adjustedPos.Y() -= (bottom - screenSize.GetHeight()) + offset.GetHeight();
				}
				return widget.ScreenToClient(adjustedPos);
			}
		}
		return Point::UnspecifiedPosition();
	}
	String ToolTip::StripMarkupIfNeeded(const DataView::Node& node, const DataView::Column& column, const String& text) const
	{
		auto& widget = column.GetOwningWdget();
		if (!text.IsEmpty())
		{
			if (auto model = widget.GetDataModel())
			{
				auto renderer = node.GetCellRenderer(column);
				if (renderer && renderer.IsMarkupEnabled())
				{
					return renderer.StripMarkup(text);
				}
			}
		}
		return text;
	}

	bool ToolTip::IsNull() const noexcept
	{
		if (HasAnyIcon())
		{
			return !m_Caption.IsEmpty() && !m_Message.IsEmpty();
		}
		return !m_Message.IsEmpty();
	}

	bool ToolTip::Show(const DataView::Node& node, const DataView::Column& column)
	{
		auto& widget = column.GetOwningWdget();
		if (!m_Caption.IsEmpty())
		{
			auto& tooltip = column.m_View->m_ToolTip;
			tooltip.SetCaption(StripMarkupIfNeeded(node, column, m_Caption));
			tooltip.SetMessage(StripMarkupIfNeeded(node, column, m_Message));

			tooltip.RemoveOption(UI::ToolTipExStyle::LargeIcons);
			if (auto icon = GetIconBitmap())
			{
				tooltip.SetIcon(icon.ToGDIBitmap());
			}
			else
			{
				tooltip.SetIcon(GetIconID());
			}

			return tooltip.Popup(AdjustPopupPosition(node, column, GetPopupPosition(node, column)));
		}
		else if (auto window = widget.GetWxWindow())
		{
			window->SetToolTip(StripMarkupIfNeeded(node, column, m_Message));
			return window->GetToolTip() != nullptr;
		}
		return false;
	}
}
