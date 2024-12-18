#pragma once
#include "../Common.h"
#include <wx/menu.h>
#include <wx/menuitem.h>

namespace kxf
{
	class IMenuWidget;
}
namespace kxf::Widgets
{
	class MenuWidgetItem;
}

namespace kxf::WXUI
{
	class KXF_API MenuItem: public wxMenuItem
	{
		private:
			std::shared_ptr<IMenuWidget> m_SubMenu;
			std::shared_ptr<Widgets::MenuWidgetItem> m_Item;

		protected:
			bool OnMeasureItem(size_t* width, size_t* height) override;
			bool OnDrawItem(wxDC& dc, const wxRect& rect, wxODAction action, wxODStatus status) override;

		public:
			MenuItem(std::shared_ptr<Widgets::MenuWidgetItem> item, std::shared_ptr<IMenuWidget> subMenuRef, wxMenu& subMenu) noexcept;
			MenuItem(std::shared_ptr<Widgets::MenuWidgetItem> item, wxItemKind itemKind) noexcept;
			~MenuItem();

		public:
			wxWindow* GetWindow() const;
	};
}
