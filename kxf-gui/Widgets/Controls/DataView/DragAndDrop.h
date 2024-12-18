#pragma once
#include "Common.h"
#include "Row.h"
#include <wx/dnd.h>

class wxDragImage;
namespace kxf::UI
{
	class SplashWindow;
}
namespace kxf::UI::DataView
{
	class MainWindow;
}

namespace kxf::UI::DataView
{
	class KXF_API DNDOperationInfo final
	{
		private:
			wxDataObjectSimple* m_DropDataObject = nullptr;
			FlagSet<DNDOpType> m_Type;
			bool m_IsPreferedDrop = false;

		public:
			DNDOperationInfo() = default;
			DNDOperationInfo(wxDataObjectSimple& dataObject, FlagSet<DNDOpType> type, bool isPreferredDrop = false)
				:m_DropDataObject(&dataObject), m_Type(type), m_IsPreferedDrop(isPreferredDrop)
			{
			}

		public:
			bool IsOK() const
			{
				return m_DropDataObject && m_Type != DNDOpType::None;
			}
			FlagSet<DNDOpType> GetType() const
			{
				return m_Type;
			}
			wxDataObjectSimple& GetDataObject() const
			{
				return *m_DropDataObject;
			}

			void Combine(const DNDOperationInfo& other)
			{
				m_Type = m_Type|other.m_Type;
				m_DropDataObject = other.m_DropDataObject;
				m_IsPreferedDrop = other.m_IsPreferedDrop;
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API DnDInfo final
	{
		public:
			enum class Result
			{
				None,
				OperationAdded,
				OperationChanged,
				OperationRemoved,
			};

		private:
			std::map<wxDataFormat, DNDOperationInfo> m_DataFormats;

		private:
			bool DoCheckOperation(const wxDataFormat& format, FlagSet<DNDOpType> desiredType) const;
			Result DoChangeOperation(const wxDataFormat& format, const DNDOperationInfo& info);

		public:
			DNDOperationInfo* GetOperationInfo(const wxDataFormat& format)
			{
				if (auto it = m_DataFormats.find(format); it != m_DataFormats.end())
				{
					return &it->second;
				}
				return nullptr;
			}
			const DNDOperationInfo* GetOperationInfo(const wxDataFormat& format) const
			{
				return const_cast<DnDInfo*>(this)->GetOperationInfo(format);
			}

			bool IsDragEnabled(const wxDataFormat& format) const
			{
				return DoCheckOperation(format, DNDOpType::Drag);
			}
			bool IsDropEnabled(const wxDataFormat& format) const
			{
				return DoCheckOperation(format, DNDOpType::Drop);
			}
			bool IsFullEnabled(const wxDataFormat& format) const
			{
				return DoCheckOperation(format, DNDOpType::Drag|DNDOpType::Drop);
			}

			Result EnableOperation(wxDataObjectSimple& dataObject, DNDOpType type, bool isPreferredDrop = false)
			{
				return DoChangeOperation(dataObject.GetFormat(), DNDOperationInfo(dataObject, type, isPreferredDrop));
			}
			Result DisableOperations(const wxDataFormat& format)
			{
				return DoChangeOperation(format, {});
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API DropSource: public wxDropSource
	{
		private:
			MainWindow* m_MainWindow = nullptr;
			SplashWindow* m_DragImage = nullptr;
			GDIBitmap m_HintBitmap;
			Point m_HintPosition;

			Point m_Distance = Point::UnspecifiedPosition();
			Row m_Row;

		private:
			void OnScroll(wxMouseEvent& event);
			bool GiveFeedback(wxDragResult effect) override;

			Point GetHintPosition(const Point& mousePos) const;

		public:
			DropSource(MainWindow* mainWindow, Row row);
			~DropSource();
	};
}

namespace kxf::UI::DataView
{
	class KXF_API DropTarget: public wxDropTarget
	{
		private:
			MainWindow* m_MainWindow = nullptr;

		protected:
			wxDataFormat GetReceivedFormat() const;
			bool IsFormatSupported(const wxDataFormat& format, wxDataObjectSimple*& dataObject) const;

			wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult dragResult) override;
			bool OnDrop(wxCoord x, wxCoord y) override;
			wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult dragResult) override;

			wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult dragResult) override;
			void OnLeave() override;

		public:
			DropTarget(wxDataObject* dataObject, MainWindow* mainWindow);
	};
}
