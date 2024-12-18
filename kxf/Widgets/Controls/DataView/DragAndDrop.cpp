#include "kxf-pch.h"
#include "DragAndDrop.h"
#include "MainWindow.h"
#include "kxf/Widgets/Windows/SplashWindow.h"
#include "kxf/Utility/Common.h"
#include <wx/dragimag.h>

namespace kxf::UI::DataView
{
	bool DnDInfo::DoCheckOperation(const wxDataFormat& format, FlagSet<DNDOpType> desiredType) const
	{
		if (const DNDOperationInfo* info = GetOperationInfo(format))
		{
			return info->GetType().Contains(desiredType);
		}
		return false;
	}
	DnDInfo::Result DnDInfo::DoChangeOperation(const wxDataFormat& format, const DNDOperationInfo& info)
	{
		auto it = m_DataFormats.find(format);
		if (it != m_DataFormats.end())
		{
			if (info.IsOK() && format != wxDF_INVALID)
			{
				it->second.Combine(info);
				return Result::OperationChanged;
			}
			else
			{
				m_DataFormats.erase(it);
				return Result::OperationRemoved;
			}
		}
		else if (info.IsOK() && format != wxDF_INVALID)
		{
			m_DataFormats.insert_or_assign(format, info);
			return Result::OperationAdded;
		}
		return Result::None;
	}
}

namespace kxf::UI::DataView
{
	void DropSource::OnScroll(wxMouseEvent& event)
	{
		if (m_MainWindow)
		{
			View* view = m_MainWindow->GetView();

			int rateX = 0;
			int rateY = 0;
			view->GetScrollPixelsPerUnit(&rateX, &rateY);
			Point startPos = Point(view->GetViewStart());

			wxCoord value = -event.GetWheelRotation();
			if (event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
			{
				view->Scroll(wxDefaultCoord, startPos.GetY() + (float)value / (rateY != 0 ? rateY : 1));
			}
			else
			{
				view->Scroll(startPos.GetX() + (float)value / (rateX != 0 ? rateX : 1), wxDefaultCoord);
			}
		}
		event.Skip();
	}
	bool DropSource::GiveFeedback(wxDragResult effect)
	{
		Point mousePos = Point(wxGetMousePosition());
		if (!m_DragImage)
		{
			Point linePos(0, m_MainWindow->GetRowStart(m_Row));

			m_MainWindow->GetView()->CalcUnscrolledPosition(0, linePos.GetY(), nullptr, &linePos.Y());
			linePos = Point(m_MainWindow->ClientToScreen(linePos));

			int rowIndent = 0;
			m_HintBitmap = m_MainWindow->CreateItemBitmap(m_Row, rowIndent);

			m_Distance.X() = (mousePos.GetX() - linePos.GetX()) - rowIndent;
			m_Distance.Y() = mousePos.GetY() - linePos.GetY();
			m_HintPosition = GetHintPosition(mousePos);

			m_DragImage = new SplashWindow(m_MainWindow, m_HintBitmap, {}, SplashWindowStyle::None|(WindowStyle::None|TopLevelWindowStyle::StayOnTop));
			m_DragImage->SetTransparent(225);
			m_DragImage->Update();
			m_DragImage->Show();
		}
		else
		{
			m_HintPosition = GetHintPosition(mousePos);
			m_DragImage->Move(m_HintPosition);
		}
		return false;
	}

	Point DropSource::GetHintPosition(const Point& mousePos) const
	{
		return {mousePos.GetX() - m_Distance.GetX(), mousePos.GetY() + 5};
	}

	DropSource::DropSource(MainWindow* mainWindow, Row row)
		:wxDropSource(mainWindow), m_MainWindow(mainWindow), m_Row(row), m_Distance(0, 0)
	{
	}
	DropSource::~DropSource()
	{
		m_HintPosition = Point::UnspecifiedPosition();
		if (m_DragImage)
		{
			m_DragImage->Destroy();
		}
	}
}

namespace kxf::UI::DataView
{
	wxDataFormat DropTarget::GetReceivedFormat() const
	{
		return const_cast<DropTarget*>(this)->GetMatchingPair();
	}
	bool DropTarget::IsFormatSupported(const wxDataFormat& format, wxDataObjectSimple*& dataObject) const
	{
		if (format != wxDF_INVALID)
		{
			dataObject = m_MainWindow->m_DragDropDataObject->GetObject(format);
			return dataObject != nullptr;
		}
		return false;
	}

	wxDragResult DropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult dragResult)
	{
		wxDataObjectSimple* dataObject = nullptr;
		if (wxDataFormat format = GetReceivedFormat(); IsFormatSupported(format, dataObject))
		{
			return m_MainWindow->OnDragOver(*dataObject, Point(x, y), dragResult);
		}
		return wxDragNone;
	}
	bool DropTarget::OnDrop(wxCoord x, wxCoord y)
	{
		wxDataObjectSimple* dataObject = nullptr;
		if (wxDataFormat format = GetReceivedFormat(); IsFormatSupported(format, dataObject))
		{
			return m_MainWindow->TestDropPossible(*dataObject, Point(x, y));
		}
		return false;
	}
	wxDragResult DropTarget::OnData(wxCoord x, wxCoord y, wxDragResult dragResult)
	{
		wxDataObjectSimple* dataObject = nullptr;
		if (wxDataFormat format = GetReceivedFormat(); IsFormatSupported(format, dataObject))
		{
			GetData();
			return m_MainWindow->OnDropData(*dataObject, Point(x, y), dragResult);
		}
		return wxDragNone;
	}

	wxDragResult DropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult dragResult)
	{
		wxDataObjectSimple* dataObject = nullptr;
		if (wxDataFormat format = GetReceivedFormat(); GetData() && IsFormatSupported(format, dataObject))
		{
			return m_MainWindow->OnDragDropEnter(*dataObject, Point(x, y), dragResult);
		}
		return wxDragNone;
	}
	void DropTarget::OnLeave()
	{
		m_MainWindow->OnDragDropLeave();
	}

	DropTarget::DropTarget(wxDataObject* dataObject, MainWindow* mainWindow)
		:wxDropTarget(dataObject), m_MainWindow(mainWindow)
	{
	}
}
