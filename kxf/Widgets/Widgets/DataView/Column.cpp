#include "kxf-pch.h"
#include "Column.h"
#include "../../Widgets/WXUI/DataView/View.h"
#include "../../Widgets/WXUI/DataView/HeaderCtrl.h"
#include "../../Widgets/WXUI/DataView/MainWindow.h"
#include "../../Events/DataViewWidgetEvent.h"
#include "kxf/Utility/Common.h"

namespace kxf::DataView
{
	int Column::GetAbsMinColumnWidth() noexcept
	{
		return 0;
	}
	int Column::GetAbsMaxColumnWidth() noexcept
	{
		return wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	}

	void Column::MarkDirty(bool value)
	{
		m_IsDirty = value;
		if (m_View && value)
		{
			m_View->m_ColumnsDirty = true;
		}
	}
	void Column::UpdateDisplay()
	{
		if (m_View)
		{
			m_View->OnColumnChange(*this);
		}
	}
	
	WXUI::DataView::MainWindow* Column::GetMainWindow() const
	{
		return m_View ? m_View->GetMainWindow() : nullptr;
	}

	int Column::CalcTitleWidth() const
	{
		if (m_View)
		{
			wxWindow* window = m_View;
			if (auto header = m_View->GetHeaderCtrl())
			{
				window = header;
			}

			int width = window->GetTextExtent(m_Title).GetWidth();
			width += 2 * wxRendererNative::Get().GetHeaderButtonMargin(window);

			// If a bitmap is used, add space for it and 2px border
			if (m_Icon)
			{
				width += m_Icon.GetWidth() + window->FromDIP(wxSize(4, 0)).GetWidth();
			}
			return width;
		}
		return -1;
	}
	int Column::CalcBestWidth() const
	{
		if (auto mainWindow = GetMainWindow())
		{
			return mainWindow->CalcBestColumnWidth(*this);
		}
		return -1;
	}

	// IDataViewColumn
	void Column::OnColumnAttached(WXUI::DataView::View& view, size_t index, size_t displayIndex)
	{
		if (!m_View)
		{
			m_View = &view;
		}

		if (m_View)
		{
			m_Index = index;
			m_DisplayIndex = index < m_View->GetColumnCount() ? displayIndex : index;
		}
	}
	void Column::OnColumnDetached()
	{
		m_View = nullptr;
		m_Index = npos;
		m_DisplayIndex = npos;
	}

	IDataViewWidget& Column::GetOwningWdget() const
	{
		return m_View->m_Widget;
	}

	size_t Column::GetPhysicalDisplayIndex() const
	{
		if (IsVisible())
		{
			const size_t count = m_View->m_Columns.size();

			size_t index = 0;
			for (size_t i = 0; i < count; i++)
			{
				auto column = m_View->GetColumnDisplayedAt(i);
				if (column == this)
				{
					return index;
				}
				else if (column->IsVisible())
				{
					index++;
				}
			}
		}
		return npos;
	}
	void Column::SetDisplayIndex(size_t index)
	{
		if (m_View)
		{
			m_View->MoveColumn(*this, index);
			UpdateDisplay();
		}
		else
		{
			m_DisplayIndex = index;
		}
	}
	void Column::SetPhysicalDisplayIndex(size_t index)
	{
		if (IsVisible())
		{
			if (m_View)
			{
				m_View->MoveColumnToPhysicalIndex(*this, index);
				UpdateDisplay();
			}
			else
			{
				m_DisplayIndex = GetInvisibleColumnsBefore() + index;
			}
		}
	}
	size_t Column::GetInvisibleColumnsBefore() const
	{
		size_t count = 0;
		for (const auto& column: m_View->m_Columns)
		{
			if (column->GetIndex() < m_Index && !column->IsVisible())
			{
				count++;
			}
		}
		return count;
	}

	int Column::GetMinWidth() const
	{
		int width = m_MinWidth;
		if (m_View)
		{
			if (m_Style.Contains(ColumnStyle::Dropdown))
			{
				width += GetDropdownRect().GetWidth();
				width += m_View->FromDIP(wxSize(m_Style.Contains(ColumnStyle::CheckBox) ? 1 : 2, 0)).GetWidth();
			}
			if (m_Style.Contains(ColumnStyle::CheckBox))
			{
				width += IRendererNative::Get().GetCheckBoxSize(m_View).GetWidth();
				width += m_View->FromDIP(wxSize(8, 0)).GetWidth();
			}
		}
		return width;
	}
	bool Column::IsExposed(int& width) const
	{
		if (m_IsVisible)
		{
			width = GetWidth();
			if (width > 0)
			{
				return true;
			}
		}

		width = 0;
		return false;
	}

	Rect Column::GetRect() const
	{
		auto& widget = GetOwningWdget();

		int x = 0;
		for (size_t i = 0; i < widget.GetColumnCount(); i++)
		{
			if (auto column = widget.GetColumnDisplayedAt(i))
			{
				if (column == this)
				{
					break;
				}

				int width = 0;
				if (column->IsExposed(width))
				{
					x += width;
				}
			}
		}

		return {x, 0, GetWidth(), widget.GetSize(WidgetSizeFlag::Client).GetHeight()};
	}
	Rect Column::GetDropdownRect() const
	{
		auto& widget = GetOwningWdget();
		if (m_IsVisible && m_View && m_View->m_HeaderArea)
		{
			return m_View->m_HeaderArea->GetDropdownRect(*this);
		}
		return {};
	}

	void Column::Refresh() const
	{
		if (auto mainWindow = GetMainWindow())
		{
			mainWindow->RefreshColumn(*this);
		}
	}
	void Column::FitInside()
	{
		if (auto mainWindow = GetMainWindow())
		{
			int width = m_Width;

			mainWindow->RecalculateDisplay();
			for (auto& column: m_View->m_Columns)
			{
				if (column.get() == this)
				{
					width = CalcBestWidth();
				}
			}

			const int clientWidth = mainWindow->m_View->GetClientSize().GetWidth();
			const int virtualWidth = mainWindow->GetRowWidth();
			if (virtualWidth > clientWidth)
			{
				width -= virtualWidth - clientWidth;
			}
			else
			{
				width += clientWidth - virtualWidth;
			}

			SetWidth(width);
		}
	}
	bool Column::FitContent()
	{
		if (auto mainWindow = GetMainWindow())
		{
			auto width = std::max({CalcTitleWidth(), GetBestWidth(), GetMinWidth()});

			DataViewWidgetEvent event(m_View->m_Widget);
			event.SetColumn(this);
			event.SetSize({width, Geometry::DefaultCoord});

			m_View->m_Widget.ProcessEvent(event, DataViewWidgetEvent::EvtColumnHeaderWidthFit);
			if (event.IsAllowed())
			{
				SetWidth(event.GetSize().GetWidth());
				return true;
			}
		}
		return false;
	}

	bool Column::IsCurrent() const
	{
		return this == GetOwningWdget().GetCurrentColumn();
	}
	bool Column::IsExpander() const
	{
		return this == GetOwningWdget().GetExpanderColumn();
	}
	bool Column::IsHotTracked() const
	{
		return this == GetOwningWdget().GetHotTrackColumn();
	}

	bool Column::IsFirst() const
	{
		return GetIndex() == 0;
	}
	bool Column::IsDisplayedFirst() const
	{
		return GetPhysicalDisplayIndex() == 0;
	}

	bool Column::IsLast() const
	{
		return GetIndex() + 1 == GetOwningWdget().GetColumnCount();
	}
	bool Column::IsDisplayedLast() const
	{
		return GetPhysicalDisplayIndex() + 1 == GetOwningWdget().GetVisibleColumnCount();
	}
}
