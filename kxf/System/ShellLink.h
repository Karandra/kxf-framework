#pragma once
#include "Common.h"
#include "COM.h"
#include "HResult.h"
#include "kxf/FileSystem/FSPath.h"
#include <wx/event.h>
struct IShellLinkW;

namespace kxf
{
	class KXF_API ShellLink final
	{
		private:
			COMPtr<IShellLinkW> m_ShellLink;

		public:
			ShellLink() noexcept;
			ShellLink(const ShellLink&) = delete;
			~ShellLink() noexcept;

		public:
			bool IsNull() const noexcept
			{
				return !m_ShellLink;
			}
			HResult Load(const FSPath& path);
			HResult Save(const FSPath& path) const;

			FSPath GetTarget() const;
			HResult SetTarget(const FSPath& value);

			String GetArguments() const;
			HResult SetArguments(const String& value);

			FSPath GetWorkingDirectory() const;
			HResult SetWorkingDirectory(const FSPath& value);

			String GetDescription() const;
			HResult SetDescription(const String& value);

			FSPath GetIconLocation() const;
			HResult SetIconLocation(const FSPath& path, int index = 0);
			std::optional<int> GetIconIndex() const noexcept;

			FlagSet<SHWindowCommand> GetShowCommand() const noexcept;
			HResult SetShowCommand(FlagSet<SHWindowCommand> command) noexcept;

			wxKeyEvent GetHotKey() const noexcept;
			HResult SetHotKey(const wxKeyEvent& keyState) noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			ShellLink& operator=(const ShellLink&) = delete;
	};
}
