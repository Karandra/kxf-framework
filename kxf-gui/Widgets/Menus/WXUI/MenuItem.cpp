#include "kxf-pch.h"
#include "MenuItem.h"
#include "../MenuWidget.h"
#include "../MenuWidgetItem.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/System/SystemInformation.h"

namespace kxf::WXUI
{
	bool MenuItem::OnMeasureItem(size_t* width, size_t* height)
	{
		// TODO: Fully reimplement 'wxMenuItem::OnMeasureItem' here
		wxMenuItem::OnMeasureItem(width, height);

		auto size = m_Item->OnMeasureItem(SizeF(*width, *height));
		if (size.GetWidth() != Geometry::DefaultCoord)
		{
			*width = size.GetWidth();
		}
		if (size.GetHeight() != Geometry::DefaultCoord)
		{
			*height = size.GetHeight();
		}
		return true;
	}
	bool MenuItem::OnDrawItem(wxDC& dc, const wxRect& rect, wxODAction action, wxODStatus status)
	{
		FlagSet<NativeWidgetFlag> flags;
		flags.Add(NativeWidgetFlag::Selected, status & wxODStatus::wxODSelected);
		flags.Add(NativeWidgetFlag::Disabled, status & wxODStatus::wxODGrayed);
		flags.Add(NativeWidgetFlag::Disabled, status & wxODStatus::wxODDisabled);
		flags.Add(NativeWidgetFlag::Checked, status & wxODStatus::wxODChecked);
		flags.Add(NativeWidgetFlag::Focused, status & wxODStatus::wxODHasFocus);
		flags.Add(NativeWidgetFlag::DefaultItem, status & wxODStatus::wxODDefault);

		auto renderer = m_Item->m_OwningMenu.lock()->GetActiveGraphicsRenderer();

		auto texture = renderer->CreateTexture(Size(rect.GetSize()), Drawing::GetStockColor(StockColor::Transparent));
		auto gc = renderer->CreateLegacyContext(texture, GetWindow());
		m_Item->OnDrawItem(std::move(gc), Rect(rect.GetSize()), flags);

		dc.DrawBitmap(texture->ToBitmapImage().ToGDIBitmap().ToWxBitmap(), rect.GetPosition());
		return true;
	}

	MenuItem::MenuItem(std::shared_ptr<Widgets::MenuWidgetItem> item, std::shared_ptr<IMenuWidget> subMenuRef, wxMenu& subMenu) noexcept
		:wxMenuItem(nullptr, wxID_ANY, {}, {}, wxITEM_NORMAL, &subMenu), m_SubMenu(std::move(subMenuRef)), m_Item(std::move(item))
	{
	}
	MenuItem::MenuItem(std::shared_ptr<Widgets::MenuWidgetItem> item, wxItemKind itemKind) noexcept
		:wxMenuItem(nullptr, (itemKind == wxITEM_SEPARATOR ? wxID_SEPARATOR : wxID_ANY), {}, {}, itemKind), m_Item(std::move(item))
	{
	}
	MenuItem::~MenuItem()
	{
		m_Item->OnWXMenuDestroyed();
	}

	wxWindow* MenuItem::GetWindow() const
	{
		if (wxMenu* menu = GetMenu())
		{
			return menu->GetWindow();
		}
		return nullptr;
	}
}
