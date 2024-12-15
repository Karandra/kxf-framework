#pragma once
#include "kxf/EventSystem/Event.h"
#include "Common.h"
#include "Row.h"
#include "kxf/Core/Any.h"
#include <wx/dnd.h>
#include <wx/dataobj.h>

namespace kxf::UI::DataView
{
	class Node;
	class View;
	class Column;
	class MainWindow;
	class Model;
}

namespace kxf::UI::DataView
{
	class KX_API ItemEvent: public wxNotifyEvent
	{
		public:
			KxEVENT_MEMBER(ItemEvent, ItemSelected);
			KxEVENT_MEMBER(ItemEvent, ItemHovered);
			KxEVENT_MEMBER(ItemEvent, ItemActivated);
			KxEVENT_MEMBER(ItemEvent, ItemCollapsed);
			KxEVENT_MEMBER(ItemEvent, ItemExpanded);
			KxEVENT_MEMBER(ItemEvent, ItemCollapsing);
			KxEVENT_MEMBER(ItemEvent, ItemExpanding);
			KxEVENT_MEMBER(ItemEvent, ItemContextMenu);
			KxEVENT_MEMBER(ItemEvent, ItemValueChanged);

			KxEVENT_MEMBER(ItemEvent, ColumnHeaderClick);
			KxEVENT_MEMBER(ItemEvent, ColumnHeaderRClick);
			KxEVENT_MEMBER(ItemEvent, ColumnHeaderMenuItem);
			KxEVENT_MEMBER(ItemEvent, ColumnHeaderSeparatorDClick);
			KxEVENT_MEMBER(ItemEvent, ColumnHeaderWidthFit);
			KxEVENT_MEMBER(ItemEvent, ColumnDropdown);
			KxEVENT_MEMBER(ItemEvent, ColumnToggle);
			KxEVENT_MEMBER(ItemEvent, ColumnSorted);
			KxEVENT_MEMBER(ItemEvent, ColumnMoved);

			KxEVENT_MEMBER(ItemEvent, ColumnBeginResize);
			KxEVENT_MEMBER(ItemEvent, ColumnResize);
			KxEVENT_MEMBER(ItemEvent, ColumnEndResize);

			KxEVENT_MEMBER(ItemEvent, ViewCacheHint);

		private:
			Node* m_Node = nullptr;
			Column* m_Column = nullptr;
			std::optional<Rect> m_Rect;
			std::pair<Row, Row> m_CacheHints;

		public:
			ItemEvent(const EventID& eventID = {}, WidgetID id = wxID_NONE)
				:wxNotifyEvent(eventID.AsInt(), id.ToInt())
			{
			}

		public:
			ItemEvent* Clone() const override
			{
				return new ItemEvent(*this);
			}

			View* GetView() const;
			MainWindow* GetMainWindow() const;

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

			Rect GetRect() const
			{
				return m_Rect ? *m_Rect : Rect();
			}
			void SetRect(const Rect& rect)
			{
				m_Rect = rect;
			}
			void ResetRect()
			{
				m_Rect.reset();
			}

			Point GetPosition() const
			{
				return m_Rect ? m_Rect->GetPosition() : Point::UnspecifiedPosition();
			}
			void SetPosition(int x, int y)
			{
				if (!m_Rect)
				{
					m_Rect = Rect();
				}
				m_Rect->X() = x;
				m_Rect->Y() = y;
			}
			void SetPosition(const Point& pos)
			{
				SetPosition(pos.GetX(), pos.GetY());
			}
			void ResetPosition()
			{
				SetPosition(Point::UnspecifiedPosition());
			}

			Size GetSize() const
			{
				return m_Rect ? m_Rect->GetSize() : Size::UnspecifiedSize();
			}
			void SetSize(const Size& size)
			{
				if (!m_Rect)
				{
					m_Rect = Rect();
				}
				m_Rect->SetSize(size);
			}
			void ResetSize(const Size& size)
			{
				SetSize(Size::UnspecifiedSize());
			}

			int GetWidth() const
			{
				return GetSize().GetWidth();
			}
			void SetWidth(int width)
			{
				SetSize({width, 0});
			}
			int GetHeight() const
			{
				return GetSize().GetHeight();
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

namespace kxf::UI::DataView
{
	class KX_API EditorEvent: public ItemEvent
	{
		public:
			KxEVENT_MEMBER(EditorEvent, ItemEditStarting);
			KxEVENT_MEMBER(EditorEvent, ItemEditStarted);
			KxEVENT_MEMBER(EditorEvent, ItemEditDone);

		private:
			std::variant<Any, const Any*> m_Value;
			bool m_IsEditCancelled = false;

		public:
			EditorEvent(const EventID& eventID = {}, WidgetID id = wxID_NONE)
				:ItemEvent(eventID, id)
			{
			}

		public:
			EditorEvent* Clone() const override
			{
				return new EditorEvent(*this);
			}

			bool IsEditCancelled() const
			{
				return m_IsEditCancelled;
			}
			void SetEditCanceled(bool editCancelled = true)
			{
				m_IsEditCancelled = editCancelled;
			}

			const Any& GetValue() const&
			{
				if (decltype(auto) value = std::get_if<const Any*>(&m_Value))
				{
					return **value;
				}
				else
				{
					return std::get<Any>(m_Value);
				}
			}
			Any GetValue() &&
			{
				if (decltype(auto) value = std::get_if<const Any*>(&m_Value))
				{
					return **value;
				}
				else
				{
					return std::get<Any>(std::move(m_Value));
				}
			}

			void SetValue(Any&& value)
			{
				m_Value = std::move(value);
			}
			void SetValue(const Any& value)
			{
				m_Value = &value;
			}
	};
}

namespace kxf::UI::DataView
{
	class KX_API DragDropEvent: public ItemEvent
	{
		friend class MainWindow;

		public:
			KxEVENT_MEMBER(DragDropEvent, ItemDrag);
			KxEVENT_MEMBER(DragDropEvent, ItemDrop);
			KxEVENT_MEMBER(DragDropEvent, ItemDropPossible);

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
			DragDropEvent(const EventID& eventID = {}, WidgetID id = wxID_NONE)
				:ItemEvent(eventID, id)
			{
			}

		public:
			DragDropEvent* Clone() const override
			{
				return new DragDropEvent(*this);
			}

			// Drag
			wxDataObjectSimple* GetDragObject(const wxDataFormat& format) const;

			template<class T>
			T* GetDragObject(const wxDataFormat& format) const
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
			wxDataObjectSimple* GetReceivedDataObject() const;

			template<class T>
			T* GetReceivedDataObject() const
			{
				static_assert(std::is_base_of_v<wxDataObjectSimple, T>);
				return dynamic_cast<T*>(GetReceivedDataObject());
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
