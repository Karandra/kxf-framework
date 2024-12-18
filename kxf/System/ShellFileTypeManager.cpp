#include "kxf-pch.h"
#include "ShellFileTypeManager.h"
#include "Registry.h"

namespace kxf
{
	ShellFileType ShellFileTypeManager::FileTypeFromExtension(const String& extension) const
	{
		return m_Manager.GetFileTypeFromExtension(extension);
	}
	ShellFileType ShellFileTypeManager::FileTypeFromMimeType(const String& mimeType) const
	{
		return m_Manager.GetFileTypeFromMimeType(mimeType);
	}

	ShellFileType ShellFileTypeManager::Associate(const ShellFileTypeInfo& fileTypeInfo)
	{
		if (wxFileType* fileType = m_Manager.Associate(fileTypeInfo.AsWxFileTypeInfo()))
		{
			fileTypeInfo.EnumExtensions([&](String extension)
			{
				if (fileTypeInfo.IsURLProtocol(extension))
				{
					RegistryKey classesRoot(RegistryRootKey::ClassesRoot, {}, RegistryAccess::Create);
					if (classesRoot)
					{
						String ext = FSPath(std::move(extension)).GetExtension();

						RegistryKey key = classesRoot.CreateKey(ext, RegistryAccess::Write);
						key.SetStringValue({}, Format("URL:{} Protocol", ext.MakeUpper()));
						key.SetStringValue("URL Protocol", {});
					}
				}
				return true;
			});
			return fileType;
		}
		return nullptr;
	}
	bool ShellFileTypeManager::IsAssociatedWith(const ShellFileType& fileType, const FSPath& executablePath) const
	{
		if (fileType)
		{
			return fileType.GetOpenExecutable() == executablePath;
		}
		return false;
	}
	bool ShellFileTypeManager::Unassociate(ShellFileType& fileType)
	{
		if (fileType)
		{
			return m_Manager.Unassociate(&fileType.AsWxFileType());
		}
		return false;
	}
}
