#include "kxf-pch.h"
#include "ImageGallery.h"
#include "../../Events/WidgetItemEvent.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Drawing/IRendererNative.h"
#include "kxf/Drawing/SizeRatio.h"

namespace kxf::WXUI
{
	// ImageGallery
	void ImageGallery::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		DoPrepareDC(dc);

		auto renderer = m_RendererAware->GetActiveGraphicsRenderer();
		auto gc = renderer->CreateLegacyContext(dc, SizeF(GetVirtualSize()));
		gc->Clear(renderer->GetTransparentBrush());

		IRendererNative& nativeRenderer = IRendererNative::Get();
		const size_t itemsInRow = CalcItemsPerRow();
		const size_t rowBegin = GetVisibleRowsBegin();
		const size_t rowEnd = GetVisibleRowsEnd();
		for (size_t row = rowBegin; row < rowEnd; row++)
		{
			for (size_t column = 0; column < itemsInRow; column++)
			{
				const size_t itemIndex = GetIndexByRowColumn(row, column, itemsInRow);
				if (itemIndex < m_Items.size())
				{
					Rect itemRect = GetItemFullRect(row, column, rowBegin);
					if (itemIndex == m_Focused)
					{
						nativeRenderer.DrawItemSelectionRect(this, *gc, itemRect, NativeWidgetFlag::Current|NativeWidgetFlag::CellItem);
					}
					if (itemIndex == m_Selected)
					{
						nativeRenderer.DrawItemSelectionRect(this, *gc, itemRect, NativeWidgetFlag::Current|NativeWidgetFlag::Selected|NativeWidgetFlag::Focused);
					}

					const auto& image = *m_Items[itemIndex];
					const Point newPos = itemRect.GetTopLeft() + Point::FromSize(m_ItemSize - image.GetSize()) / 2;
					gc->DrawTexture(image, RectF(newPos, image.GetSize()));
				}
				else
				{
					return;
				}
			}
		}
	}
	void ImageGallery::OnSize(wxSizeEvent& event)
	{
		event.Skip();

		RecalculateDisplay();
		Refresh();
	}
	void ImageGallery::OnMouse(wxMouseEvent& event)
	{
		event.Skip();

		auto TranslateEventType = [](wxMouseEvent& event) -> EventID
		{
			if (event.GetEventType() == wxEVT_LEFT_DOWN)
			{
				return WidgetItemEvent::EvtSelected;
			}
			if (event.GetEventType() == wxEVT_LEFT_DCLICK)
			{
				return WidgetItemEvent::EvtActivated;
			}
			if (event.GetEventType() == wxEVT_RIGHT_UP)
			{
				return WidgetItemEvent::EvtContextMenu;
			}
			return {};
		};
		auto SendEvent = [&](const EventID& eventType, wxMouseEvent& mouseEvent, size_t selection)
		{
			WidgetItemEvent event(m_Widget, selection, WidgetMouseEvent(m_Widget, mouseEvent));
			return m_Widget.ProcessEvent(event, eventType);
		};

		const size_t itemsInRow = CalcItemsPerRow();
		const Point pos = Point(event.GetPosition());
		const int row = VirtualHitTest(pos.GetY());

		const EventID eventType = TranslateEventType(event);
		const size_t oldSelection = m_Selected;
		const size_t oldFocus = m_Focused;
		if (row != IImageGalleryWidget::npos && pos.GetX() != 0)
		{
			const size_t rowBegin = GetVisibleRowsBegin();
			for (size_t column = 0; column < itemsInRow; column++)
			{
				const Rect thumbRect = GetItemFullRect(row, column, rowBegin);
				if (pos.GetX() >= thumbRect.GetLeft() && pos.GetX() <= thumbRect.GetRight())
				{
					const size_t index = GetIndexByRowColumn(row, column, itemsInRow);
					const size_t focus = index < GetItemCount() ? index : IImageGalleryWidget::npos;
					const size_t selection = focus;

					if (focus != oldFocus)
					{
						ScheduleRefresh();
						m_Focused = focus;
					}
					if (eventType && selection != oldSelection)
					{
						ScheduleRefresh();
						m_Selected = selection;

						SendEvent(eventType, event, selection);
					}
					return;
				}
			}

			m_Focused = IImageGalleryWidget::npos;
			if (eventType)
			{
				m_Selected = IImageGalleryWidget::npos;
			}
			if (eventType == WidgetItemEvent::EvtContextMenu)
			{
				SendEvent(eventType, event, IImageGalleryWidget::npos);
			}
			ScheduleRefresh();
		}
	}
	void ImageGallery::OnKillFocus(wxFocusEvent& event)
	{
		m_Focused = IImageGalleryWidget::npos;
		ScheduleRefresh();

		event.Skip();
	}

	Rect ImageGallery::GetItemRect(size_t row, size_t columnIndex, size_t beginRow)
	{
		return {Point(columnIndex * m_ItemSize.GetWidth(), row * m_ItemSize.GetHeight()), m_ItemSize};
	}
	Rect ImageGallery::GetItemFullRect(size_t row, size_t columnIndex, size_t beginRow)
	{
		auto pos = Point(columnIndex * m_ItemSize.GetWidth(), row * m_ItemSize.GetHeight());
		if (columnIndex != 0)
		{
			pos.X() += m_Spacing.GetWidth() * columnIndex;
		}
		if (row != 0 && row != beginRow)
		{
			pos.Y() += m_Spacing.GetHeight() * (row - beginRow);
		}
		return {pos, m_ItemSize};
	}
	Size ImageGallery::GetFinalItemSize() const
	{
		return m_ItemSize + m_Spacing;
	}
	size_t ImageGallery::GetIndexByRowColumn(size_t row, size_t columnIndex, size_t itemsInRow) const
	{
		return row * itemsInRow + columnIndex;
	}

	size_t ImageGallery::CalcItemsPerRow() const
	{
		int count = GetClientSize().GetWidth() / GetFinalItemSize().GetWidth();
		return count == 0 ? 1 : count;
	}
	size_t ImageGallery::CalcRowCount() const
	{
		return static_cast<size_t>(std::ceil(m_Items.size() / static_cast<double>(CalcItemsPerRow())));
	}
	void ImageGallery::RecalculateDisplay()
	{
		const size_t count = CalcRowCount();
		const size_t oldCount = GetRowCount();
		if (oldCount != count)
		{
			wxVScrolledWindow::SetRowCount(count);
		}
	}

	// wxWindow
	void ImageGallery::OnInternalIdle()
	{
		if (m_Focused != IImageGalleryWidget::npos && !IsMouseInWindow())
		{
			m_Focused = IImageGalleryWidget::npos;
			ScheduleRefresh();
		}

		wxVScrolledWindow::OnInternalIdle();
		WindowRefreshScheduler::OnInternalIdle();
	}

	// ImageGallery
	bool ImageGallery::Create(wxWindow* parent,
							  const String& label,
							  const Point& pos,
							  const Size& size
	)
	{
		if (wxVScrolledWindow::Create(parent, wxID_NONE, pos, size, wxBORDER_THEME, wxS("ImageGallery")))
		{
			SetLabel(label);
			EnableSystemTheme();
			SetDoubleBuffered(true);

			using namespace Geometry;
			m_Spacing = m_Widget.FromDIP<Size>(2, 2);
			m_ItemSize = m_Widget.FromDIP(SizeRatio::FromWidth(SizeRatio::FromSystemIcon().GetWidth() * 4, SizeRatio::r16_9));

			Bind(wxEVT_SIZE, &ImageGallery::OnSize, this);
			Bind(wxEVT_PAINT, &ImageGallery::OnPaint, this);
			Bind(wxEVT_MOTION, &ImageGallery::OnMouse, this);
			Bind(wxEVT_LEFT_DOWN, &ImageGallery::OnMouse, this);
			Bind(wxEVT_LEFT_DCLICK, &ImageGallery::OnMouse, this);
			Bind(wxEVT_RIGHT_UP, &ImageGallery::OnMouse, this);
			Bind(wxEVT_KILL_FOCUS, &ImageGallery::OnKillFocus, this);

			return m_Widget.QueryInterface(m_RendererAware);
		}
		return false;
	}
}
