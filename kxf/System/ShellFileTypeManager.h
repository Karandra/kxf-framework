#pragma once
#include "Common.h"
#include "ShellFileType.h"
#include "ShellFileTypeInfo.h"
#include "kxf/Core/String.h"
#include "kxf/FileSystem/FSPath.h"
#include <wx/mimetype.h>

namespace kxf
{
	class KXF_API ShellFileTypeManager
	{
		public:
			static bool IsOfType(const String& mimeType, const String& wildcard)
			{
				return wxMimeTypesManager::IsOfType(mimeType, wildcard);
			}

		private:
			mutable wxMimeTypesManager m_Manager;

		public:
			ShellFileType FileTypeFromExtension(const String& extension) const;
			ShellFileType FileTypeFromMimeType(const String& mimeType) const;

			ShellFileType Associate(const ShellFileTypeInfo& fileTypeInfo);
			bool IsAssociatedWith(const ShellFileType& fileType, const FSPath& executablePath) const;
			bool Unassociate(ShellFileType& fileType);

			template<class TFunc>
			size_t EnumFileTypes(TFunc&& func) const
			{
				using T = std::invoke_result_t<TFunc, const String&>;

				size_t counter = 0;
				wxArrayString mimetypes = m_Manager.EnumAllFileTypes();

				for (wxString& mime: mimetypes)
				{
					if constexpr(std::is_same_v<T, bool>)
					{
						counter++;
						if (!std::invoke(func, String(std::move(mime))))
						{
							break;
						}
					}
					else if constexpr(std::is_same_v<T, void>)
					{
						counter++;
						std::invoke(func, String(std::move(mime)));
					}
					else
					{
						static_assert(sizeof(TFunc*) == 0, "invalid return type");
					}
				}
				return counter;
			}

			void AddFallback(const ShellFileTypeInfo& fileTypeInfo)
			{
				m_Manager.AddFallback(fileTypeInfo.AsWxFileTypeInfo());
			}
	};
}
