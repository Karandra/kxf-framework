#include "kxf-pch.h"
#include "Event.h"
#include "View.h"
#include "MainWindow.h"

namespace kxf::UI::DataView
{
	View* ItemEvent::GetView() const
	{
		return static_cast<View*>(wxNotifyEvent::GetEventObject());
	}
	MainWindow* ItemEvent::GetMainWindow() const
	{
		if (View* view = GetView())
		{
			return view->GetMainWindow();
		}
		return nullptr;
	}
}

namespace kxf::UI::DataView
{
	wxDataObjectSimple* DragDropEvent::GetDragObject(const wxDataFormat& format) const
	{
		MainWindow* mainWindow = GetMainWindow();
		if (mainWindow && mainWindow->m_DragDropDataObject)
		{
			return mainWindow->m_DragDropDataObject->GetObject(format);
		}
		return nullptr;
	}
	void DragDropEvent::DragDone(const wxDataObjectSimple& dataObject, int flags)
	{
		m_DataObject = const_cast<wxDataObjectSimple*>(&dataObject);
		m_DragFlags = flags;
	}
	void DragDropEvent::DragCancel()
	{
		m_DataObject = nullptr;
		m_DragFlags = wxDrag_CopyOnly;
	}

	wxDataObjectSimple* DragDropEvent::GetReceivedDataObject() const
	{
		return m_DataObject;
	}
	void DragDropEvent::DropDone(wxDragResult result)
	{
		m_DropResult = result;
	}
	void DragDropEvent::DropCancel()
	{
		m_DropResult = wxDragCancel;
	}
	void DragDropEvent::DropError()
	{
		m_DropResult = wxDragError;
	}
}
