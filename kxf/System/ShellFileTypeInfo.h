#pragma once
#include "Common.h"
#include "kxf/Core/String.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/String.h"
#include <wx/mimetype.h>
#include <wx/iconloc.h>

namespace kxf
{
	class KXF_API ShellFileTypeInfo final
	{
		private:
			wxFileTypeInfo m_FileTypeInfo;
			Utility::UnorderedMapNoCase<String, bool> m_URLProtocolMap;

		public:
			ShellFileTypeInfo() = default;
			ShellFileTypeInfo(const String& mimeType)
				:m_FileTypeInfo(mimeType)
			{
			}

			template<class... Args>
			ShellFileTypeInfo(const String& mimeType,
						   const String& openCommand,
						   const String& printCommand,
						   const String& description,
						   const FSPath& extension,
						   Args&& ... arg
			)
				:m_FileTypeInfo(mimeType, openCommand, printCommand, description, extension.GetExtension())
			{
				AddExtensions(std::forward<Args>(arg)...);
			}

			ShellFileTypeInfo(const wxFileTypeInfo& other)
				:m_FileTypeInfo(other)
			{
			}
			ShellFileTypeInfo(wxFileTypeInfo&& other)
				:m_FileTypeInfo(std::move(other))
			{
			}
			ShellFileTypeInfo(const ShellFileTypeInfo&) = delete;
			ShellFileTypeInfo(ShellFileTypeInfo&&) = delete;

		public:
			const wxFileTypeInfo& AsWxFileTypeInfo() const noexcept
			{
				return m_FileTypeInfo;
			}
			wxFileTypeInfo& AsWxFileTypeInfo() noexcept
			{
				return m_FileTypeInfo;
			}

		public:
			String GetDescription() const
			{
				return m_FileTypeInfo.GetDescription();
			}
			ShellFileTypeInfo& SetDescription(const String& value)
			{
				m_FileTypeInfo.SetDescription(value);
				return *this;
			}
		
			String GetShortDescription() const
			{
				return m_FileTypeInfo.GetShortDesc();
			}
			ShellFileTypeInfo& SetShortDescription(const String& value)
			{
				m_FileTypeInfo.SetShortDesc(value);
				return *this;
			}
		
			String GetMimeType() const
			{
				return m_FileTypeInfo.GetMimeType();
			}

			String GetOpenCommand() const
			{
				return m_FileTypeInfo.GetOpenCommand();
			}
			ShellFileTypeInfo& SetOpenCommand(const String& value)
			{
				m_FileTypeInfo.SetOpenCommand(value);
				return *this;
			}

			String GetPrintCommand() const
			{
				return m_FileTypeInfo.GetPrintCommand();
			}
			ShellFileTypeInfo& SetPrintCommand(const String& value)
			{
				m_FileTypeInfo.SetPrintCommand(value);
				return *this;
			}

			template<class TFunc>
			size_t EnumExtensions(TFunc&& func) const
			{
				size_t count = 0;
				for (const wxString& value: m_FileTypeInfo.GetExtensions())
				{
					count++;
					if (!std::invoke(func, FSPath(value).GetExtension()))
					{
						break;
					}
				}
				return count;
			}

			size_t GetExtensionsCount() const
			{
				return m_FileTypeInfo.GetExtensionsCount();
			}
			ShellFileTypeInfo& AddExtension(const FSPath& extension, bool isURLProtocol = false)
			{
				String ext = extension.GetExtension();

				m_FileTypeInfo.AddExtension(ext);
				m_URLProtocolMap.insert_or_assign(std::move(ext), isURLProtocol);
				return *this;
			}
			
			template<class... Args>
			ShellFileTypeInfo& AddExtensions(Args&&... arg)
			{
				Utility::ForEachParameterPackItem([this](FSPath ext)
				{
					AddExtension(std::move(ext));
				}, std::forward<Args>(arg)...);
				return *this;
			}

			int GetIconIndex() const
			{
				return m_FileTypeInfo.GetIconIndex();
			}
			FSPath GetIconFile() const
			{
				return String(m_FileTypeInfo.GetIconFile());
			}
			ShellFileTypeInfo& SetIcon(const FSPath& filePath, int index = 0)
			{
				m_FileTypeInfo.SetIcon(filePath.GetFullPath(), index);
				return *this;
			}

			bool IsURLProtocol(const FSPath& ext) const;
			ShellFileTypeInfo& SetURLProtocol(const FSPath& ext, bool protocol = true);

		public:
			explicit operator bool() const noexcept
			{
				return m_FileTypeInfo.IsValid();
			}
			bool operator!() const noexcept
			{
				return !m_FileTypeInfo.IsValid();
			}

			ShellFileTypeInfo& operator=(const ShellFileTypeInfo&) = delete;
			ShellFileTypeInfo& operator=(ShellFileTypeInfo&&) = delete;

			ShellFileTypeInfo& operator=(const wxFileTypeInfo& other)
			{
				m_FileTypeInfo = other;
				return *this;
			}
			ShellFileTypeInfo& operator=(wxFileTypeInfo&& other)
			{
				m_FileTypeInfo = std::move(other);
				return *this;
			}
	};
}
