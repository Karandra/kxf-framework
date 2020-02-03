#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Common.h"
#include "KxFramework/DataView2/Row.h"
#include "KxEvent.h"

namespace KxDataView2
{
	class KX_API Node;
	class KX_API View;
	class KX_API Column;
	class KX_API MainWindow;
	class KX_API Model;
}

namespace KxDataView2
{
	class KX_API Event: public wxNotifyEvent
	{
		private:
			Node* m_Node = nullptr;
			Column* m_Column = nullptr;
			std::optional<wxRect> m_Rect;
			std::pair<Row, Row> m_CacheHints;

		protected:
			View* GetView() const;
			MainWindow* GetMainWindow() const;

		public:
			Event(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE)
				:wxNotifyEvent(type, winid)
			{
			}
			wxEvent* Clone() const override
			{
				return new Event(*this);
			}

		public:
			Node* GetNode() const
			{
				return m_Node;
			}
			void SetNode(Node* item)
			{
				m_Node = item;
			}

			Column* GetColumn() const
			{
				return m_Column;
			}
			void SetColumn(Column* column)
			{
				m_Column = column;
			}

			wxRect GetRect() const
			{
				return m_Rect ? *m_Rect : wxRect();
			}
			void SetRect(const wxRect& rect)
			{
				m_Rect = rect;
			}
			void ResetRect()
			{
				m_Rect.reset();
			}

			wxPoint GetPosition() const 
			{
				return m_Rect ? m_Rect->GetPosition() : wxDefaultPosition;
			}
			void SetPosition(int x, int y)
			{
				if (!m_Rect)
				{
					m_Rect = {};
				}
				m_Rect->x = x;
				m_Rect->y = y;
			}
			void SetPosition(const wxPoint& pos)
			{
				SetPosition(pos.x, pos.y);
			}
			void ResetPosition()
			{
				m_Rect.reset();
			}

			wxSize GetSize() const
			{
				return m_Rect ? m_Rect->GetSize() : wxDefaultSize;
			}
			void SetSize(const wxSize& size)
			{
				m_Rect = size;
			}
			int GetWidth() const
			{
				return GetSize().x;
			}
			void SetWidth(int width)
			{
				SetSize({width, 0});
			}
			int GetHeight() const
			{
				return GetSize().y;
			}
			void SetHeight(int height)
			{
				SetSize({0, height});
			}

			// Return hints as physical rows
			std::pair<Row, Row> GetCacheHint() const
			{
				return m_CacheHints;
			}
			Row GetCacheHintFrom() const
			{
				return m_CacheHints.first;
			}
			Row GetCacheHintTo() const
			{
				return m_CacheHints.second;
			}
			
			// These are physical rows as user sees them, not logical
			void SetCacheHints(Row from, Row to)
			{
				m_CacheHints = {from, to};
			}
	};
}

namespace KxDataView2
{
	class KX_API EventEditor: public Event
	{
		private:
			wxAny m_Value;
			bool m_IsEditCancelled = false;

		public:
			EventEditor(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE)
				:Event(type, winid)
			{
			}

		public:
			wxEvent* Clone() const override
			{
				return new EventEditor(*this);
			}
			
			bool IsEditCancelled() const
			{
				return m_IsEditCancelled;
			}
			void SetEditCanceled(bool editCancelled = true)
			{
				m_IsEditCancelled = editCancelled;
			}

			const wxAny& GetValue() const
			{
				return m_Value;
			}
			void SetValue(wxAny&& value)
			{
				m_Value = std::move(value);
				value.MakeNull();
			}
			void SetValue(const wxAny& value)
			{
				m_Value = value;
			}
	};
}

namespace KxDataView2
{
	class KX_API EventDND: public Event
	{
		friend class KX_API MainWindow;

		private:
			wxDataObjectSimple* m_DataObject = nullptr;
			wxDragResult m_DropResult = wxDragNone;
			int m_DragFlags = wxDrag_CopyOnly;

		protected:
			wxDataObjectSimple* GetDataObject() const
			{
				return m_DataObject;
			}
			void SetDataObject(wxDataObjectSimple* object)
			{
				m_DataObject = object;
			}
			
			void SetDragFlags(int flags)
			{
				m_DragFlags = flags;
			}
			void SetDropEffect(wxDragResult effect)
			{
				m_DropResult = effect;
			}

		public:
			EventDND(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE)
				:Event(type, winid)
			{
			}

		public:
			wxEvent* Clone() const override
			{
				return new EventDND(*this);
			}
			
			// Drag
			wxDataObjectSimple* GetDragObject(const wxDataFormat& format) const;
			template<class T> T* GetDragObject(const wxDataFormat& format) const
			{
				static_assert(std::is_base_of_v<wxDataObjectSimple, T>);
				return dynamic_cast<T*>(GetDragObject(format));
			}

			int GetDragFlags() const
			{
				return m_DragFlags;
			}
			void DragDone(const wxDataObjectSimple& dataObject, int flags = wxDrag_CopyOnly);
			void DragCancel();

			// Drop
			wxDataObjectSimple* GetRecievedDataObject() const;
			template<class T> T* GetRecievedDataObject() const
			{
				static_assert(std::is_base_of_v<wxDataObjectSimple, T>);
				return dynamic_cast<T*>(GetRecievedDataObject());
			}
			
			wxDragResult GetDropResult() const
			{
				return m_DropResult;
			}
			void DropDone(wxDragResult result = wxDragNone);
			void DropCancel();
			void DropError();
	};
}

namespace KxDataView2
{
	KxEVENT_DECLARE_LOCAL(Event, ITEM_SELECTED);
	KxEVENT_DECLARE_LOCAL(Event, ITEM_HOVERED);
	KxEVENT_DECLARE_LOCAL(Event, ITEM_ACTIVATED);
	KxEVENT_DECLARE_LOCAL(Event, ITEM_COLLAPSED);
	KxEVENT_DECLARE_LOCAL(Event, ITEM_EXPANDED);
	KxEVENT_DECLARE_LOCAL(Event, ITEM_COLLAPSING);
	KxEVENT_DECLARE_LOCAL(Event, ITEM_EXPANDING);
	KxEVENT_DECLARE_LOCAL(Event, ITEM_CONTEXT_MENU);
	KxEVENT_DECLARE_LOCAL(Event, ITEM_VALUE_CHANGED);

	KxEVENT_DECLARE_LOCAL(EventEditor, ITEM_EDIT_STARTING);
	KxEVENT_DECLARE_LOCAL(EventEditor, ITEM_EDIT_STARTED);
	KxEVENT_DECLARE_LOCAL(EventEditor, ITEM_EDIT_DONE);

	KxEVENT_DECLARE_LOCAL(EventDND, ITEM_DRAG);
	KxEVENT_DECLARE_LOCAL(EventDND, ITEM_DROP);
	KxEVENT_DECLARE_LOCAL(EventDND, ITEM_DROP_POSSIBLE);

	KxEVENT_DECLARE_LOCAL(Event, COLUMN_HEADER_CLICK);
	KxEVENT_DECLARE_LOCAL(Event, COLUMN_HEADER_RCLICK);
	KxEVENT_DECLARE_LOCAL(Event, COLUMN_HEADER_MENU_ITEM);
	KxEVENT_DECLARE_LOCAL(Event, COLUMN_HEADER_SEPARATOR_DCLICK);
	KxEVENT_DECLARE_LOCAL(Event, COLUMN_HEADER_WIDTH_FIT);
	KxEVENT_DECLARE_LOCAL(Event, COLUMN_DROPDOWN);
	KxEVENT_DECLARE_LOCAL(Event, COLUMN_TOGGLE);
	KxEVENT_DECLARE_LOCAL(Event, COLUMN_SORTED);
	KxEVENT_DECLARE_LOCAL(Event, COLUMN_MOVED);

	KxEVENT_DECLARE_LOCAL(Event, COLUMN_BEGIN_RESIZE);
	KxEVENT_DECLARE_LOCAL(Event, COLUMN_RESIZE);
	KxEVENT_DECLARE_LOCAL(Event, COLUMN_END_RESIZE);

	KxEVENT_DECLARE_LOCAL(Event, VIEW_CACHE_HINT);
}
