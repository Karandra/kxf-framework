#pragma once
#include "kxf-gui/Widgets/Common.h"
#include "kxf/EventSystem/Event.h"
#include <wx/aui/auibar.h>

namespace kxf::UI
{
	class AuiToolBarItem;
}

namespace kxf::UI
{
	class KXF_API AuiToolBarEvent: public wxAuiToolBarEvent
	{
		public:
			kxf_EVENT_MEMBER(AuiToolBarEvent, ItemClick);
			kxf_EVENT_MEMBER(AuiToolBarEvent, ItemRightClick);
			kxf_EVENT_MEMBER(AuiToolBarEvent, ItemMiddleClick);
			kxf_EVENT_MEMBER(AuiToolBarEvent, ItemDropdown);
			kxf_EVENT_MEMBER(AuiToolBarEvent, OverflowClick);

		private:
			AuiToolBarItem* m_Item = nullptr;

		public:
			AuiToolBarEvent(const EventID& type = {}, wxWindowID id = 0)
				:wxAuiToolBarEvent(type.AsInt(), id)
			{
			}
			AuiToolBarEvent(const wxAuiToolBarEvent& other)
			{
				SetId(other.GetId());
				SetInt(other.GetInt());
				SetExtraLong(other.GetExtraLong());
				SetString(other.GetString());
				SetEventType(other.GetEventType());

				SetDropDownClicked(other.IsDropDownClicked());
				SetClickPoint(other.GetClickPoint());
				SetItemRect(other.GetItemRect());
				SetToolId(other.GetToolId());
			}

		public:
			AuiToolBarEvent* Clone() const override
			{
				return new AuiToolBarEvent(*this);
			}

			AuiToolBarItem* GetItem() const
			{
				return m_Item;
			}
			void SetItem(AuiToolBarItem* item)
			{
				m_Item = item;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(AuiToolBarEvent);
	};
}

