#pragma once
#include "Common.h"
#include "Event.h"
#include "kxf/Drawing/GDIRenderer/GDIImageList.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
struct _HD_ITEMW;
class wxHeaderCtrlEvent;

namespace kxf::UI::DataView
{
	class View;
	class MainWindow;
	class Column;
	class ItemEvent;
}

namespace kxf::UI::DataView
{
	class KXF_API HeaderCtrl: public WindowRefreshScheduler<wxControl>
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
			void* m_HeaderCtrlHandle = nullptr;
			int m_ScrollOffset = 0;

			std::unique_ptr<GDIImageList> m_ImageList;
			Column* m_DraggedColumn = nullptr;
			Column* m_ResizedColumn = nullptr;
			bool m_UpdateColumns = false;

		private:
			void* GetHeaderCtrlHandle() const;
			void DoSetSize(int x, int y, int width, int height, int sizeFlags) override;
			void ScrollWidget(int dx);

			void FinishEditing();
			EventResult SendCtrlEvent(ItemEvent& event, const EventID& type, Column* column = nullptr, std::optional<Rect> rect = {});
			EventResult SendCtrlEvent(const EventID& type, Column* column = nullptr, std::optional<Rect> rect = {})
			{
				ItemEvent event;
				return SendCtrlEvent(event, type, column, std::move(rect));
			}

			void OnCreate(wxWindowCreateEvent& event);
			void OnDestroy(wxWindowDestroyEvent& event);

			void OnClick(wxHeaderCtrlEvent& event);
			void OnRClick(wxHeaderCtrlEvent& event);
			void OnWindowClick(wxMouseEvent& event);
			void OnSeparatorDClick(wxHeaderCtrlEvent& event);

			void OnResize(wxHeaderCtrlEvent& event);
			void OnResizeEnd(wxHeaderCtrlEvent& event);
			void OnReorderEnd(wxHeaderCtrlEvent& event);

		protected:
			bool UpdateColumnWidthToFit(size_t index);
			void DoMakeItem(_HD_ITEMW& item, const Column& column);
			bool MSWOnNotify(int ctrlID, WXLPARAM lParam, WXLPARAM* result) override;
			void OnInternalIdle() override;

			wxBorder GetDefaultBorder() const override
			{
				return wxBORDER_NONE;
			}

		protected:
			void DoUpdate();
			void UpdateColumn(const Column& column);
			void UpdateColumnCount();
			bool CanUpdateSingleColumn() const
			{
				return false;
			}

			Rect GetDropdownRect(const Column& column) const;
			Rect GetDropdownRect(size_t index) const;

		public:
			HeaderCtrl(View* parent);
			~HeaderCtrl();

		public:
			View* GetView() const
			{
				return m_View;
			}
			MainWindow* GetMainWindow() const;

			size_t GetColumnCount() const;
			Column* GetColumnAt(size_t index) const;
			Column* GetColumnDisplayedAt(size_t index) const;

			void ToggleSortByColumn(size_t index);
			void ToggleSortByColumn(Column& column);

			bool SetBackgroundColour(const wxColour& color) override;

		public:
			wxDECLARE_NO_COPY_CLASS(HeaderCtrl);
	};
}
