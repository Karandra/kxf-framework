#pragma once
#include "Common.h"
#include "FSPath.h"
#include "kxf/Core/String.h"
#include "kxf/Core/DataSize.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/DateTime/DateTime.h"
#include "kxf/Serialization/BinarySerializer.h"

namespace kxf
{
	class IFileSystem;
}

namespace kxf
{
	class KXF_API FileItem final
	{
		friend struct BinarySerializer<FileItem>;

		private:
			FSPath m_Path;
			DataSize m_Size;
			DataSize m_CompressedSize;
			DateTime m_CreationTime;
			DateTime m_LastAccessTime;
			DateTime m_ModificationTime;
			UniversallyUniqueID m_UniqueID;
			FlagSet<FileAttribute> m_Attributes;
			FlagSet<ReparsePointTag> m_ReparsePointTags;

		public:
			FileItem() = default;
			FileItem(FileItem&&) noexcept = default;
			FileItem(const FileItem&) = default;

			FileItem(FSPath fullPath)
				:m_Path(std::move(fullPath))
			{
			}
			FileItem(FSPath source, const FSPath& fileName)
				:m_Path(std::move(source))
			{
				m_Path.Append(fileName);
			}
			
			FileItem(const IFileSystem& fileSystem, FSPath fullPath)
				:FileItem(std::move(fullPath))
			{
				Refresh(fileSystem);
			}
			FileItem(const IFileSystem& fileSystem, FSPath source, const FSPath& fileName)
				:FileItem(std::move(source), fileName)
			{
				Refresh(fileSystem);
			}

		public:
			// General
			FileItem& Refresh(const IFileSystem& fileSystem);
			
			bool IsValid() const noexcept
			{
				return m_Path && m_Attributes != FileAttribute::Invalid;
			}
			bool IsNormalItem() const noexcept
			{
				return IsValid() && !IsReparsePoint() && !IsCurrentOrParentDirectoryRef();
			}
			bool IsCurrentOrParentDirectoryRef() const noexcept
			{
				if (m_Path.GetLength() >= 1 && m_Path.GetLength() <= 2)
				{
					const String name = m_Path.GetName();
					return name == kxfS("..") || name == kxfS('.');
				}
				return false;
			}

			// Attributes
			FlagSet<FileAttribute> GetAttributes() const noexcept
			{
				return m_Attributes;
			}
			FileItem& SetAttributes(FlagSet<FileAttribute> attributes) noexcept
			{
				m_Attributes = attributes;
				return *this;
			}
			
			FlagSet<ReparsePointTag> GetReparsePointTags() const noexcept
			{
				return m_ReparsePointTags;
			}
			FileItem& SetReparsePointTags(FlagSet<ReparsePointTag> tags) noexcept
			{
				m_ReparsePointTags = tags;
				return *this;
			}

			bool IsDirectory() const noexcept
			{
				return m_Attributes & FileAttribute::Directory;
			}
			bool IsCompressed() const noexcept
			{
				return m_Attributes & FileAttribute::Compressed;
			}
			bool IsReparsePoint() const noexcept
			{
				return m_Attributes & FileAttribute::ReparsePoint;
			}
			bool IsSymLink() const noexcept
			{
				return IsReparsePoint() && m_ReparsePointTags & ReparsePointTag::SymLink;
			}

			// Date and time
			DateTime GetCreationTime() const noexcept
			{
				return m_CreationTime;
			}
			FileItem& SetCreationTime(DateTime value) noexcept
			{
				m_CreationTime = value;
				return *this;
			}
			
			DateTime GetLastAccessTime() const noexcept
			{
				return m_LastAccessTime;
			}
			FileItem& SetLastAccessTime(DateTime value) noexcept
			{
				m_LastAccessTime = value;
				return *this;
			}
			
			DateTime GetModificationTime() const noexcept
			{
				return m_ModificationTime;
			}
			FileItem& SetModificationTime(DateTime value) noexcept
			{
				m_ModificationTime = value;
				return *this;
			}

			// Path and name
			FSPath GetFullPath() const noexcept
			{
				return m_Path;
			}
			FileItem& SetFullPath(FSPath fullPath) noexcept
			{
				m_Path = std::move(fullPath);
				return *this;
			}

			FSPath GetSource() const noexcept
			{
				return m_Path.GetParent();
			}
			FileItem& SetSource(FSPath source) noexcept
			{
				String name = m_Path.GetName();
				m_Path = std::move(source);
				m_Path.SetName(std::move(name));

				return *this;
			}
			
			String GetName() const
			{
				return m_Path.GetName();
			}
			FileItem& SetName(const String& name)
			{
				m_Path.SetName(name);
				return *this;
			}
			
			String GetFileExtension() const
			{
				return m_Path.GetExtension();
			}
			FileItem& SetFileExtension(const String& ext)
			{
				m_Path.SetExtension(ext);
				return *this;
			}

			// Size
			DataSize GetSize() const noexcept
			{
				return m_Size;
			}
			FileItem& SetSize(DataSize size) noexcept
			{
				m_Size = size;
				return *this;
			}

			double GetCompressionRatio() const noexcept
			{
				if (IsCompressed() && m_CompressedSize)
				{
					return GetSizeRatio(m_CompressedSize, m_Size);
				}
				return 1;
			}
			DataSize GetCompressedSize() const noexcept
			{
				return m_CompressedSize;
			}
			FileItem& SetCompressedSize(DataSize size) noexcept
			{
				m_CompressedSize = size;
				return *this;
			}

			// ID
			UniversallyUniqueID GetUniqueID() const noexcept
			{
				return m_UniqueID;
			}
			FileItem& SetUniqueID(UniversallyUniqueID id) noexcept
			{
				m_UniqueID = std::move(id);
				return *this;
			}

		public:
			FileItem& operator=(FileItem&&) noexcept = default;
			FileItem& operator=(const FileItem&) = default;

			explicit operator bool() const noexcept
			{
				return IsValid();
			}
			bool operator!() const noexcept
			{
				return !IsValid();
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<FileItem> final
	{
		uint64_t Serialize(IOutputStream& stream, const FileItem& value) const;
		uint64_t Deserialize(IInputStream& stream, FileItem& value) const;
	};
}
