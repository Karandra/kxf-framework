#include "KxfPCH.h"
#include "ShellMenuWidgetItem.h"
#include "ShellMenuWidget.h"
#include "WXUI/Menu.h"
#include "WXUI/MenuItem.h"
#include "../INativeWidget.h"
#include "kxf/Utility/String.h"
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <Shldisp.h>
#include <ShlObj.h>

namespace kxf::Widgets
{
	uint32_t ShellMenuWidgetItem::DoGetShellItemID() const
	{
		return WXUI::Menu::WxIDToWin(m_MenuItem->GetId()) - 1;
	}
	String ShellMenuWidgetItem::DoGetCommandString(uint32_t stringIndex) const
	{
		auto owningMenu = std::static_pointer_cast<ShellMenuWidget>(m_OwningMenu.lock());
		if (m_MenuItem && owningMenu && stringIndex & GCS_UNICODE)
		{
			const auto id = DoGetShellItemID();
			wchar_t buffer[std::numeric_limits<int16_t>::max()] = {};

			owningMenu->m_ShellMenu->GetCommandString(static_cast<UINT_PTR>(id), stringIndex, nullptr, reinterpret_cast<char*>(buffer), std::size(buffer) - sizeof(wchar_t));
			return buffer;
		}
		return {};
	}

	// IWidgetItem
	String ShellMenuWidgetItem::GetDescription() const
	{
		String result = MenuWidgetItem::GetDescription();
		if (!result.IsEmpty())
		{
			return result;
		}
		return DoGetCommandString(GCS_HELPTEXTW);
	}

	// IShellMenuWidgetItem
	String ShellMenuWidgetItem::GetIconString() const
	{
		return DoGetCommandString(GCS_VERBICONW);
	}
	String ShellMenuWidgetItem::GetHelpString() const
	{
		return DoGetCommandString(GCS_HELPTEXTW);
	}
	String ShellMenuWidgetItem::GetCommandString() const
	{
		return DoGetCommandString(GCS_VERBW);
	}
	uint32_t ShellMenuWidgetItem::GetShellItemID() const
	{
		if (m_MenuItem)
		{
			return DoGetShellItemID();
		}
		return std::numeric_limits<uint32_t>::max();
	}

	HResult ShellMenuWidgetItem::InvokeShellCommand(const IWidget* widget, const String& commandLine, const FSPath& workingDirectory)
	{
		auto owningMenu = std::static_pointer_cast<ShellMenuWidget>(m_OwningMenu.lock());
		if (m_MenuItem && owningMenu)
		{
			String workingDirectoryString;
			CMINVOKECOMMANDINFOEX info = {};
			info.cbSize = sizeof(info);
			info.fMask = CMIC_MASK_UNICODE;
			info.nShow = SW_SHOWNORMAL;

			if (widget)
			{
				info.hwnd = reinterpret_cast<HWND>(GetNativeHandleOf(*widget));
			}
			else if (auto invokingWidget = owningMenu->GetInvokingWidget())
			{
				info.hwnd = reinterpret_cast<HWND>(GetNativeHandleOf(*invokingWidget));
			}

			if (auto pos = owningMenu->GetInvokingPosition(); pos.IsFullySpecified())
			{
				info.ptInvoke.x = pos.GetX();
				info.ptInvoke.y = pos.GetY();
			}

			const auto id = DoGetShellItemID();
			info.lpVerb = MAKEINTRESOURCEA(id);
			info.lpVerbW = MAKEINTRESOURCEW(id);

			if (!commandLine.IsEmpty())
			{
				info.lpParameters = commandLine.nc_str();
				info.lpParametersW = commandLine.wc_str();
			}
			if (workingDirectory)
			{
				workingDirectoryString = workingDirectory.GetFullPath();
				info.lpDirectory = workingDirectoryString.nc_str();
				info.lpDirectoryW = workingDirectoryString.wc_str();
			}

			return owningMenu->m_ShellMenu->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&info));
		}
		return HResult::InvalidPointer();
	}
}
