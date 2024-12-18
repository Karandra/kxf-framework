#pragma once
#include "WidgetEvent.h"
#include "../IDataViewWidget.h"
#include "../Widgets/DataView/Row.h"
#include "../Widgets/DataView/Node.h"
#include "../Widgets/DataView/Column.h"

namespace kxf
{
	class KXF_API DataViewWidgetEvent: public WidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemSelected);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemHovered);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemActivated);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemCollapsed);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemExpanded);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemCollapsing);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemExpanding);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemContextMenu);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemValueChanged);

			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemEditStarted);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemEditFinished);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ItemEditCanceled);

			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnHeaderClick);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnHeaderRClick);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnHeaderMenuItem);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnHeaderSeparatorDClick);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnHeaderWidthFit);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnDropdown);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnToggle);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnSorted);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnMoved);

			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnBeginResize);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnResize);
			kxf_EVENT_MEMBER(DataViewWidgetEvent, ColumnEndResize);

			kxf_EVENT_MEMBER(DataViewWidgetEvent, ViewCacheHint);

		private:
			DataView::Node* m_Node = nullptr;
			DataView::Column* m_Column = nullptr;
			std::optional<Rect> m_Rect;
			std::pair<DataView::Row, DataView::Row> m_CacheHints;

			std::variant<Any, const Any*> m_Value;
			bool m_IsEditCancelled = false;

		public:
			DataViewWidgetEvent(IDataViewWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<DataViewWidgetEvent>(std::move(*this));
			}

			// DataViewWidgetEvent
			std::shared_ptr<IDataViewWidget> GetDataViewWidget() const noexcept
			{
				return GetWidget()->QueryInterface<IDataViewWidget>();
			}

		public:
			template<std::derived_from<IObject> T = IDataViewItem>
			std::shared_ptr<T> GetItem() const noexcept
			{
				return m_Node ? m_Node->GetItem<T>() : nullptr;
			}

			DataView::Node* GetNode() const noexcept
			{
				return m_Node;
			}
			void SetNode(DataView::Node* node) noexcept
			{
				m_Node = node;
			}

			DataView::Column* GetColumn() const noexcept
			{
				return m_Column;
			}
			void SetColumn(DataView::Column* column) noexcept
			{
				m_Column = column;
			}

			Rect GetRect() const noexcept
			{
				return m_Rect ? *m_Rect : Rect::UnspecifiedRect();
			}
			void SetRect(const Rect& rect) noexcept
			{
				m_Rect = rect;
			}

			Point GetPosition() const noexcept
			{
				return m_Rect ? m_Rect->GetPosition() : Point::UnspecifiedPosition();
			}
			void SetPosition(const Point& pos) noexcept
			{
				if (!m_Rect)
				{
					m_Rect = Rect::UnspecifiedRect();
				}
				m_Rect->SetPosition(pos);
			}

			Size GetSize() const noexcept
			{
				return m_Rect ? m_Rect->GetSize() : Size::UnspecifiedSize();
			}
			void SetSize(const Size& size) noexcept
			{
				if (!m_Rect)
				{
					m_Rect = Rect::UnspecifiedRect();
				}
				m_Rect->SetSize(size);
			}

			// Hints as physical rows
			std::pair<DataView::Row, DataView::Row> GetCacheHint() const noexcept
			{
				return m_CacheHints;
			}
			DataView::Row GetCacheHintFrom() const noexcept
			{
				return m_CacheHints.first;
			}
			DataView::Row GetCacheHintTo() const noexcept
			{
				return m_CacheHints.second;
			}
			void SetCacheHints(DataView::Row from, DataView::Row to) noexcept
			{
				m_CacheHints = {from, to};
			}

			// Editor
			bool IsEditCancelled() const noexcept
			{
				return m_IsEditCancelled;
			}
			void SetEditCanceled(bool editCancelled = true) noexcept
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
			Any GetValue() && noexcept
			{
				if (decltype(auto) value = std::get_if<const Any*>(&m_Value))
				{
					return **value;
				}
				else if (decltype(auto) any = std::get_if<Any>(&m_Value))
				{
					return std::move(*any);
				}
				return {};
			}

			void SetValue(Any&& value) noexcept
			{
				m_Value = std::move(value);
			}
			void SetValue(const Any& value) noexcept
			{
				m_Value = &value;
			}
	};
}

#if 0
namespace kxf::DataView
{
	class KXF_API DragDropEvent: public DataViewWidgetEvent
	{
		public:
			kxf_EVENT_MEMBER(DragDropEvent, ItemDrag);
			kxf_EVENT_MEMBER(DragDropEvent, ItemDrop);
			kxf_EVENT_MEMBER(DragDropEvent, ItemDropPossible);

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
			DragDropEvent(IDataViewWidget& widget)
				:DataViewWidgetEvent(widget)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<EditorEvent>(std::move(*this));
			}

		public:
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
#endif
