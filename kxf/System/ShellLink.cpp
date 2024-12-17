#include "KxfPCH.h"
#include "ShellLink.h"
#include "Private/Shell.h"
#include "kxf/Utility/Memory.h"
#include <ShObjIDL.h>
#include "UndefWindows.h"

namespace kxf
{
	ShellLink::ShellLink() noexcept
	{
		COMPtr<IShellLinkW> shellLink;
		if (COM::CreateInstance(CLSID_ShellLink, COMClassContext::InprocServer, &shellLink))
		{
			m_ShellLink = std::move(shellLink);
		}
	}
	ShellLink::~ShellLink() noexcept
	{
	}

	HResult ShellLink::Load(const FSPath& path)
	{
		HResult hr = E_INVALIDARG;

		if (path)
		{
			COMPtr<IPersistFile> persistFile;
			if (hr = m_ShellLink->QueryInterface(&persistFile))
			{
				String pathString = path.GetFullPath();
				return persistFile->Load(pathString.wc_str(), 0);
			}
		}
		return hr;
	}
	HResult ShellLink::Save(const FSPath& path) const
	{
		HResult hr = E_INVALIDARG;

		if (path)
		{
			COMPtr<IPersistFile> persistFile;
			if (hr = m_ShellLink->QueryInterface(&persistFile))
			{
				String pathString = path.GetFullPath();
				return persistFile->Save(pathString.wc_str(), TRUE);
			}
		}
		return hr;
	}

	FSPath ShellLink::GetTarget() const
	{
		XChar buffer[INT16_MAX] = {};
		WIN32_FIND_DATAW findData = {};
		if (HResult(m_ShellLink->GetPath(buffer, std::size(buffer), &findData, 0)))
		{
			return buffer;
		}
		return {};
	}
	HResult ShellLink::SetTarget(const FSPath& path)
	{
		String pathString = path.GetFullPath();
		return m_ShellLink->SetPath(pathString.wc_str());
	}

	String ShellLink::GetArguments() const
	{
		XChar buffer[INT16_MAX] = {};
		if (HResult(m_ShellLink->GetArguments(buffer, std::size(buffer))))
		{
			return buffer;
		}
		return {};
	}
	HResult ShellLink::SetArguments(const String& value)
	{
		return m_ShellLink->SetArguments(value.wc_str());
	}

	FSPath ShellLink::GetWorkingDirectory() const
	{
		XChar buffer[INT16_MAX] = {};
		if (HResult(m_ShellLink->GetWorkingDirectory(buffer, std::size(buffer))))
		{
			return buffer;
		}
		return {};
	}
	HResult ShellLink::SetWorkingDirectory(const FSPath& path)
	{
		String pathString = path.GetFullPath();
		return m_ShellLink->SetWorkingDirectory(pathString.wc_str());
	}

	String ShellLink::GetDescription() const
	{
		XChar buffer[INFOTIPSIZE] = {};
		if (HResult(m_ShellLink->GetDescription(buffer, std::size(buffer))))
		{
			return buffer;
		}
		return {};
	}
	HResult ShellLink::SetDescription(const String& value)
	{
		return m_ShellLink->SetDescription(value.wc_str());
	}

	FSPath ShellLink::GetIconLocation() const
	{
		int index = 0;
		if (HResult(m_ShellLink->GetIconLocation(nullptr, 0, &index)))
		{
			wchar_t path[std::numeric_limits<int16_t>::max()] = {};
			if (HResult(m_ShellLink->GetIconLocation(path, std::size(path), &index)))
			{
				return path;
			}
		}
		return {};
	}
	HResult ShellLink::SetIconLocation(const FSPath& path, int index)
	{
		String pathString = path.GetFullPath();
		return m_ShellLink->SetIconLocation(pathString.wc_str(), index);
	}
	std::optional<int> ShellLink::GetIconIndex() const noexcept
	{
		int index = -1;
		if (HResult(m_ShellLink->GetIconLocation(nullptr, 0, &index)))
		{
			return index;
		}
		return {};
	}

	FlagSet<SHWindowCommand> ShellLink::GetShowCommand() const noexcept
	{
		int command = 0;
		if (HResult(m_ShellLink->GetShowCmd(&command)))
		{
			return Shell::Private::MapSHWindowCommand(command);
		}
		return {};
	}
	HResult ShellLink::SetShowCommand(FlagSet<SHWindowCommand> command) noexcept
	{
		return m_ShellLink->SetShowCmd(Shell::Private::MapSHWindowCommand(command).value_or(SW_SHOWNORMAL));
	}

	wxKeyEvent ShellLink::GetHotKey() const noexcept
	{
		wxKeyEvent keyState;

		WORD hotKeys = 0;
		if (HResult(m_ShellLink->GetHotkey(&hotKeys)))
		{
			Utility::CompositeInteger<BYTE> hotKeyData(hotKeys);
			keyState.m_keyCode = hotKeyData.GetLow();

			auto modifiers = hotKeyData.GetHigh();
			keyState.m_controlDown = modifiers & HOTKEYF_CONTROL;
			keyState.m_altDown = modifiers & HOTKEYF_ALT;
			keyState.m_shiftDown = modifiers & HOTKEYF_SHIFT;
		}
		return keyState;
	}
	HResult ShellLink::SetHotKey(const wxKeyEvent& keyState) noexcept
	{
		FlagSet<BYTE> modifiers;
		modifiers.Add(HOTKEYF_CONTROL, keyState.ControlDown());
		modifiers.Add(HOTKEYF_ALT, keyState.AltDown());
		modifiers.Add(HOTKEYF_SHIFT, keyState.ShiftDown());

		return m_ShellLink->SetHotkey(*Utility::CompositeInteger(*modifiers, static_cast<BYTE>(keyState.GetKeyCode())));
	}
}
