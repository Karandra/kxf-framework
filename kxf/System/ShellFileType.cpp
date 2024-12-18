#include "kxf-pch.h"
#include "ShellFileType.h"
#include "ShellFileTypeManager.h"
#include "Registry.h"
#include "kxf/Core/RegEx.h"
#include <wx/regex.h>

namespace kxf
{
	FSPath ShellFileType::GetOpenExecutable() const
	{
		String openCommand = GetOpenCommand(NullString);
		if (RegEx regEx(u8R"(\"(.+?)\")", RegExFlag::IgnoreCase); regEx.Matches(openCommand))
		{
			return regEx.GetMatch(openCommand, 1);
		}
		return {};
	}

	bool ShellFileType::IsURLProtocol(const FSPath& extension) const
	{
		if (m_FileType && extension)
		{
			for (const String& ext: GetAllExtensions())
			{
				if (ext == extension)
				{
					RegistryKey key(RegistryRootKey::ClassesRoot, extension.GetExtension(), RegistryAccess::Read);
					return key.DoesValueExist("URL Protocol");
				}
			}
		}
		return false;
	}
}
