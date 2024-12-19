#pragma once
#include "Common.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIImageList.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
#include "../../../Events/DataViewWidgetEvent.h"
struct _HD_ITEMW;
class wxHeaderCtrlEvent;

namespace kxf::DataView
{
	class Column;
}
namespace kxf::WXUI::DataView
{
	class View;
	class MainWindow;

	namespace DV = kxf::DataView;
}

namespace kxf::WXUI::DataView
{
	class KXF_API HeaderCtrl: public UI::WindowRefreshScheduler<wxControl>
	{
		friend class View;
		friend class MainWindow;
		friend class DV::Column;

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
			DV::Column* m_DraggedColumn = nullptr;
			DV::Column* m_ResizedColumn = nullptr;
			bool m_UpdateColumns = false;

		private:
			void* GetHeaderCtrlHandle() const;
			void DoSetSize(int x, int y, int width, int height, int sizeFlags) override;
			void ScrollWidget(int dx, int dy, const Rect& rect);

			void FinishEditing();
			EventResult SendWidgetEvent(DataViewWidgetEvent& event, const EventID& eventID, DV::Column* column = nullptr, std::optional<Rect> rect = {});
			EventResult SendWidgetEvent(const EventID& eventID, DV::Column* column = nullptr, std::optional<Rect> rect = {});

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
			void DoMakeItem(_HD_ITEMW& item, const DV::Column& column);
			bool MSWOnNotify(int ctrlID, WXLPARAM lParam, WXLPARAM* result) override;
			void OnInternalIdle() override;

			wxBorder GetDefaultBorder() const override
			{
				return wxBORDER_NONE;
			}

		protected:
			void DoUpdate();
			void UpdateColumn(const DV::Column& column);
			void UpdateColumnCount();
			bool CanUpdateSingleColumn() const
			{
				return false;
			}

			Rect GetDropdownRect(const DV::Column& column) const;
			Rect GetDropdownRect(size_t index) const;

		public:
			HeaderCtrl(View& parent);
			~HeaderCtrl();

		public:
			View* GetView() const
			{
				return m_View;
			}
			MainWindow* GetMainWindow() const;


			void ToggleSortByColumn(size_t index);
			void ToggleSortByColumn(DV::Column& column);

			bool SetBackgroundColour(const wxColour& color) override;

		public:
			wxDECLARE_NO_COPY_CLASS(HeaderCtrl);
	};
}
