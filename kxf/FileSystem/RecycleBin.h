#pragma once
#include "Common.h"
#include "FSPath.h"
#include "FileItem.h"
#include "LegacyVolume.h"
#include "kxf/Core/DataSize.h"
#include "kxf/Core/Enumerator.h"
class wxWindow;

namespace kxf
{
	class IFileSystem;
}

namespace kxf
{
	class KXF_API RecycleBin final
	{
		private:
			XChar m_Path[4] = {};
			LegacyVolume m_Volume;
			IFileSystem* m_FileSystem = nullptr;
			wxWindow* m_Window = nullptr;

		public:
			RecycleBin(LegacyVolume volume = {});
			RecycleBin(LegacyVolume volume, IFileSystem& fileSystem);

		public:
			bool IsEnabled() const;
			void SetWindow(wxWindow* window);

			DataSize GetSize() const;
			size_t GetItemCount() const;
			bool ClearItems(FlagSet<FSActionFlag> flags = {});

			FileItem GetItem(const FSPath& path) const;
			Enumerator<FileItem> EnumItems() const;

			bool Recycle(const FSPath& path, FlagSet<FSActionFlag> flags = {});
			bool Restore(const FSPath& path, FlagSet<FSActionFlag> flags = {});
	};
}
