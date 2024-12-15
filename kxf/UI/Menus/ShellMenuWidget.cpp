#include "KxfPCH.h"
#include "ShellMenuWidget.h"
#include "ShellMenuWidgetItem.h"
#include "../INativeWidget.h"
#include "WXUI/Menu.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"
#include "kxf/Utility/String.h"

#include <Windows.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <Shldisp.h>
#include <ShlObj.h>

namespace
{
	constexpr std::pair g_ShellMenuIDRange = {0x1, 0x7FFF};
}

namespace kxf::Widgets
{
	// ShellMenuWidget
	void ShellMenuWidget::CopyItems(IMenuWidget& menu, void* menuHandle, size_t itemCount)
	{
		for (size_t i = 0; i < itemCount; i++)
		{
			MENUITEMINFOW itemInfo = {};
			itemInfo.cbSize = sizeof(itemInfo);
			itemInfo.fMask = MIIM_BITMAP|MIIM_CHECKMARKS|MIIM_DATA|MIIM_FTYPE|MIIM_ID|MIIM_STATE|MIIM_STRING|MIIM_SUBMENU;

			if (::GetMenuItemInfoW(static_cast<HMENU>(menuHandle), i, TRUE, &itemInfo))
			{
				// Get label for non-separator items
				String label;
				if (itemInfo.cch != 0)
				{
					Utility::StringBuffer labelBuffer(label, itemInfo.cch);
					itemInfo.dwTypeData = labelBuffer;
					itemInfo.cch += 1;

					if (!::GetMenuItemInfoW(static_cast<HMENU>(menuHandle), i, TRUE, &itemInfo))
					{
						continue;
					}
				}

				std::shared_ptr<IMenuWidgetItem> item;
				bool isInserted = false;

				if (itemInfo.hSubMenu)
				{
					// Insert our own sub-menu and copy sub-items to it
					int menuItems = ::GetMenuItemCount(itemInfo.hSubMenu);
					if (menuItems > 0)
					{
						// Create a sub-menu and copy the shell menu object to it to allow
						// 'InvokeShellCommand' to access it from the this menu's sub-items.
						auto subMenu = NewWidget<ShellMenuWidget>(menu.QueryInterface<IWidget>());
						subMenu->m_ShellMenu = m_ShellMenu;

						// Fill subitems (possibly recursively)
						CopyItems(*subMenu, itemInfo.hSubMenu, menuItems);

						item = menu.InsertMenu(*subMenu, label);
						isInserted = true;
					}
				}
				else if (itemInfo.fType & MFT_SEPARATOR)
				{
					item = menu.CreateItem({}, ItemType::Separator);
				}
				else if ((itemInfo.hbmpChecked || itemInfo.hbmpUnchecked) && (itemInfo.hbmpChecked != itemInfo.hbmpUnchecked))
				{
					if (itemInfo.fType & MFT_RADIOCHECK)
					{
						item = menu.CreateItem(label, ItemType::RadioItem);
					}
					else
					{
						item = menu.CreateItem(label, ItemType::CheckItem);
					}
				}
				else
				{
					item = menu.CreateItem(label, ItemType::Regular);
				}

				if (item)
				{
					static_cast<MenuWidgetItem&>(*item).GetWxItem()->SetId(WXUI::Menu::WinIDToWx(itemInfo.wID).ToInt());
					if (!isInserted)
					{
						menu.InsertItem(*item);
					}

					// Try to find at least some one valid bitmap
					if (itemInfo.hbmpItem || itemInfo.hbmpChecked || itemInfo.hbmpUnchecked)
					{
						auto ToBitmapImage = [](void* handle) -> BitmapImage
						{
							if (handle)
							{
								GDIBitmap bitmap;
								bitmap.AttachHandle(handle);

								BitmapImage image(bitmap);
								bitmap.DetachHandle();

								return image;
							}
							return {};
						};

						BitmapImage image = ToBitmapImage(itemInfo.hbmpItem);
						if (!image)
						{
							ToBitmapImage(itemInfo.hbmpChecked);
						}
						if (!image)
						{
							ToBitmapImage(itemInfo.hbmpUnchecked);
						}

						item->SetIcon(image);
					}

					item->SetChecked(itemInfo.fState & MFS_CHECKED);
					item->SetEnabled(!(itemInfo.fState & MFS_DISABLED));
					if (itemInfo.fState & MFS_DEFAULT)
					{
						item->SetDefaultItem();
					}
				}
			}
		}
	}

	// MenuWidget
	std::shared_ptr<MenuWidgetItem> ShellMenuWidget::DoCreateItem()
	{
		return std::make_shared<ShellMenuWidgetItem>();
	}

	// ShellMenuWidget
	ShellMenuWidget::ShellMenuWidget() = default;
	ShellMenuWidget::~ShellMenuWidget() = default;

	// IWidget
	bool ShellMenuWidget::DestroyWidget()
	{
		const bool result = MenuWidget::DestroyWidget();
		m_Initializer.Uninitialize();

		return result;
	}

	// IShellMenuWidget
	HResult ShellMenuWidget::InitializeFromFSObject(const FSPath& path)
	{
		if (m_Menu)
		{
			if (m_ShellMenu)
			{
				return HResult::InvalidStateChnage();
			}

			HResult hr = m_Initializer.Initialize(COMThreadingModel::Apartment);
			if (m_Initializer.IsInitialized())
			{
				wxMenu tempMenu;

				String pathName = path.GetFullPathWithNS();
				if (hr = ::SHParseDisplayName(pathName.wc_str(), nullptr, &m_ShellItemList, 0, nullptr))
				{
					if (hr = ::SHBindToParent(m_ShellItemList, IID_IShellFolder, m_ShellFolder.GetAddress(), &m_ShellChildItemList))
					{
						void* handle = m_InvokingWidget ? GetNativeHandleOf(*m_InvokingWidget) : nullptr;
						if (hr = m_ShellFolder->GetUIObjectOf(static_cast<HWND>(handle), 1, &m_ShellChildItemList, IID_IContextMenu, 0, m_ShellMenu.GetAddress()))
						{
							hr = m_ShellMenu->QueryContextMenu(tempMenu.GetHMenu(), 0, g_ShellMenuIDRange.first, g_ShellMenuIDRange.second, CMF_NORMAL);
						}
					}
				}

				int menuItems = ::GetMenuItemCount(tempMenu.GetHMenu());
				if (hr && menuItems > 0)
				{
					CopyItems(*this, tempMenu.GetHMenu(), menuItems);
				}
				else
				{
					return HResult::Fail();
				}
			}
			return hr;
		}
		return HResult::InvalidPointer();
	}	
}
