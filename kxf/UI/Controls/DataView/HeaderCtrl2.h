#pragma once
#include "Common.h"
#include "Event.h"
#include "kxf/Drawing/GDIRenderer/GDIImageList.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/systhemectrl.h>
#include <wx/headerctrl.h>
#include <wx/overlay.h>

namespace kxf::UI::DataView
{
	class View;
	class MainWindow;
	class Column;
	class ItemEvent;
}

namespace kxf::UI::DataView
{
	class KXF_API HeaderCtrl2: public WindowRefreshScheduler<wxSystemThemedControl<wxControl>>
	{
		friend class View;
		friend class MainWindow;
		friend class Column;

		public:
			struct EventResult
			{
				bool Processed = false;
				bool Allowed = false;
			};

		private:
			View* m_View = nullptr;

			Column* m_DraggedColumn = nullptr;
			Column* m_ResizedColumn = nullptr;
			Column* m_HoverColumn = nullptr;

			wxOverlay m_DragOverlay;
			int m_ScrollOffset = 0;
			int m_DragOffset = 0;
			bool m_WasSeparatorDClick = false;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnMouse(wxMouseEvent& event);
			void OnKeyDown(wxKeyEvent& event);
			void OnCaptureLost(wxMouseCaptureLostEvent& event);

			void UpdateReorderingMarker(int xPhysical);
			void ClearMarkers();

			void EndDragging();
			void CancelDragging();

			void StartReordering(Column& col, int xPhysical);
			bool EndReordering(int xPhysical);

			int ConstrainByMinWidth(Column& col, int& xPhysical);
			void StartOrContinueResizing(Column& col, int xPhysical);
			void EndResizing(int xPhysical);

			void FinishEditing();
			EventResult SendCtrlEvent(ItemEvent& event, const EventID& type, Column* column = nullptr, std::optional<Rect> rect = {});
			EventResult SendCtrlEvent(const EventID& type, Column* column = nullptr, std::optional<Rect> rect = {})
			{
				ItemEvent event;
				return SendCtrlEvent(event, type, column, std::move(rect));
			}

			void OnClick(wxHeaderCtrlEvent& event);
			void OnRClick(wxHeaderCtrlEvent& event);
			void OnWindowClick(wxMouseEvent& event);
			void OnSeparatorDClick(wxHeaderCtrlEvent& event);

			void OnResize(wxHeaderCtrlEvent& event);
			void OnResizeEnd(wxHeaderCtrlEvent& event);
			void OnReorderEnd(wxHeaderCtrlEvent& event);

		protected:
			wxSize DoGetBestSize() const override;
			void OnInternalIdle() override;

			Rect GetDropdownRect(const Column& column) const;

		public:
			HeaderCtrl2(View* parent, long style = wxHD_DEFAULT_STYLE)
			{
				Create(parent, style);
			}
			bool Create(View* parent, long style = wxHD_DEFAULT_STYLE);

		public:
			View* GetView() const
			{
				return m_View;
			}
			MainWindow* GetMainWindow() const;

			bool IsResizing() const
			{
				return m_ResizedColumn != nullptr;
			}
			bool IsReordering() const
			{
				return m_DraggedColumn != nullptr;
			}
			bool IsDragging() const
			{
				return IsResizing() || IsReordering();
			}

			bool HasColumns() const
			{
				return GetColumnCount() == 0;
			}
			size_t GetColumnCount() const;
			Column* GetColumnAt(size_t index) const;
			Column* GetColumnDisplayedAt(size_t index) const;

			void ToggleSortByColumn(size_t index);
			void ToggleSortByColumn(Column& column);

			void RefreshColumn(Column& column);
			void RefreshColumnsAfter(Column& column);

			Column* GetColumnAtPoint(int xPhysical, bool* separator = nullptr) const;
			Column* GetColumnClosestToPoint(int xPhysical) const;

			void ScrollWindow(int dx, int dy, const wxRect* rect = nullptr) override;

		public:
			wxDECLARE_NO_COPY_CLASS(HeaderCtrl2);
	};
}
