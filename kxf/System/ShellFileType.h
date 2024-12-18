#pragma once
#include "Common.h"
#include "kxf/Core/String.h"
#include "kxf/FileSystem/FSPath.h"
#include <wx/mimetype.h>
#include <wx/iconloc.h>

namespace kxf
{
	class KXF_API ShellFileType final
	{
		public:
			using MessageParameters = wxFileType::MessageParameters;

		public:
			static String ExpandCommand(const String &command, const MessageParameters& parameters)
			{
				return wxFileType::ExpandCommand(command, parameters);
			}

		private:
			std::unique_ptr<wxFileType> m_FileType;

		public:
			ShellFileType() = default;
			ShellFileType(wxFileType* fileType) noexcept
				:m_FileType(fileType)
			{
			}
			ShellFileType(ShellFileType&& other) noexcept
			{
				*this = std::move(other);
			}
			ShellFileType(const ShellFileType&) = delete;

		public:
			const wxFileType& AsWxFileType() const
			{
				return *m_FileType;
			}
			wxFileType& AsWxFileType()
			{
				return *m_FileType;
			}

		public:
			String GetDescription() const
			{
				wxString value;
				m_FileType->GetDescription(&value);
				return value;
			}
			String GetExtension() const
			{
				wxArrayString extensions;
				return m_FileType->GetExtensions(extensions) ? extensions.front() : "";
			}
			wxArrayString GetAllExtensions() const
			{
				wxArrayString extensions;
				m_FileType->GetExtensions(extensions);
				return extensions;
			}

			String GetMimeType() const
			{
				wxString value;
				m_FileType->GetMimeType(&value);
				return value;
			}
			wxArrayString GetAllMimeTypes() const
			{
				wxArrayString mimeTypes;
				m_FileType->GetMimeTypes(mimeTypes);
				return mimeTypes;
			}

			wxIconLocation GetIcon() const
			{
				wxIconLocation icon;
				m_FileType->GetIcon(&icon);
				return icon;
			}
			wxIconLocation GetIcon(const MessageParameters& parameters) const
			{
				wxIconLocation icon;
				m_FileType->GetIcon(&icon, parameters);
				return icon;
			}

			String GetCommand(const MessageParameters& parameters, const String& action) const
			{
				String value;
				m_FileType->GetExpandedCommand(action, parameters);
				return value;
			}
			String GetCommand(const String& filePath, const String& action) const
			{
				return GetCommand(MessageParameters(filePath), action);
			}

			String GetOpenCommand(const MessageParameters& parameters) const
			{
				wxString value;
				m_FileType->GetOpenCommand(&value, parameters);
				return value;
			}
			String GetOpenCommand(const FSPath& filePath) const
			{
				return GetOpenCommand(MessageParameters(filePath.GetFullPath()));
			}
			FSPath GetOpenExecutable() const;

			String GetPrintCommand(const MessageParameters& parameters) const
			{
				wxString value;
				m_FileType->GetPrintCommand(&value, parameters);
				return value;
			}
			String GetPrintCommand(const String& filePath) const
			{
				return GetPrintCommand(MessageParameters(filePath));
			}
		
			size_t GetAllCommands(wxArrayString& verbs, wxArrayString& commands, const MessageParameters& parameters) const
			{
				return m_FileType->GetAllCommands(&verbs, &commands, parameters);
			}
			bool IsURLProtocol(const FSPath& extension) const;

		public:
			explicit operator bool() const noexcept
			{
				return m_FileType != nullptr;
			}
			bool operator!() const noexcept
			{
				return m_FileType == nullptr;
			}

			ShellFileType& operator=(ShellFileType&& other) noexcept
			{
				m_FileType = std::move(other.m_FileType);
				return *this;
			}
			ShellFileType& operator=(const ShellFileType&) = delete;
	};
}
